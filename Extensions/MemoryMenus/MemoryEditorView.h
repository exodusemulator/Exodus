#ifndef __MEMORYEDITORVIEW_H__
#define __MEMORYEDITORVIEW_H__
#include "WindowsSupport/WindowsSupport.pkg"
#include "DeviceInterface/DeviceInterface.pkg"
#include "MemoryEditorViewPresenter.h"
#include "Memory/MemoryRead.h"

class MemoryEditorView :public ViewBase
{
public:
	// Constructors
	MemoryEditorView(IUIManager& uiManager, MemoryEditorViewPresenter& presenter, IMemory& model);

protected:
	// Member window procedure
	virtual LRESULT WndProcWindow(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	// Constants
	static const long long HexEditControlID = 100;

private:
	// Event handlers
	LRESULT msgWM_CREATE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_DESTROY(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_TIMER(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_COMMAND(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SIZE(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_PAINT(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_SETFOCUS(HWND hwnd, WPARAM wParam, LPARAM lParam);
	LRESULT msgWM_KILLFOCUS(HWND hwnd, WPARAM wParam, LPARAM lParam);

private:
	MemoryEditorViewPresenter& _presenter;
	IMemory& _model;
	HWND _hwndMem;
};

#endif
