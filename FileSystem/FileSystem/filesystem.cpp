#include "filesystem.h"

int FileSystem::Init()
{
	switch (mMemblockDevice.Init())
	{
	case 0:
	{
		// Create root directory
		mCurrDir = new Directory("/", -1);
		int dir = mCurrDir->Save(&mMemblockDevice);
		if (dir == -1)
			return -1;


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
	return mCurrDir->GetPath(&mMemblockDevice);
}

std::string FileSystem::cd(std::string & name)
{
	int temp;
	if (name == "//" || name ==  "")
	{
		delete mCurrDir;
		mCurrDir = new Directory;
		int temp = mCurrDir->Read(&mMemblockDevice, mMemblockDevice.GetStart());
		if (temp == -1)
			return "Error.\n";
	}
	else if (name == "..")
	{
		
		temp = mCurrDir->GetParent();
		if (temp != -1)
		{
			delete mCurrDir;
			mCurrDir = new Directory;
			temp = mCurrDir->Read(&mMemblockDevice, temp);
			if (temp == -1)
				return "Error.\n";
		}
			
	}

	else
	{
		temp = mCurrDir->GetChildDir(&mMemblockDevice, name);
		if (temp == -1)
			return "Directory not found.\n";
		delete mCurrDir;
		mCurrDir = new Directory;
		temp = mCurrDir->Read(&mMemblockDevice, temp);
		if (temp == -1)
			return "Error.\n";
	}
	return "";
}

std::string FileSystem::cd(vector<string>& path)
{
	int size = path.size();
	Directory* dir = mCurrDir;
	if (path[size - 1] == "")
	{
		mCurrDir = new Directory;
		int temp = mCurrDir->Read(&mMemblockDevice, mMemblockDevice.GetStart());
		if (temp == -1)
			return "Error.\n";
		path.pop_back();
		size -= 1;
	}
	else
	{
		mCurrDir = new Directory(dir);
	}

	for (int i = 0; i < size; i++)
	{
		string cp = path[size - i - 1];
		cp = cd(cp);
		if (cp != "")
		{
			delete mCurrDir;
			mCurrDir = dir;
			return "Path not found.\n";
		}
	}
	delete dir;
	return "";
}

FileSystem::FileSystem() {
	mCurrDir = 0;

}
FileSystem::~FileSystem() {
	if (mCurrDir)
	{
		delete mCurrDir;
	}
}

std::string FileSystem::format()
{
	mMemblockDevice.Format();

	Directory* temp = new Directory("/", -1);

	int dir = temp->Save(&mMemblockDevice);
	if (dir == -1)
		return "Format failed";
	else
	{
		delete mCurrDir;
		mCurrDir = temp;
	}
	return "Device has been formated.\n";
}

std::string FileSystem::ls()
{
	return mCurrDir->PrintContent(&mMemblockDevice);
}

std::string FileSystem::ls(const vector<string>& path)
{
	int size = path.size();
	Directory* dir = mCurrDir;
	mCurrDir = new Directory(dir);
	for (int i = 0; i < size; i++)
	{
		string cp = path[size-i-1];
		cp = cd(cp);
		if (cp != "")
			return "Path not found.\n";
	}
	string out = ls();
	delete mCurrDir;
	mCurrDir = dir;
	return out;
}

std::string FileSystem::create(const std::string & name)
{
	int r;
	r = mCurrDir->GetChildFile(&mMemblockDevice, name);
	if (r != -1)
		return "File already exist.\n";

	string data;
	cout << "\nEnter file text:\n";
	getline(cin, data);

	File file(name, mCurrDir->GetBlockNum(), data);
	r = file.Save(&mMemblockDevice);
	if (r == -1)
		return "No memory\n";

	r = mCurrDir->AddFile(&mMemblockDevice, r);
	if (r == -1)
		return "No memory\n";

	return "";
}

std::string FileSystem::create(const std::string & name, const std::string & data)
{
	int r;
	r = mCurrDir->GetChildFile(&mMemblockDevice, name);
	if (r != -1)
		return "File already exist.\n";

	File file(name, mCurrDir->GetBlockNum(), data);
	r = file.Save(&mMemblockDevice);
	if (r == -1)
		return "No memory\n";

	r = mCurrDir->AddFile(&mMemblockDevice, r);
	if (r == -1)
		return "No memory\n";

	return "";
}

std::string FileSystem::cat(std::string & name)
{
	int r;
	r = mCurrDir->GetChildFile(&mMemblockDevice, name);
	if (r == -1)
		return "File not found.\n";

	File file;
	r = file.Read(&mMemblockDevice, r);
	if (r == -1)
		return "Fail\n";

	return file.ToString();
}

std::string FileSystem::save(const std::string & saveFile)
{
	int r = mCurrDir->Save(&mMemblockDevice);
	if (r == -1)
		"Save failed.\n";
	r = mMemblockDevice.SaveToFile(saveFile);
	if (r == -1)
		"Save failed.\n";

	return "Device state saved\n";
}

std::string FileSystem::read(const std::string & saveFile)
{
	int r = mMemblockDevice.LoadFromFile(saveFile);
	if (r == -1)
		return "File not found.\n";
	Directory* temp = new Directory;
	temp->Read(&mMemblockDevice, r);
	if (r == -1)
		return "Error";
	delete mCurrDir;
	mCurrDir = temp;

	system("cls");

	return "State loaded.\n";
}

std::string FileSystem::rm(const std::string & filePath)
{
	int r;
	r = mCurrDir->DeleteDir(&mMemblockDevice, filePath);
	if (r == 1)
		return "";

	r = mCurrDir->DeleteFile(&mMemblockDevice, filePath);
	if (r == 1)
		return "\n";

	return "No such file or directory.\n";
}

std::string FileSystem::copy(const std::string & source, const std::string & dest)
{
	int r = mCurrDir->GetChildFile(&mMemblockDevice, source);
	if (r == -1)
		return "Source file not found.\n";

	File files;
	r = files.Read(&mMemblockDevice, r);
	if (r == -1)
		return "Feeeel\n";

	return create(dest, files.ToString());
}

std::string FileSystem::append(const std::string & source, const std::string & app)
{
	int r;
	r = mCurrDir->GetChildFile(&mMemblockDevice, source);
	if (r == -1)
		return "Source file not found.\n";

	File files;
	r = files.Read(&mMemblockDevice, r);
	if (r == -1)
		return "Feeeel\n";

	r = mCurrDir->GetChildFile(&mMemblockDevice, app);
	if (r == -1)
		return "Append file not found.\n";

	File filea;
	r = filea.Read(&mMemblockDevice, r);
	if (r == -1)
		return "Feeeel\n";

	r = files.AddData(&mMemblockDevice, filea.ToString());
	if (r == -1)
		return "Feeeel\n";

	return "";
}

std::string FileSystem::rename(const std::string & source, const std::string & newName)
{
	int r = mCurrDir->GetChildFile(&mMemblockDevice, source);
	if (r != -1)
	{
		File file;
		file.Read(&mMemblockDevice, r);
		file.SetName(&mMemblockDevice, newName);
		return "";
	}

	r = mCurrDir->GetChildDir(&mMemblockDevice, source);
	if (r != -1)
	{
		Directory dir;
		dir.Read(&mMemblockDevice, r);
		dir.SetName(&mMemblockDevice, newName);
		return "";
	}

	return "No such file or directory.\n";
}

std::string FileSystem::mkdir(const std::string & filePath)
{
	int r = mCurrDir->GetChildDir(&mMemblockDevice, filePath);
	if (r != -1)
		return "Directory already exist.\n";

	Directory dire(filePath, mCurrDir->GetBlockNum());

	r = dire.Save(&mMemblockDevice);
	if (r == -1)
		return "No memory\n";

	r = mCurrDir->AddDir(&mMemblockDevice, r);
	if (r == -1)
		return "No memory\n";

	return "\n";
}

std::string FileSystem::copy(vector<string>& source, vector<string>& dest)
{
	Directory* dir = mCurrDir;
	mCurrDir = new Directory(dir);
	if (source.size() != 1)
	{
		vector<string> path;
		for (int i = 1; i < source.size(); i++)
		{
			path.push_back(source[source.size() - i]);
		
		}
		if (cd(path) != "")
			return "Source file not found\n";
	}

	int r = mCurrDir->GetChildFile(&mMemblockDevice, source[0]);
	if (r == -1)
		return "Source file not found.\n";

	File files;
	r = files.Read(&mMemblockDevice, r);
	if (r == -1)
		return "Error.\n";

	delete mCurrDir;
	Directory* destf = mCurrDir = new Directory(dir);
	if (dest.size() != 1)
	{
		

		vector<string> path;
		for (int i = 1; i < dest.size(); i++)
		{
			path.push_back(dest[dest.size() - i]);

		}
		if (cd(path) != "")
			return "Destination path not found\n";
	}
	else
	{
		mCurrDir = dir;
	}

	string out = create(dest[0], files.ToString());

	delete destf;
	mCurrDir = dir;

	return out;
}
