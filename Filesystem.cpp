#include "Filesystem.h"
#include "Logger.h"
#include <fstream>

namespace sr
{
    Filesystem::Filesystem()
    {
        std::error_code ec;
        m_executivePath = std::filesystem::current_path(ec);
        if (ec)
        {
            Logger::log<LogLevel::Critical>("Failed to obtain current path because of error: " + ec.message());
            return;
        }
        m_systemConfigPath = m_executivePath / "sysconf";
        if (!std::filesystem::exists(m_systemConfigPath))
        {
            m_systemConfigPath = m_executivePath;
        }
    }

    const std::filesystem::path& Filesystem::getExecutivePath() const
    {
        return m_executivePath;
    }

    const std::filesystem::path& Filesystem::getSystemConfigPath() const
    {
        return m_systemConfigPath;
    }

    std::string Filesystem::getFileContents(const std::filesystem::path& path) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!std::filesystem::exists(path))
        {
            return "";
        }
        std::ifstream ifs(path);
        std::string contents;
        ifs >> contents;
        ifs.close();
        return contents;
    }
}
