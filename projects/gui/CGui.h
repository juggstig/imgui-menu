#pragma once

#include "CTextures.h"

#include <d3d11.h>
#include <vector>

enum ImFonts_ : int
{
	ImFont_Main = 0,
	ImFont_Icons
};

enum MenuPages_ : int
{
	MenuPage_Home,
	MenuPage_Self,
	MenuPage_Players,
	MenuPage_Misc,
	MenuPage_Scripts,
	MenuPage_Appearance,
	MenuPage_Updates,

	MenuPages_COUNT
};

class CGui
{
private:
	bool m_bDrawPage[MenuPages_COUNT];

public:
	CGui();
	~CGui();

	bool Initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* device_context);
	void Shutdown();
	void Render(POINTS window_size);

	bool MsgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	ID3D11Device* GetDevice() { return m_pDevice; };
	ID3D11DeviceContext* GetDeviceContext() { return m_pDeviceContext; };
	HWND GetWindow() { return m_hWnd; };

private:
	HWND m_hWnd;
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;

private:
	CTextures* m_pTextures;
};
