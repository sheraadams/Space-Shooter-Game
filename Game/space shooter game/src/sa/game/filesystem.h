#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <cstdlib>
#include "root_directory.h" 

class FileSystem
{
public:
#pragma warning(disable: 4996)
private:
    typedef std::string(*Builder) (const std::string& path);

public:
    static std::string getPath(const std::string& path)
    {
        static std::string(*pathBuilder)(std::string const&) = getPathBuilder();
        return (*pathBuilder)(path);
    }

private:
    static std::string const& getRoot()
    {
        static std::string root = (getProjectDirectory());
        return root;
    }

    static std::string getProjectDirectory()
    {
        // You need to replace this with your code to dynamically retrieve the project directory
        // For example, you might use platform-specific code or a build system variable.
        // This is a simplified example, and you may need to adjust it based on your project structure.
        return "./"; // Update this with your actual code to get the project directory
    }

    static Builder getPathBuilder()
    {
        if (getRoot() != "")
            return &FileSystem::getPathRelativeRoot;
        else
            return &FileSystem::getPathRelativeBinary;
    }

    static std::string getPathRelativeRoot(const std::string& path)
    {
        return getRoot() + std::string("/") + path;
    }

    static std::string getPathRelativeBinary(const std::string& path)
    {
        return getProjectDirectory() + path;
    }
};

#endif // FILESYSTEM_H
