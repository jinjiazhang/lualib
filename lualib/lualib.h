#ifndef _JINJIAZHANG_LUALIB_H_
#define _JINJIAZHANG_LUALIB_H_

#include <tuple>
#include <string>
#include <functional>

#include "lua/lua.hpp"
// #include "protolog/protolog.h"

#define IMPORT_CFUNC(method) \
    #method, lua_##method

#define EXPORT_CFUNC(method) \
int lua_##method(lua_State* L) \
{ \
    static cfunc f = make_luafunc(method); \
    return f(L); \
}

#define IMPORT_OFUNC(class, method) \
    #method, lua_##class##_##method

#define EXPORT_OFUNC(class, method) \
int lua_##class##_##method(lua_State* L) \
{ \
    static ofunc f = make_luafunc(&class::method); \
    class* obj = luaL_getvalue<class*>(L, lua_upvalueindex(1)); \
    if (obj == nullptr) { \
        luaL_error(L, "lua_"#class"_"#method" obj is nullptr\n"); \
        return 0; \
    } \
    return f(obj, L); \
}

#ifdef _JINJIAZHANG_PROTOLOG_H_
#define lualib_trace(fmt, ...)  log_trace(fmt, __VA_ARGS__)
#define lualib_debug(fmt, ...)  log_debug(fmt, __VA_ARGS__)
#define lualib_info(fmt, ...)   log_info(fmt, __VA_ARGS__)
#define lualib_warn(fmt, ...)   log_warn(fmt, __VA_ARGS__)
#define lualib_error(fmt, ...)  log_error(fmt, __VA_ARGS__)
#define lualib_fatal(fmt, ...)  log_fatal(fmt, __VA_ARGS__)
#else
#define lualib_trace(fmt, ...)
#define lualib_debug(fmt, ...)
#define lualib_info(fmt, ...)
#define lualib_warn(fmt, ...)
#define lualib_error(fmt, ...)
#define lualib_fatal(fmt, ...)
#endif

#define LUAPP_DO(exp) { if(!(exp)) return false; }

int   lua_emptyfunc(lua_State* L);
bool  lua_islobject(lua_State* L, int idx);
void* lua_tolobject(lua_State* L, int idx);
void  lua_pushlobject(lua_State* L, void* obj);


template <typename T> 
inline T luaL_getvalue(lua_State* L, int i) { return (T)lua_tolobject(L, i); }
template <> inline bool luaL_getvalue<bool>(lua_State* L, int i) { return lua_toboolean(L, i) != 0; }
template <> inline char luaL_getvalue<char>(lua_State* L, int i) { return (char)lua_tointeger(L, i); }
template <> inline unsigned char luaL_getvalue<unsigned char>(lua_State* L, int i) { return (unsigned char)lua_tointeger(L, i); }
template <> inline short luaL_getvalue<short>(lua_State* L, int i) { return (short)lua_tointeger(L, i); }
template <> inline unsigned short luaL_getvalue<unsigned short>(lua_State* L, int i) { return (unsigned short)lua_tointeger(L, i); }
template <> inline int luaL_getvalue<int>(lua_State* L, int i) { return (int)lua_tointeger(L, i); }
template <> inline unsigned int luaL_getvalue<unsigned int>(lua_State* L, int i) { return (unsigned int)lua_tointeger(L, i); }
template <> inline long luaL_getvalue<long>(lua_State* L, int i) { return (long)lua_tointeger(L, i); }
template <> inline unsigned long luaL_getvalue<unsigned long>(lua_State* L, int i) { return (unsigned long)lua_tointeger(L, i); }
template <> inline long long luaL_getvalue<long long>(lua_State* L, int i) { return lua_tointeger(L, i); }
template <> inline unsigned long long luaL_getvalue<unsigned long long>(lua_State* L, int i) { return (unsigned long long)lua_tointeger(L, i); }
template <> inline float luaL_getvalue<float>(lua_State* L, int i) { return (float)lua_tonumber(L, i); }
template <> inline double luaL_getvalue<double>(lua_State* L, int i) { return lua_tonumber(L, i); }
template <> inline const char* luaL_getvalue<const char*>(lua_State* L, int i) { return lua_tostring(L, i); }
template <> inline std::string luaL_getvalue<std::string>(lua_State* L, int i) { size_t len; const char* str = lua_tolstring(L, i, &len); return std::string(str, len); }

template <typename T> 
inline void luaL_pushvalue(lua_State* L, T* v) { lua_pushlobject(L, v); }
inline void luaL_pushvalue(lua_State* L, bool v) { lua_pushboolean(L, v); }
inline void luaL_pushvalue(lua_State* L, char v) { lua_pushinteger(L, v); }
inline void luaL_pushvalue(lua_State* L, unsigned char v) { lua_pushinteger(L, v); }
inline void luaL_pushvalue(lua_State* L, short v) { lua_pushinteger(L, v); }
inline void luaL_pushvalue(lua_State* L, unsigned short v) { lua_pushinteger(L, v); }
inline void luaL_pushvalue(lua_State* L, int v) { lua_pushinteger(L, v); }
inline void luaL_pushvalue(lua_State* L, unsigned int v) { lua_pushinteger(L, v); }
inline void luaL_pushvalue(lua_State* L, long v) { lua_pushinteger(L, v); }
inline void luaL_pushvalue(lua_State* L, unsigned long v) { lua_pushinteger(L, v); }
inline void luaL_pushvalue(lua_State* L, long long v) { lua_pushinteger(L, (lua_Integer)v); }
inline void luaL_pushvalue(lua_State* L, unsigned long long v) { lua_pushinteger(L, (lua_Integer)v); }
inline void luaL_pushvalue(lua_State* L, float v) { lua_pushnumber(L, v); }
inline void luaL_pushvalue(lua_State* L, double v) { lua_pushnumber(L, v); }
inline void luaL_pushvalue(lua_State* L, char* v) { lua_pushstring(L, v); }
inline void luaL_pushvalue(lua_State* L, const char* v) { lua_pushstring(L, v); }
inline void luaL_pushvalue(lua_State* L, const std::string& v) { lua_pushlstring(L, v.c_str(), v.size()); }

template <size_t... ints>
struct luapp_sequence { };

template <size_t size, size_t... ints>
struct make_luapp_sequence : make_luapp_sequence<size - 1, size - 1, ints...> { };

template <size_t... ints>
struct make_luapp_sequence<0, ints...> : luapp_sequence<ints...> { };

bool luaL_pushfunc(lua_State* L, const char* name);

bool luaL_pushfunc(lua_State* L, void* obj, const char* name);

bool luaL_pushfunc(lua_State* L, const char* module, const char* name);

bool luaL_safecall(lua_State* L, int nargs, int nrets);

template<typename ...types>
inline bool luaL_pushargs(lua_State* L, types ...args)
{
    int _[] = { 0, (luaL_pushvalue(L, args), 0)... };
    return true;
}

template<size_t ...ints, typename ...types>
inline bool luaL_getrets(lua_State* L, std::tuple<types&...>& rets, luapp_sequence<ints...>&&)
{
    int _[] = { 0, (std::get<ints>(rets) = luaL_getvalue<types>(L, ints - sizeof...(ints)), 0)... };
    return true;
}

template<typename ...TTT, typename ...types>
inline bool luaL_callfunc(lua_State* L, const char* func, std::tuple<TTT&...>&& rets, types ...args)
{
    LUAPP_DO(luaL_pushfunc(L, func));
    LUAPP_DO(luaL_pushargs(L, args...));
    LUAPP_DO(luaL_safecall(L, sizeof...(types), sizeof...(TTT)));
    LUAPP_DO(luaL_getrets(L, rets, make_luapp_sequence<sizeof...(TTT)>()));
    return true;
}

template<typename ...TTT, typename ...types>
inline bool luaL_callfunc(lua_State* L, void* obj, const char* func, std::tuple<TTT&...>&& rets, types ...args)
{
    LUAPP_DO(luaL_pushfunc(L, obj, func));
    LUAPP_DO(luaL_pushargs(L, args...));
    LUAPP_DO(luaL_safecall(L, sizeof...(types), sizeof...(TTT)));
    LUAPP_DO(luaL_getrets(L, rets, make_luapp_sequence<sizeof...(TTT)>()));
    return true;
}

template<typename ...TTT, typename ...types>
inline bool luaL_callfunc(lua_State* L, const char* module, const char* func, std::tuple<TTT&...>&& rets, types ...args)
{
    LUAPP_DO(luaL_pushfunc(L, module, func));
    LUAPP_DO(luaL_pushargs(L, args...));
    LUAPP_DO(luaL_safecall(L, sizeof...(types), sizeof...(TTT)));
    LUAPP_DO(luaL_getrets(L, rets, make_luapp_sequence<sizeof...(TTT)>()));
    return true;
}

template<typename ...types>
inline bool luaL_callfunc(lua_State* L, const char* func, types ...args) { return luaL_callfunc(L, func, std::tie(), args...);  }

template<typename ...types>
inline bool luaL_callfunc(lua_State* L, void* obj, const char* func, types ...args) { return luaL_callfunc(L, obj, func, std::tie(), args...); }

template<typename ...types>
inline bool luaL_callfunc(lua_State* L, const char* module, const char* func, types ...args) { return luaL_callfunc(L, module, func, std::tie(), args...); }


using cfunc = std::function<int(lua_State*)>;
using ofunc = std::function<int(void*, lua_State*)>;

template<size_t ...ints, typename T, typename ...types>
T call_cppfunc(lua_State* L, T(*func)(types...), luapp_sequence<ints...>&&)
{
    return (*func)(luaL_getvalue<types>(L, ints + 1)...);
}

template<size_t ...ints, typename T, class C, typename ...types>
T call_cppfunc(lua_State* L, C* obj, T(C::*func)(types...), luapp_sequence<ints...>&&)
{
    return (obj->*func)(luaL_getvalue<types>(L, ints + 1)...);
}

template<typename ...types>
cfunc make_luafunc(void(*func)(types...))
{
    return [=](lua_State* L)
    {
        call_cppfunc(L, func, make_luapp_sequence<sizeof...(types)>());
        return 0;
    };
}

template<typename T, typename ...types>
cfunc make_luafunc(T(*func)(types...))
{
    return [=](lua_State* L)
    {
        T ret = call_cppfunc(L, func, make_luapp_sequence<sizeof...(types)>());
        luaL_pushvalue(L, ret);
        return 1;
    };
}

template<class C>
ofunc make_luafunc(int(C::*func)(lua_State*))
{
    return [=](void* obj, lua_State* L)
    {
        return (((C*)obj)->*func)(L);
    };
}

template<class C, typename ...types>
ofunc make_luafunc(void(C::*func)(types...))
{
    return [=](void* obj, lua_State* L)
    {
        call_cppfunc(L, (C*)obj, func, make_luapp_sequence<sizeof...(types)>());
        return 0;
    };
}

template<class C, typename T, typename ...types>
ofunc make_luafunc(T(C::*func)(types...))
{
    return [=](void* obj, lua_State* L)
    {
        T ret = call_cppfunc(L, (C*)obj, func, make_luapp_sequence<sizeof...(types)>());
        luaL_pushvalue(L, ret);
        return 1;
    };
}

#endif