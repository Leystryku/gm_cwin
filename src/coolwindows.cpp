//See copyright notice in LICENSE
#pragma once

#include "coolwindows.h"
char gameroot[MAX_PATH];


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

int lua_ShowWindow(lua_State* state)
{
	LUA->CheckType(1, GarrysMod::Lua::Type::LIGHTUSERDATA);
	LUA->CheckType(2, GarrysMod::Lua::Type::NUMBER);

	LUA->PushBool(ShowWindow((HWND)LUA->GetUserdata(1), (UINT)LUA->GetNumber(2)));
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


class CProceduralRegenerator : public ITextureRegenerator
{
public:
	CProceduralRegenerator(void) {};
	virtual void RegenerateTextureBits(ITexture *pTexture, IVTFTexture *pVTFTexture, Rect_t *pSubRect);
	virtual void Release(void);

	HWND window;
};



void CProceduralRegenerator::RegenerateTextureBits(ITexture *pTexture, IVTFTexture *pVTFTexture, Rect_t *pSubRect)
{
	//create

	if (!window)
		return;

	HDC hdc = GetDC(window);
	if (!hdc)
		return;

	HDC memhdc = CreateCompatibleDC(hdc);

	RECT rect;
	GetClientRect(window, &rect);

	if (!rect.right || !rect.bottom)
		return;

	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;


	BITMAPINFO livebmpinfo;
	livebmpinfo.bmiHeader.biSize = sizeof(livebmpinfo.bmiHeader);
	livebmpinfo.bmiHeader.biWidth = rect.right;
	livebmpinfo.bmiHeader.biHeight = rect.bottom;
	livebmpinfo.bmiHeader.biPlanes = 1;
	livebmpinfo.bmiHeader.biBitCount = 32;
	livebmpinfo.bmiHeader.biCompression = BI_RGB;
	livebmpinfo.bmiHeader.biSizeImage = rect.right * 4 * rect.bottom;
	livebmpinfo.bmiHeader.biClrUsed = 0;
	livebmpinfo.bmiHeader.biClrImportant = 0;


	BYTE*imagebits;
	HBITMAP hbmp = CreateDIBSection(memhdc, &livebmpinfo, DIB_RGB_COLORS, (void**)(&imagebits), NULL, NULL);


	SelectObject(memhdc, hbmp);
	BitBlt(memhdc, 0, 0, width, height, hdc, 0, 0, SRCCOPY | CAPTUREBLT);

	if (!imagebits)
	{
		return;
	}

	CPixelWriter pixelWriter;
	pixelWriter.SetPixelMemory(pVTFTexture->Format(), pVTFTexture->ImageData(0, 0, 0), pVTFTexture->RowSizeInBytes(0));

	int xmax = pSubRect->x + pSubRect->width;
	int ymax = pSubRect->y + pSubRect->height;
	int x, y;

	for (y = pSubRect->y; y < ymax; ++y)
	{
		pixelWriter.Seek(pSubRect->x, y);
		for (x = pSubRect->x; x < xmax; ++x)
		{
			if (x >= width || y >= height) {
				pixelWriter.WritePixel(x, y, 0, 255);
				continue;
			}
			int num = ((x) + (height-y-1)*width) * 4;
			int b = (int)imagebits[num];
			int g = (int)imagebits[num + 1];
			int r = (int)imagebits[num + 2];
			int a = (int)imagebits[num + 3];


			pixelWriter.WritePixel(r, g, b, a);
		}
	}

	DeleteObject(hbmp);
	ReleaseDC(HWND_DESKTOP, hdc);
	ReleaseDC(NULL, memhdc);
	DeleteDC(memhdc);
	DeleteDC(hdc);

}

void CProceduralRegenerator::Release()
{

}


int lua_GetWindowRendererEX(lua_State *state)//because stuff like ppspp etc
{
	//finish me when nice way of getting the data to the game is found ty
	LUA->ThrowError("getwindowrenderex is not done yet!");

	return 1;
}

int lua_GetWindowRender(lua_State *state)
{

	LUA->CheckType(1, GarrysMod::Lua::Type::LIGHTUSERDATA);
	LUA->CheckType(2, GarrysMod::Lua::Type::TEXTURE);

	if (LUA->IsType(3, GarrysMod::Lua::Type::BOOL) && LUA->GetBool(3))
	{
		return lua_GetWindowRendererEX(state);
	}
	GarrysMod::Lua::UserData*ud = LUA->GetUserdata(2);
	ITexture *txt = (ITexture*)ud->data;
	if (!txt)
	{
		LUA->ThrowError("no texture");
		return 1;
	}


	HWND window = (HWND)LUA->GetUserdata(1);



	CProceduralRegenerator *regen = new CProceduralRegenerator();
	regen->window = window;

	txt->SetTextureRegenerator(regen);

	return 1;
}

#define errhandler

PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp)
{
	BITMAP bmp;
	PBITMAPINFO pbmi;
	WORD    cClrBits;

	// Retrieve the bitmap color format, width, and height.  
	if (!GetObjectA(hBmp, sizeof(BITMAP), (LPSTR)&bmp))
		return 0;

	// Convert the color format to a count of bits.  
	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
	if (cClrBits == 1)
		cClrBits = 1;
	else if (cClrBits <= 4)
		cClrBits = 4;
	else if (cClrBits <= 8)
		cClrBits = 8;
	else if (cClrBits <= 16)
		cClrBits = 16;
	else if (cClrBits <= 24)
		cClrBits = 24;
	else cClrBits = 32;

	// Allocate memory for the BITMAPINFO structure. (This structure  
	// contains a BITMAPINFOHEADER structure and an array of RGBQUAD  
	// data structures.)  

	if (cClrBits < 24)
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
		sizeof(BITMAPINFOHEADER) +
		sizeof(RGBQUAD) * (1 << cClrBits));

	// There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel 

	else
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
		sizeof(BITMAPINFOHEADER));

	// Initialize the fields in the BITMAPINFO structure.  

	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = bmp.bmWidth;
	pbmi->bmiHeader.biHeight = bmp.bmHeight;
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
	if (cClrBits < 24)
		pbmi->bmiHeader.biClrUsed = (1 << cClrBits);

	// If the bitmap is not compressed, set the BI_RGB flag.  
	pbmi->bmiHeader.biCompression = BI_RGB;

	// Compute the number of bytes in the array of color  
	// indices and store the result in biSizeImage.  
	// The width must be DWORD aligned unless the bitmap is RLE 
	// compressed. 
	pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits + 31) & ~31) / 8
		* pbmi->bmiHeader.biHeight;
	// Set biClrImportant to 0, indicating that all of the  
	// device colors are important.  
	pbmi->bmiHeader.biClrImportant = 0;
	return pbmi;
}


int CreateBMPFile(LPTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC)
{
	HANDLE hf;                 // file handle  
	BITMAPFILEHEADER hdr;       // bitmap file-header  
	PBITMAPINFOHEADER pbih;     // bitmap info-header  
	LPBYTE lpBits;              // memory pointer  
	DWORD dwTotal;              // total count of bytes  
	DWORD cb;                   // incremental count of bytes  
	BYTE *hp;                   // byte pointer  
	DWORD dwTmp;

	pbih = (PBITMAPINFOHEADER)pbi;
	lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

	if (!lpBits)
		return 0;

	GetDIBits(hDC, hBMP, 0, (WORD)pbih->biHeight, lpBits, pbi, DIB_RGB_COLORS);

	// Create the .BMP file.  
	hf = CreateFile(pszFile,
		GENERIC_READ | GENERIC_WRITE,
		(DWORD)0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL);

	if (hf == INVALID_HANDLE_VALUE)
		return 0;

	hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
	// Compute the size of the entire file.  
	hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) +
		pbih->biSize + pbih->biClrUsed
		* sizeof(RGBQUAD) + pbih->biSizeImage);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;

	// Compute the offset to the array of color indices.  
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) +
		pbih->biSize + pbih->biClrUsed
		* sizeof(RGBQUAD);

	WriteFile(hf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER),
		(LPDWORD)&dwTmp, NULL);

	WriteFile(hf, (LPVOID)pbih, sizeof(BITMAPINFOHEADER)
		+ pbih->biClrUsed * sizeof(RGBQUAD),
		(LPDWORD)&dwTmp, (NULL));

	dwTotal = cb = pbih->biSizeImage;
	hp = lpBits;
	WriteFile(hf, (LPSTR)hp, (int)cb, (LPDWORD)&dwTmp, NULL);
	CloseHandle(hf);


	// Free memory.  
	GlobalFree((HGLOBAL)lpBits);

	return 1;
}

char coolwindowsfile[MAX_PATH];
int lua_GetWindowRenderToFile(lua_State *state)
{

	LUA->CheckType(1, GarrysMod::Lua::Type::LIGHTUSERDATA);

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
	BitBlt(memhdc, 0, 0, width, height, hdc, 0, 0, SRCCOPY | CAPTUREBLT);

	CreateBMPFile(coolwindowsfile, CreateBitmapInfoStruct(hbmp), hbmp, hdc);


	DeleteObject(hbmp);
	ReleaseDC(HWND_DESKTOP, hdc);
	ReleaseDC(NULL, memhdc);
	DeleteDC(memhdc);
	DeleteDC(hdc);

	return 1;
}


int lua_IsHungAppWindow(lua_State* state)
{
	LUA->CheckType(1, GarrysMod::Lua::Type::LIGHTUSERDATA);

	LUA->PushBool(IsHungAppWindow((HWND)LUA->GetUserdata(1)));
	return 1;
}

GMOD_MODULE_OPEN()
{
	memset(gameroot, 0, MAX_PATH);

	GetCurrentDirectory(MAX_PATH, gameroot);
	strcat(coolwindowsfile, gameroot);
	strcat(coolwindowsfile, "\\garrysmod\\coolwindows.bmp");

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
	//start give lua all WM_ stuff like WM_CHAR

	LUA->CreateTable();
#include "wmlist.h"
	LUA->SetField(-2, "WM");
	LUA->Pop();

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
	//end give lua all WM_ stuff like WM_CHAR

	LUA->CreateTable();

#include "swlist.h"

	LUA->SetField(-2, "SW");
	LUA->Pop();

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
	LUA->CreateTable();


	LUA->PushCFunction(lua_DoWinInput);
	LUA->SetField(-2, "DoWinInput");

	LUA->PushCFunction(lua_ShowWindow);
	LUA->SetField(-2, "ShowWindow");


	LUA->PushCFunction(lua_FindWindow);
	LUA->SetField(-2, "FindWindow");

	LUA->PushCFunction(lua_FindWindowEx);
	LUA->SetField(-2, "FindWindowEx");

	LUA->PushCFunction(lua_GetWindowRender);
	LUA->SetField(-2, "GetWindowRender");

	LUA->PushCFunction(lua_GetWindowRenderToFile);
	LUA->SetField(-2, "GetWindowRenderToFile");

	LUA->PushCFunction(lua_IsHungAppWindow);
	LUA->SetField(-2, "IsHungAppWindow");

	LUA->SetField(-2, "CWIN");
	LUA->Pop();

	return 1;
}

GMOD_MODULE_CLOSE()
{
	return 1;
}