#include "lobject.h"

lobject::lobject(lua_State* L)
{
	this->L = L;
	link_object();
}

lobject::~lobject()
{
	unlink_object();
	this->L = nullptr;
}

const luaL_Reg* lobject::get_libs()
{
	static const luaL_Reg libs[] = {
		{ NULL, NULL },
	};
	return libs;
}

void lobject::link_object()
{
	// _R[this] = { __this = nullprt }
	lua_pushlightuserdata(L, this);
	lua_newtable(L);
	lua_pushlightuserdata(L, nullptr);
	lua_setfield(L, -2, "__this");
	lua_settable(L, LUA_REGISTRYINDEX);
}

void lobject::unlink_object()
{
	// _R[this].__this = nullprt
	lua_pushlightuserdata(L, this);
	lua_gettable(L, LUA_REGISTRYINDEX);
	lua_pushlightuserdata(L, nullptr);
	lua_setfield(L, -2, "__this");
	lua_pop(L, 1);

	// _R[this] = nil
	lua_pushlightuserdata(L, this);
	lua_pushnil(L);
	lua_settable(L, LUA_REGISTRYINDEX);
}

bool lua_islobject(lua_State* L, int idx)
{
	if (!lua_istable(L, idx)) {
		return false;
	}

	lua_getfield(L, idx, "__this");
	if (!lua_isuserdata(L, -1)) {
		lua_pop(L, 1);
		return false;
	}
	lua_pop(L, 1);
	return true;
}

void* lua_tolobject(lua_State* L, int idx)
{
	if (!lua_istable(L, idx)) {
		return nullptr;
	}
	
	lua_getfield(L, idx, "__this");
	if (!lua_isuserdata(L, -1)) {
		lua_pop(L, 1);
		return nullptr;
	}

	void* obj = lua_touserdata(L, -1);
	lua_pop(L, 1);
	return obj;
}

void lua_pushlobject(lua_State* L, void* p)
{
	lua_pushlightuserdata(L, p);
	lua_gettable(L, LUA_REGISTRYINDEX);

	// _R[p].__this == nullprt
	if (lua_islobject(L, -1) && !lua_tolobject(L, -1))
	{
		lua_pushlightuserdata(L, p);
		lua_gettable(L, LUA_REGISTRYINDEX);
		lobject* obj = static_cast<lobject*>(p);
		luaL_setfuncs(L, obj->get_libs(), 1);

		lua_pushlightuserdata(L, obj);
		lua_setfield(L, -2, "__this");
	}
}