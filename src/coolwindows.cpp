#pragma once

#include "coolwindows.h"



int lua_DoWinInput(lua_State* state)
{
	LUA->CheckType(1, GarrysMod::Lua::Type::LIGHTUSERDATA);
	LUA->CheckType(2, GarrysMod::Lua::Type::NUMBER);
	LUA->CheckType(3, GarrysMod::Lua::Type::NUMBER);


	LPARAM lparam = 0;
	if (LUA->IsType(4, GarrysMod::Lua::Type::NUMBER))
	{
		lparam = (LPARAM)LUA->GetNumber(4);
		if (LUA->IsType(5, GarrysMod::Lua::Type::NUMBER))
		{
			lparam = MAKELPARAM(LUA->GetNumber(4), LUA->GetNumber(5));
		}
	}
	
	LUA->PushBool(PostMessageA((HWND)LUA->GetUserdata(1), (WPARAM)LUA->GetNumber(2), (UINT)LUA->GetNumber(3), lparam));
	return 1;
}

int lua_FindWindow(lua_State* state)
{

	const char*cl = LUA->GetString(1);
	const char*win = LUA->GetString(2);
	
	if (cl && strlen(cl) == 0)
		cl = 0;

	if (win && strlen(win) == 0)
		win = 0;

	if (!win&&!cl)
	{
		LUA->PushNil();
		return 1;
	}

	void*window = FindWindow(cl, win);

	if (!window)
		LUA->PushNil();
	else
		LUA->PushUserdata(window);

	return 1;
}

int lua_FindWindowEx(lua_State* state)
{
	LUA->CheckType(1, GarrysMod::Lua::Type::LIGHTUSERDATA);

	const char*cl = LUA->GetString(2);
	const char*win = LUA->GetString(3);

	if (cl && strlen(cl) == 0)
		cl = 0;

	if (win && strlen(win) == 0)
		win = 0;

	if (!win&&!cl)
	{
		LUA->PushNil();
		return 1;
	}

	void* window = FindWindowEx((HWND)LUA->GetUserdata(1), NULL, cl, win);

	if (window)
		LUA->PushUserdata(window);
	else
		LUA->PushNil();

	return 1;
}

int lua_GetWindowRendererEX(lua_State *state)
{
	//finish me when nice way of getting the data to the game is found ty
	LUA->ThrowError("getwindowrenderex is not done yet!");

	return 1;
}

int lua_GetWindowRender(lua_State *state)
{

	LUA->CheckType(1, GarrysMod::Lua::Type::LIGHTUSERDATA);

	if (LUA->IsType(2, GarrysMod::Lua::Type::BOOL) && LUA->GetBool(2))
	{
		return lua_GetWindowRendererEX(state);
	}

	HWND window = (HWND)LUA->GetUserdata(1);
	
	
	RECT rc;
	GetClientRect(window, &rc);

	//create
	HDC hdc = GetDC(window);
	HDC memhdc = CreateCompatibleDC(hdc);

	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	HBITMAP hbmp = CreateCompatibleBitmap(hdc,
		width, height);

	SelectObject(memhdc, hbmp);
	BitBlt(memhdc, 0, 0, width, height, hdc, 0, 0, SRCCOPY);
//	OpenClipboard(NULL);
	//EmptyClipboard();
//	SetClipboardData(CF_BITMAP, hbmp);
	//CloseClipboard();

	DeleteObject(hbmp);
	ReleaseDC(HWND_DESKTOP, hdc);
	ReleaseDC(NULL, memhdc);
	DeleteDC(memhdc);
	DeleteDC(hdc);

	return 1;
}


GMOD_MODULE_OPEN()
{

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
	//todo: give lua all WM_ stuff like WM_CHAR
	LUA->CreateTable();

		LUA->PushCFunction(lua_DoWinInput);
		LUA->SetField(-2, "DoWinInput");

		LUA->PushCFunction(lua_FindWindow);
		LUA->SetField(-2, "FindWindow");

		LUA->PushCFunction(lua_FindWindowEx);
		LUA->SetField(-2, "FindWindowEx");

		LUA->PushCFunction(lua_GetWindowRender);
		LUA->SetField(-2, "GetWindowRender");

	LUA->SetField(-2, "CWIN");
	LUA->Pop();

	return 1;
}

GMOD_MODULE_CLOSE()
{

	return 1;
}