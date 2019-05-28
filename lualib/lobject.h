#ifndef _JINJIAZHANG_LOBJECT_H_
#define _JINJIAZHANG_LOBJECT_H_

#include "lualib.h"

class lobject
{
public:
    lobject(lua_State* L);
    virtual ~lobject();

    virtual const luaL_Reg* get_libs();
    
private:
    void link_object();
    void unlink_object();

protected:
    lua_State* L;
};

#endif