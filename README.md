# lualib
C call lua function and lua call C, Cpp function wrapping

```C
// luapp.h
class luapp : public lobject
{
public:
    luapp();
    ~luapp();

    virtual const luaL_Reg* get_libs();

public:
    int init();
    int64_t get_time();
    void mov_time(int64_t offset);
    
private:
    int64_t time_offset_;
}
```

```C
// lua.cpp
luapp::luapp() : lobject(luaL_newstate())
{
    time_offset_ = 0;
}

luapp::~luapp()
{
    lua_close(L);
    L = NULL;
}

int luapp::init()
{
    luaL_openlibs(L);
    lua_pushlobject(L, this);
    lua_setglobal(L, "app");
    if (luaL_dofile(L, "main.lua"))
    {
        printf("%s\n", lua_tostring(L, -1));
        return -1;
    }

    int t1, t2, offset;
    luaL_callfunc(L, this, "test", std::tie(t1, t2, offset), 5);
    printf("%d\t%d\t%d\n", t1, t2, offset);
    return 0;
}

int64_t luapp::get_time()
{
    int64_t time = 0;
    luaL_callfunc(L, "os", "time", std::tie(time));
    return time + time_offset_;
}

void luapp::mov_time(int64_t offset)
{
    time_offset_ += offset;
}

EXPORT_OFUNC(luapp, get_time)
EXPORT_OFUNC(luapp, mov_time)
const luaL_Reg* luapp::get_libs()
{
    static const luaL_Reg libs[] = {
        { IMPORT_OFUNC(luapp, get_time) },
        { IMPORT_OFUNC(luapp, mov_time) },
        { NULL, NULL }
    };
    return libs;
}
```

```lua
-- main.lua
app.test = function (offset)
	local t1 = app.get_time()
	app.mov_time(offset)
	local t2 = app.get_time()
	return t1, t2, t2 - t1
end
```

```lua
-- (new luapp())->init()
-- result:
1523960116      1523960121      5
```
