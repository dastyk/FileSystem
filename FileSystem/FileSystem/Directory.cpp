#include "Directory.h"
#include "File.h"


Directory::Directory()
{
	mName = "/";
	mParent = -1;
	mBnum = -1;
}

Directory::Directory(string name) : mName(name)
{
	mBnum = -1;
	mParent = -1;
}

Directory::Directory(string name, int parent) : mName(name), mParent(parent)
{
	mBnum = -1;
}

Directory::Directory(const Directory * other)
{
	this->mBnum = other->mBnum;
	this->mName = other->mName;
	this->mParent = other->mParent;
	this->mChildDir = other->mChildDir;
	this->mChildFile = other->mChildFile;
}


Directory::~Directory()
{
}

int Directory::Save(MemBlockDevice* dev)
{
	if (mBnum == -1)
	{
		mBnum = dev->GetNextFreeBlock();
		if (mBnum == -1)
			return -1;		
	}
	int err;
	err = dev->writeBlock(mBnum, ToString());
	if (err == -1)
		return -1;
	return mBnum;
}

int Directory::Read(MemBlockDevice * dev, int path)
{
	mBnum = path;
	Block b = dev->readBlock(mBnum);
	string data = b.toString();
	int temp = 0;
	int offset = 0;

	memcpy(&temp, data.c_str() + offset, sizeof(int));
	offset += sizeof(int);

	char buff[blocksize + 1];
	memcpy(buff, data.c_str() + offset, temp);
	buff[temp] = '\0';
	offset += temp;
	mName = buff;

	memcpy(&mParent, data.c_str() + offset, sizeof(int));
	offset += sizeof(int);

	int t2;
	memcpy(&t2, data.c_str() + offset, sizeof(int));
	offset += sizeof(int);

	int t3;
	memcpy(&t3, data.c_str() + offset, sizeof(int));
	offset += sizeof(int);

	int t4;
	for (int i = 0; i < t2; i++)
	{
		memcpy(&t4, data.c_str() + offset, sizeof(int));
		offset += sizeof(int);
		mChildDir.push_back(t4);
	}
	for (int i = 0; i < t3; i++)
	{
		memcpy(&t4, data.c_str() + offset, sizeof(int));
		offset += sizeof(int);
		mChildFile.push_back(t4);
	}
	return 1;
}

string Directory::PrintContent(MemBlockDevice* dev)
{
	string out;
	Directory dir;
	for (int i = 0; i < mChildDir.size(); i++)
	{
		dir.Read(dev, mChildDir[i]);
		out += "/" + dir.GetName() + " " + to_string(blocksize) + "\n\n";
	}
	File file;
	for (int i = 0; i < mChildFile.size(); i++)
	{
		file.Read(dev, mChildFile[i]);
		out += file.GetName() + " " + to_string(file.GetSize()) + "\n\n";
	}
	return out;
}

string Directory::GetName()
{
	return mName;
}

int Directory::GetChildDir(MemBlockDevice * dev, string name)
{
	for (int i = 0; i < mChildDir.size(); i++)
	{
		Directory dir;
		int err = dir.Read(dev, mChildDir[i]);
		if (err == -1)
			return -1;

		if (name == dir.GetName())
		{
			return mChildDir[i];
		}
	}


	return -1;
}

int Directory::GetChildFile(MemBlockDevice * dev, string name)
{
	for (int i = 0; i < mChildFile.size(); i++)
	{
		File file;
		int err = file.Read(dev, mChildFile[i]);
		if (err == -1)
			return -1;

		if (name == file.GetName())
		{
			return mChildFile[i];
		}
	}


	return -1;
}

void Directory::DeleteDir(MemBlockDevice * dev)
{
	int count = mChildDir.size();

	for (int i = 0; i < count; i++)
	{
		Directory dir;
		dir.Read(dev, mChildDir[i]);
		dir.DeleteDir(dev);
	}
	mChildDir.clear();
	count = mChildFile.size();

	for (int i = 0; i < count; i++)
	{
		File file;
		file.Read(dev, mChildFile[i]);
		file.DeleteFile(dev);
	}
	mChildFile.clear();
	dev->FreeMemoryBlock(mBnum);

}

int Directory::DeleteDir(MemBlockDevice * dev, string name)
{
	int r = GetChildDir(dev, name);
	if (r != -1)
	{
		Directory dir;
		int err = dir.Read(dev, r);
		if (err == -1)
			return -1;
		dir.DeleteDir(dev);

		vector<int> temp;
		for (int i = 0; i < mChildDir.size(); i++)
		{
			if (!mChildDir[i] == r)
			{
				temp.push_back(mChildDir[i]);
			}
		}
		mChildDir.clear();
		mChildDir = temp;
		int r = Save(dev);
		return r;
	}

	return -1;
}

int Directory::DeleteFile(MemBlockDevice * dev, string name)
{
	
	int r = GetChildFile(dev, name);
	if (r != -1)
	{
		File file;
		int err = file.Read(dev, r);
		if (err == -1)
			return -1;
		file.DeleteFile(dev);
		vector<int> temp;
		for (int i = 0; i < mChildFile.size(); i++)
		{
			if (!mChildFile[i] == r)
			{
				temp.push_back(mChildFile[i]);
			}
		}
		mChildFile.clear();
		mChildFile = temp;
		int r = Save(dev);
		return 1;
	}

	return -1;
}

const int Directory::GetParent() const
{
	return mParent;
}

const int Directory::GetBlockNum() const
{
	return mBnum;
}

int Directory::AddDir(MemBlockDevice* dev, int blockNr)
{
	mChildDir.push_back(blockNr);
	int r = Save(dev);
	return r;
}

int Directory::AddFile(MemBlockDevice * dev, int blockNr)
{
	mChildFile.push_back(blockNr);
	int r = Save(dev);
	return r;
}

void Directory::SetName(MemBlockDevice * dev, string name)
{
	mName = name;
	Save(dev);
}

const string Directory::ToString()const
{
	char out[512];
	int point = 0;

	string s;
	int temp = mName.size();
	memcpy(out + point, &temp, sizeof(int));
	point += sizeof(int);

	memcpy(out + point, mName.c_str(), temp);
	point += temp;

	memcpy(out + point, &mParent, sizeof(int));
	point += sizeof(int);

	int count = mChildDir.size();
	memcpy(out + point, &count, sizeof(int));
	point += sizeof(int);

	int count2 = mChildFile.size();
	memcpy(out + point, &count2, sizeof(int));
	point += sizeof(int);

	for (int i = 0; i < count; i++)
	{
		memcpy(out + point, &mChildDir[i], sizeof(int));
		point += sizeof(int);
	}
	for (int i = 0; i < count2; i++)
	{
		memcpy(out + point, &mChildFile[i], sizeof(int));
		point += sizeof(int);
	}

	for (int i = 0; i < point; i++)
	{
		s += out[i];
	}
	return s;
}

const string Directory::GetPath(MemBlockDevice* dev) const
{
	if (mParent == -1)
	{
		return mName;
	}
	else
	{
		Directory dir;
		int err = dir.Read(dev, mParent);
		if (err == -1)
			return "/";
		string out  = dir.GetPath(dev) + "/" + mName;
		return out;
	}
}
