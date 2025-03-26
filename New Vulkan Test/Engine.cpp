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
	GetSurfaceFormats();
	CreateSwapchain();
	GetSwapchainImages();
}

Engine::~Engine()
{
	DestroySwapchain();
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

	deviceExtensions.push_back("VK_KHR_swapchain");

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

void Engine::GetSurfaceFormats()
{
	uint32_t surfaceFormatSupportCount = 0;
	VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatSupportCount, nullptr));
	surfaceFormats.resize(surfaceFormatSupportCount);
	VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatSupportCount, surfaceFormats.data()));
}

void Engine::CreateSwapchain()
{
	swapchainImageCount = 2;
	if (swapchainImageCount < surfaceCapabilities.minImageCount)
		throw std::exception{};

	swapchainFormatSpace.format = VK_FORMAT_R8G8B8A8_SRGB;
	swapchainFormatSpace.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

	bool swapchainFormatFound = false;
	for (const auto& surfaceFormat : surfaceFormats)
	{
		if (surfaceFormat.format == swapchainFormatSpace.format &&
			surfaceFormat.colorSpace == swapchainFormatSpace.colorSpace)
		{
			swapchainFormatFound = true;
			break;
		}
	}

	if (!swapchainFormatFound)
		throw std::exception{};

	swapchainCreateInfo = {
		VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		nullptr,
		0,
		surface,
		swapchainImageCount,
		swapchainFormatSpace.format,
		swapchainFormatSpace.colorSpace,
		{surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height},
		1,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_SHARING_MODE_EXCLUSIVE,
		0,
		nullptr,
		surfaceCapabilities.currentTransform,
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		VK_PRESENT_MODE_FIFO_KHR,
		VK_TRUE,
		VK_NULL_HANDLE
	};

	VK_ASSERT(vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain));
}

void Engine::GetSwapchainImages()
{
	swapchainImages.resize(swapchainImageCount);

	VK_ASSERT(vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages.data()));
}

void Engine::Render()
{
}

void Engine::DestroySwapchain()
{
	if (swapchain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(device, swapchain, nullptr);
		swapchain = VK_NULL_HANDLE;
	}
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
