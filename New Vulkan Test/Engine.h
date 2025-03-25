#pragma once

#define VK_USE_PLATFORM_WIN32_KHR

#include <exception>
#include <vector>
#include <vulkan/vulkan.h>

#define VK_ASSERT(x) if (x != VK_SUCCESS) { throw std::exception{}; }

class Engine
{
public:
	Engine(HINSTANCE hInstance, HWND hWnd);
	~Engine();

	void Render();
private:
	void CreateInstance();
	void GetPhysicalDevices();
	void CreateSurface(HINSTANCE hInstance, HWND hWnd);
	void CheckSurfacePresentationSupport();
	void GetDeviceSurfaceCapabilities();
	void CreateDevice();
	void GetQueue();
	void GetSurfaceFormats();

	void DestroyDevice();
	void DestroySurface();
	void DestroyInstance();

	HINSTANCE hInstance = NULL;
	HWND hWnd = NULL;

	uint32_t selectedQueueFamilyIndex = -1;

	VkInstance instance = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	VkQueue queue = VK_NULL_HANDLE;

	VkApplicationInfo appInfo{};
	VkInstanceCreateInfo instanceCreateInfo{};
	std::vector<VkPhysicalDevice> physicalDevices{};
	VkPhysicalDeviceProperties physicalDeviceProperties{};
	VkPhysicalDeviceFeatures physicalDeviceFeatures{};
	VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties{};
	std::vector<VkQueueFamilyProperties> physicalDeviceQueueFamilyProperties{};
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
	VkDeviceCreateInfo deviceCreateInfo{};
	std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos{};
	VkSurfaceCapabilitiesKHR surfaceCapabilities{};
	std::vector<VkSurfaceFormatKHR> surfaceFormats{};

	std::vector<const char*> instanceLayers{};
	std::vector<const char*> instanceExtensions{};
	std::vector<const char*> deviceExtensions{};
};

