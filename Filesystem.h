#ifndef SIMPLERENDER_FILESYSTEM_H
#define SIMPLERENDER_FILESYSTEM_H

#include <filesystem>
#include <string>
#include <mutex>

namespace sr
{
    class Filesystem
    {
    public:
        Filesystem();
        const std::filesystem::path& getExecutivePath() const;
        const std::filesystem::path& getSystemConfigPath() const;
        std::string getFileContents(const std::filesystem::path& path) const;
    private:
        std::filesystem::path m_executivePath;
        std::filesystem::path m_systemConfigPath;
        mutable std::mutex m_mutex;
    };
}

#endif //SIMPLERENDER_FILESYSTEM_H
