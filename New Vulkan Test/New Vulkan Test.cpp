#define VK_USE_PLATFORM_WIN32_KHR

#include "Engine.h"
#include <process.h>
#include <iostream>
#include <memory>
#include <Windows.h>
#include <vulkan/vulkan.h>

static bool terminating = false;

struct SharedData {
	HWND hwnd{};
	HINSTANCE hInstance{};
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	short wheelDelta = 0;

	switch (msg) {
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_MOUSEWHEEL:
		wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		break;
		//case WM_KEYDOWN:
		//	if (wParam == 32)
		//		sharedData->switchAutoRotationState();
		//	break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}

HWND initWindow(int windowResolutionX, int windowResolutionY) {
	HWND windowHandle = NULL;
	HINSTANCE hInstance = GetModuleHandle(NULL);

	static const char windowClassName[] = "Vulkan New Test Window Class";
	static const char windowName[] = "Vulkan New Test";
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = windowClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
		return NULL;


	DWORD dwStyle = WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX);
	DWORD dwExStyle = WS_EX_CLIENTEDGE;

	RECT windowRect{ 0,0,windowResolutionX, windowResolutionY };

	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

	windowHandle = CreateWindowEx(
		dwExStyle,
		windowClassName,
		windowName,
		dwStyle,
		CW_USEDEFAULT, CW_USEDEFAULT,
		windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
		NULL, NULL, hInstance, nullptr);


	return windowHandle;
}

unsigned __stdcall engineThreadProc(void* data) {
	SharedData sharedData(*reinterpret_cast<SharedData*>(data));

	std::unique_ptr<Engine> engine = std::make_unique<Engine>(sharedData.hInstance, sharedData.hwnd);

	while (!terminating) {
		engine->Render();
	}

	return 0;
}

int main()
{
	HWND hwnd = initWindow(1024, 768);

	HANDLE hEngineThread;
	unsigned engineThreadID;

	SharedData sharedData{ hwnd, GetModuleHandle(NULL) };

	hEngineThread = (HANDLE)_beginthreadex(NULL, 0, &engineThreadProc, reinterpret_cast<void*>(&sharedData), 0, &engineThreadID);

	ShowWindow(hwnd, SW_SHOWNORMAL);
	UpdateWindow(hwnd);

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	terminating = true;

	WaitForSingleObject(hEngineThread, INFINITE);
	CloseHandle(hEngineThread);

	return EXIT_SUCCESS;
}
