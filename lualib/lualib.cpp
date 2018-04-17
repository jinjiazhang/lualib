#include "lualib.h"

const char* luaL_lasterr(lua_State* L)
{
    lua_getfield(L, LUA_REGISTRYINDEX, "__error");
    return lua_tostring(L, -1);
}

void luaL_seterr(lua_State* L, const char* fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    lua_pushvfstring(L, fmt, argp);
    va_end(argp);

    lua_writestringerror("%s\n", lua_tostring(L, -1));
    lua_setfield(L, LUA_REGISTRYINDEX, "__error");
}

bool luaL_pushfunc(lua_State* L, const char* name)
{
    lua_getglobal(L, name);
    if (!lua_isfunction(L, -1)) {
        luaL_seterr(L, "luaL_pushfunc not a function, name=%s", name);
        lua_pop(L, 1);
        return false;
    }
    return true;
}

bool luaL_pushfunc(lua_State* L, void* obj, const char* name)
{
    lua_pushlightuserdata(L, obj);
    lua_gettable(L, LUA_REGISTRYINDEX);
    if (!lua_istable(L, -1)) {
        luaL_seterr(L, "luaL_pushfunc step1 not a table, object=%p", obj);
        lua_pop(L, 1);
        return false;
    }

    lua_getfield(L, -1, name);
    if (!lua_isfunction(L, -1)) {
        luaL_seterr(L, "luaL_pushfunc step2 not a function, object=%p, name=%s", obj, name);
        lua_pop(L, 2);
        return false;
    }
    lua_remove(L, -2);
    return true;
}

bool luaL_pushfunc(lua_State* L, const char* module, const char* name)
{
    lua_getglobal(L, module);
    if (!lua_istable(L, -1)) {
        luaL_seterr(L, "luaL_pushfunc step1 not a table, module=%s", module);
        lua_pop(L, 1);
        return false;
    }
        
    lua_getfield(L, -1, name);
    if (!lua_isfunction(L, -1)) {
        luaL_seterr(L, "luaL_pushfunc step2 not a function, module=%s, name=%s", module, name);
        lua_pop(L, 2);
        return false;
    }
    lua_remove(L, -2);
    return true;
}

bool luaL_safecall(lua_State* L, int nargs, int nrets)
{
    int func = lua_gettop(L) - nargs;
    if (func <= 0 || !lua_isfunction(L, func)) {
        luaL_seterr(L, "luaL_safecall attempt to call a %s", lua_typename(L, func));
        return false;
    }

    luaL_pushfunc(L, "debug", "traceback");
    lua_insert(L, func);

    if (lua_pcall(L, nargs, nrets, func))
    {
        luaL_seterr(L, "luaL_safecall exception: %s", lua_tostring(L, -1));
        lua_pop(L, 2);
        return false;
    }
    lua_remove(L, -nrets - 1);
    return true;
}