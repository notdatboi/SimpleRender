#include "Base.h"
#include <filesystem>
#include <rapidjson/document.h>

namespace sr
{
    const std::set<std::string> Base::SupportedFeatureHolder::supportedFeaturesStr =
    {
    };

    const std::set<std::string>& Base::SupportedFeatureHolder::getSupportedFeatures()
    {
        return supportedFeaturesStr;
    }

    VkPhysicalDeviceFeatures Base::SupportedFeatureHolder::strToFeatures(const std::set<std::string>& strFeatures, const VkPhysicalDeviceFeatures& availableFeatures)
    {
        VkPhysicalDeviceFeatures features{};
        for (const auto& feature : strFeatures)
        {

        }
        return features;
    }

    Base::Base(std::shared_ptr<Filesystem> filesystem):
        m_vulkanVersion(VK_MAKE_VERSION(1, 1, 0)),
        m_vkDevice(),
        m_vkInstance(),
        m_vkPhysicalDevice(),
        m_filesystem(std::move(filesystem))
    {
        m_systemConfig.Parse(m_filesystem->getFileContents(m_filesystem->getSystemConfigPath() / "sysconf.json").c_str());
        createVkInstance();
    }

    void Base::createVkInstance()
    {
        VkApplicationInfo appInfo
        {
            VK_STRUCTURE_TYPE_APPLICATION_INFO,
            nullptr,
            "",
            VK_MAKE_VERSION(1, 0, 0),
            "notdatboi's SimpleRender",
            VK_MAKE_VERSION(1, 0, 0),
            m_vulkanVersion
        };
        VkInstanceCreateInfo instanceInfo
        {
            VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            nullptr,
            {},
            &appInfo,
            0, nullptr,
            0, nullptr
        };
        vkCreateInstance(&instanceInfo, nullptr, &m_vkInstance);
    }

    bool Base::pickPhysicalDevice()
    {
        uint32_t deviceCount;
        vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, nullptr);
        if (deviceCount == 0)
        {
            return false;
        }
        std::vector<VkPhysicalDevice> devices(deviceCount);
        std::vector<char> rates(deviceCount, 0);
        vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, devices.data());

        unsigned index = 0;
        for (const auto& device : devices)
        {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(device, &properties);

            if (properties.apiVersion < m_vulkanVersion)
            {
                Logger::log<LogLevel::Info>(std::string("Not Vulkan-compatible device: ") + properties.deviceName);
                ++index;
                continue;
            }

            if (!m_systemConfig.HasParseError())
            {
                if (m_systemConfig.HasMember("graphics_card"))
                {
                    assert(m_systemConfig["graphics_card"].IsString());
                    std::string chosenCard(m_systemConfig["graphics_card"].GetString());
                    if (chosenCard == properties.deviceName)
                    {
                        rates[index] = 100;
                        break;
                    }
                }
            }

            if (properties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                rates[index] = 80;
            }
            else if (properties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)
            {
                rates[index] = 50;
            }
            else if (properties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
            {
                rates[index] = 30;
            }
        }

        unsigned maxVal = 0, maxIndex = ~0U;
        for (index = 0; index < deviceCount; ++index)
        {
            if (rates[index] > maxVal)
            {
                maxIndex = index;
                maxVal = rates[index];
            }
        }

        if (maxIndex == ~0U)
        {
            return false;
        }
        m_vkPhysicalDevice = devices[maxIndex];
        return true;
    }

    void Base::onQueueCreateInfoReceived(const std::vector<VkDeviceQueueCreateInfo>& queueInfos)
    {
        VkPhysicalDeviceFeatures availableFeatures, enabledFeatures;
        vkGetPhysicalDeviceFeatures(m_vkPhysicalDevice, &availableFeatures);
        if (!m_systemConfig.HasParseError())
        {
            if (m_systemConfig.HasMember("features") && m_systemConfig["features"].IsArray())
            {
                std::set<std::string> queriedFeatures{};
                for (const auto& queriedFeature : m_systemConfig["features"].GetArray())
                {
                    queriedFeatures.insert(queriedFeature.GetString());
                }
                enabledFeatures = SupportedFeatureHolder::strToFeatures(queriedFeatures, availableFeatures);
            }
        }
        VkDeviceCreateInfo deviceInfo
        {
            VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            nullptr,
            {},
            static_cast<uint32_t>(queueInfos.size()), queueInfos.data(),
            0, nullptr,
            0, nullptr,
            &enabledFeatures
        };
        vkCreateDevice(m_vkPhysicalDevice, &deviceInfo, nullptr, &m_vkDevice);
    }
}