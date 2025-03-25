#include "Engine.h"

Engine::Engine(HINSTANCE hInstance, HWND hWnd) : hInstance(hInstance), hWnd(hWnd)
{
	CreateInstance();
	GetPhysicalDevices();
	CreateSurface(hInstance, hWnd);
	CheckSurfacePresentationSupport();
	CreateDevice();
	GetQueue();
	GetDeviceSurfaceCapabilities();
}

Engine::~Engine()
{
	DestroyDevice();
	DestroySurface();
	DestroyInstance();
}

void Engine::CreateInstance()
{
	appInfo = {
	VK_STRUCTURE_TYPE_APPLICATION_INFO,
	nullptr,
	"New Vulkan Test",
	0,
	"New Vulkan Test Engine",
	0,
	VK_API_VERSION_1_0
	};

	instanceLayers.push_back("VK_LAYER_KHRONOS_validation");
	instanceExtensions.push_back("VK_KHR_surface");
	instanceExtensions.push_back("VK_KHR_win32_surface");

	instanceCreateInfo = {
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		nullptr,
		0,
		&appInfo,
		static_cast<uint32_t>(instanceLayers.size()),
		instanceLayers.data(),
		static_cast<uint32_t>(instanceExtensions.size()),
		instanceExtensions.data()
	};

	VK_ASSERT(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));
}

void Engine::GetPhysicalDevices()
{
	uint32_t numPhysicalDevices = 0;
	uint32_t numPhysicalDeviceQueueFamilies = 0;

	VK_ASSERT(vkEnumeratePhysicalDevices(instance, &numPhysicalDevices, nullptr));
	physicalDevices.resize(numPhysicalDevices);
	VK_ASSERT(vkEnumeratePhysicalDevices(instance, &numPhysicalDevices, physicalDevices.data()));

	physicalDevice = physicalDevices[0];	// Selecting first physical device

	vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
	vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);

	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &numPhysicalDeviceQueueFamilies, nullptr);
	physicalDeviceQueueFamilyProperties.resize(numPhysicalDeviceQueueFamilies);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &numPhysicalDeviceQueueFamilies, physicalDeviceQueueFamilyProperties.data());

	for (uint32_t i = 0; i < physicalDeviceQueueFamilyProperties.size(); i++) {
		if ((physicalDeviceQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
			(physicalDeviceQueueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT))
		{
			selectedQueueFamilyIndex = i;
			break;
		}
	}
}

void Engine::CreateSurface(HINSTANCE hInstance, HWND hWnd)
{
	surfaceCreateInfo = {
		VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		nullptr,
		0,
		hInstance,
		hWnd
	};

	VK_ASSERT(vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface));
}

void Engine::CheckSurfacePresentationSupport()
{
	VkBool32 surfaceSupport = VK_FALSE;

	VK_ASSERT(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, selectedQueueFamilyIndex, surface, &surfaceSupport));

	if (surfaceSupport != VK_TRUE)
	{
		throw std::exception{};
	}
}

void Engine::GetDeviceSurfaceCapabilities()
{
	VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities));
}

void Engine::CreateDevice()
{
	float queuePriorities[] = { 1.0f };

	deviceQueueCreateInfos.push_back({
		VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		nullptr,
		0,
		selectedQueueFamilyIndex,
		1,
		queuePriorities
		});

	deviceCreateInfo = {
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		nullptr,
		0,
		1,
		deviceQueueCreateInfos.data(),
		0,
		nullptr,
		static_cast<uint32_t>(deviceExtensions.size()),
		deviceExtensions.data(),
		&physicalDeviceFeatures
	};

	VK_ASSERT(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device));
}

void Engine::GetQueue()
{
	vkGetDeviceQueue(device, selectedQueueFamilyIndex, 0, &queue);
}

void Engine::Render()
{
}

void Engine::DestroyDevice()
{
	if (device != VK_NULL_HANDLE)
	{
		vkDestroyDevice(device, nullptr);
		device = VK_NULL_HANDLE;
	}
}

void Engine::DestroySurface()
{
	if (surface != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR(instance, surface, nullptr);
		surface = VK_NULL_HANDLE;
	}
}

void Engine::DestroyInstance()
{
	if (instance != VK_NULL_HANDLE)
	{
		vkDestroyInstance(instance, nullptr);
		instance = VK_NULL_HANDLE;
	}
}
