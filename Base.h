#ifndef SIMPLERENDER_BASE_H
#define SIMPLERENDER_BASE_H

#include "Signal.hpp"
#include "Logger.h"
#include "Filesystem.h"
#include <set>
#include <memory>
#include <thread>
#include <mutex>
#include <cassert>
#include <vulkan/vulkan.h>
#include <rapidjson/document.h>

namespace sr
{
    class Base: public std::enable_shared_from_this<Base>
    {
    public:
        Base(std::shared_ptr<Filesystem> filesystem);
        Signal<const std::vector<VkQueueFamilyProperties>> fireQueuePropertiesObtained;
        void onQueueCreateInfoReceived(const std::vector<VkDeviceQueueCreateInfo>& queueInfos);
    private:
        void createVkInstance();
        bool pickPhysicalDevice();

        mutable std::mutex m_mutex;
        VkInstance m_vkInstance;
        VkPhysicalDevice m_vkPhysicalDevice;
        VkDevice m_vkDevice;
        uint32_t m_vulkanVersion;
        std::shared_ptr<Filesystem> m_filesystem;
        rapidjson::Document m_systemConfig;

        class SupportedFeatureHolder
        {
            static const std::set<std::string> supportedFeaturesStr;
        public:
            static VkPhysicalDeviceFeatures strToFeatures(const std::set<std::string>& strFeatures, const VkPhysicalDeviceFeatures& availableFeatures);
            static const std::set<std::string>& getSupportedFeatures();
        };
    };
}

#endif