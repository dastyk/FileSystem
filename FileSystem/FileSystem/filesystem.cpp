#include "filesystem.h"

int FileSystem::Init()
{
	switch (mMemblockDevice.Init())
	{
	case 0:
	{
		// Create root directory
		Directory root("/", -1);

		int dir = root.Save(&mMemblockDevice);
		if (dir == -1)
			return -1;

		mCurrDir = dir;
		return 0;
	}
	case 1:
	{
		return 1;
	}
	}
	return -1;
}

std::string FileSystem::GetCurrDirPath()
{

	Directory dir;
	int err = dir.Read(&mMemblockDevice, mCurrDir);
	if (err == -1)
		return "/";

	return dir.GetPath(&mMemblockDevice);
}

std::string FileSystem::cd(std::string & name)
{
	Directory dir;
	int err = dir.Read(&mMemblockDevice, mCurrDir);
	if (err == -1)
		return "No root";

	if (name == "..")
	{
		int temp;
		temp = dir.GetParent();
		if (temp != -1)
			mCurrDir = temp;
	}

	else
	{
		err = dir.GetChildDir(&mMemblockDevice, name);
		if (err == -1)
			return "Directory not found.";

		mCurrDir = err;
	}
	return "";
}

std::string FileSystem::cd(std::string names[], int count)
{
	return std::string();
}

FileSystem::FileSystem() {
	mCurrDir = 0;

}
FileSystem::~FileSystem() {

}

std::string FileSystem::format()
{
	mMemblockDevice.Format();
	// Create root directory
	Directory root;

	int dir = root.Save(&mMemblockDevice);
	if (dir == -1)
		return "Format failed";

	mCurrDir = dir;
	return "Device has been formated.\n";
}

std::string FileSystem::ls()
{
	Directory dir;
	dir.Read(&mMemblockDevice, mCurrDir);
	return dir.PrintContent(&mMemblockDevice);
}

std::string FileSystem::create(const std::string & name)
{
	Directory root;

	int r = root.Read(&mMemblockDevice, mCurrDir);
	if (r == -1)
		return "Could not find root.";

	r = root.GetChildFile(&mMemblockDevice, name);
	if (r != -1)
		return "File already exist.\n\n";

	File file(name, mCurrDir);
	r = file.Save(&mMemblockDevice);
	if (r == -1)
		return "No memory\n";

	r = root.AddFile(&mMemblockDevice, r);
	if (r == -1)
		return "No memory\n";

	return "";
}

std::string FileSystem::create(const std::string & name, const std::string & data)
{
	Directory root;

	int r = root.Read(&mMemblockDevice, mCurrDir);
	if (r == -1)
		return "Could not find root.";

	r = root.GetChildFile(&mMemblockDevice, name);
	if (r != -1)
		return "File already exist.\n\n";

	File file(name, mCurrDir, data);
	r = file.Save(&mMemblockDevice);
	if (r == -1)
		return "No memory\n";

	r = root.AddFile(&mMemblockDevice, r);
	if (r == -1)
		return "No memory\n";

	return "";
}

std::string FileSystem::cat(std::string & name)
{
	Directory root;

	int r = root.Read(&mMemblockDevice, mCurrDir);
	if (r == -1)
		return "Could not find root.";


	r = root.GetChildFile(&mMemblockDevice, name);
	if (r != -1)
	{
		File file;
		r = file.Read(&mMemblockDevice, r);
		file.DumpData(&mMemblockDevice);
		return "";
		
	}
	return "Fail";
}

std::string FileSystem::rm(const std::string & filePath)
{
	Directory root;
	int r = root.Read(&mMemblockDevice, mCurrDir);
	if (r == -1)
		return "Could not find root.";

	r = root.DeleteDir(&mMemblockDevice, filePath);
	if (r == 1)
		return "";

	r = root.DeleteFile(&mMemblockDevice, filePath);
	if (r == 1)
		return "";

	return "No such file or directory.";
}

std::string FileSystem::copy(const std::string & source, const std::string & dest)
{
	Directory root;
	int r = root.Read(&mMemblockDevice, mCurrDir);
	if (r == -1)
		return "Could not find root.";

	r = root.GetChildFile(&mMemblockDevice, source);
	if (r == -1)
		return "Source file not found.";

	File files;
	r = files.Read(&mMemblockDevice, r);
	if (r == -1)
		return "Feeeel";

	return create(dest, files.ToString());
}

std::string FileSystem::append(const std::string & source, const std::string & app)
{
	Directory root;
	int r = root.Read(&mMemblockDevice, mCurrDir);
	if (r == -1)
		return "Could not find root.";

	r = root.GetChildFile(&mMemblockDevice, source);
	if (r == -1)
		return "Source file not found.";

	File files;
	r = files.Read(&mMemblockDevice, r);
	if (r == -1)
		return "Feeeel";

	r = root.GetChildFile(&mMemblockDevice, app);
	if (r == -1)
		return "Append file not found.";

	File filea;
	r = filea.Read(&mMemblockDevice, r);
	if (r == -1)
		return "Feeeel";

	r = files.AddData(&mMemblockDevice, filea.ToString());
	if (r == -1)
		return "Feeeel";

	return "";
}

std::string FileSystem::rename(const std::string & source, const std::string & newName)
{
	Directory root;

	int r = root.Read(&mMemblockDevice, mCurrDir);
	if (r == -1)
		return "Could not find root.";


	r = root.GetChildFile(&mMemblockDevice, source);
	if (r != -1)
	{
		File file;
		file.Read(&mMemblockDevice, r);
		file.SetName(&mMemblockDevice, newName);
		return "";
	}

	r = root.GetChildDir(&mMemblockDevice, source);
	if (r != -1)
	{
		Directory dir;
		dir.Read(&mMemblockDevice, r);
		dir.SetName(&mMemblockDevice, newName);
		return "";
	}

	return "No such file or directory.";
}

std::string FileSystem::mkdir(const std::string & filePath)
{
	Directory root;

	int r = root.Read(&mMemblockDevice, mCurrDir);
	if (r == -1)
		return "Could not find root.";

	r = root.GetChildDir(&mMemblockDevice, filePath);
	if (r != -1)
		return "Directory already exist.\n\n";

	Directory dire(filePath, mCurrDir);

	r = dire.Save(&mMemblockDevice);
	if (r == -1)
		return "No memory\n";

	r = root.AddDir(&mMemblockDevice, r);
	if (r == -1)
		return "No memory\n";

	return "\n";
}
