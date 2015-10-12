#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "memblockdevice.h"
#include "Directory.h"
#include "File.h"
class FileSystem
{
private:
    MemBlockDevice mMemblockDevice;
    // Here you can add your own data structures
	int mCurrDir;
public:
	int Init();
	std::string GetCurrDirPath();
	std::string cd(std::string& name);
	std::string cd(std::string names[], int count);
	std::string mkdir(const std::string &name);
public:
    FileSystem();
	~FileSystem();
    /* These commands needs to implemented */
    /*
     * However, feel free to change the signatures, these are just examples.
     * Remember to remove 'const' if needed.
     */
    std::string format();
    std::string ls();
    std::string ls(const std::string &path) const;  // optional
    std::string create(const std::string &name);
    std::string cat(std::string &name) const;
    std::string save(const std::string &saveFile) const;
    std::string read(const std::string &saveFile) const;
    std::string rm(const std::string &filePath);
    std::string copy(const std::string &source, const std::string &dest);

    /* Optional */
    std::string append(const std::string &source, const std::string &app);
    std::string rename(const std::string &source, const std::string &newName);
    std::string chmod(int permission, const std::string &file);

    /* Add your own member-functions if needed */
};

#endif // FILESYSTEM_H
