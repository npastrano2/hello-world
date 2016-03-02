/*
Name: Nicholas Pastrano
GAME 2330 - 001 
Homework: PG107 P1
Description: This program creates a window and loads a bitmap image on it, using RECT it stuff the bitmap into
				specified dimensions
*/

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <time.h>
#include <iostream>
using namespace std;

const string APPTITLE = "Load Bitmap Program";
const int SCREENW = 1366;
const int SCREENH = 768;

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

LPDIRECT3D9 d3d = NULL; 
LPDIRECT3DDEVICE9 d3ddev = NULL; 
LPDIRECT3DSURFACE9 backbuffer = NULL;
LPDIRECT3DSURFACE9 surface = NULL;

bool gameover = false;

bool Game_Init(HWND window)
{
    d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (!d3d)
    {
        MessageBox(window, "Error initializing Direct3D", "Error", MB_OK);
        return false;
    }

    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount = 1;
    d3dpp.BackBufferWidth = SCREENW;
    d3dpp.BackBufferHeight = SCREENH;
    d3dpp.hDeviceWindow = window;

    d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);

    if (!d3ddev)
    {
        MessageBox(window, "Error creating Direct3D device", "Error", MB_OK);
        return 0;
    }

    d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
    
    HRESULT result = d3ddev->CreateOffscreenPlainSurface(
        SCREENW,            
        SCREENH,            
        D3DFMT_X8R8G8B8,    
        D3DPOOL_DEFAULT,    
        &surface,           
        NULL);              

    if (!SUCCEEDED(result)) return false;

    result = D3DXLoadSurfaceFromFile(
        surface,            
        NULL,               
        NULL,               
        "legotron.bmp",     
        NULL,               
        D3DX_DEFAULT,       
        0,                  
        NULL);              

    if (!SUCCEEDED(result)) return false;

    return true;
}

void Game_Run(HWND hwnd)
{
    if (!d3ddev) return;

    d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);

    if (d3ddev->BeginScene())
    {
		RECT rect;
		
		//int r = rand() % 255; 
		//int g = rand() % 255; 
		//int b = rand() % 255; 
		//d3ddev->ColorFill(surface, NULL, D3DCOLOR_XRGB(r, g, b)); 

		//draws multiple times at random sizes
		rect.left = rand() % SCREENW/2; 
		rect.top = rand() % SCREENH; 
		rect.right = rect.left + rand() % SCREENW/2; 
		rect.bottom = rect.top + rand() % SCREENH/2; 
		
		//draws once, at 100x90pixel
		//rect.left = 100; 
		//rect.top = 90; 
		//rect.right = 200; 
		//rect.bottom = 180;


        //draws surface to backbuffer
        d3ddev->StretchRect(surface, NULL, backbuffer, &rect, D3DTEXF_NONE);


     
        //stop rendering
        d3ddev->EndScene();
        d3ddev->Present(NULL, NULL, NULL, NULL);
    }

    if (KEY_DOWN(VK_ESCAPE))
        PostMessage(hwnd, WM_DESTROY, 0, 0);
}

void Game_End(HWND hwnd)
{
    if (surface) surface->Release();
    if (d3ddev) d3ddev->Release();
    if (d3d) d3d->Release();
}

LRESULT WINAPI WinProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            gameover = true;
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc( hWnd, msg, wParam, lParam );
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    //initialize window settings
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX); 
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = (WNDPROC)WinProc;
    wc.cbClsExtra	 = 0;
    wc.cbWndExtra	 = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = NULL;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = APPTITLE.c_str();
    wc.hIconSm       = NULL;
    RegisterClassEx(&wc);

    //create a new window
    HWND window = CreateWindow( APPTITLE.c_str(), APPTITLE.c_str(),
       WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
       SCREENW, SCREENH, NULL, NULL, hInstance, NULL);

    //was there an error creating the window?
    if (window == 0) return 0;

    //display the window
    ShowWindow(window, nCmdShow);
    UpdateWindow(window);
	
	//initialize the game
    if (!Game_Init(window)) return 0;


    // main message loop
	MSG message;
	while (!gameover)
    {
        if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) 
	    {
		    TranslateMessage(&message);
		    DispatchMessage(&message);
	    }

        //process game loop 
        Game_Run(window);
    }

	return message.wParam;
}