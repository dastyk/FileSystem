#include "File.h"



File::File()
{
	mNumBlocks = 0;
}

File::File(string name) : Directory(name)
{
	mNumBlocks = 0;
}

File::File(string name, int parent) : Directory(name,parent)
{
	char data[blocksize*sizeof(int)];

	for (int i = 0; i < 512; i++)
	{
		memcpy(data + i*sizeof(int), &i, sizeof(int));
	}
	for (int i = 0; i < blocksize*sizeof(int); i++)
	{
		mData += data[i];
	}
	mNumBlocks = 0;
}

File::File(string name, int parent, const string & data) : Directory(name, parent)
{
	mData = data;
	mNumBlocks = 0;
}


File::~File()
{
}

int File::Save(MemBlockDevice * dev)
{
	if (mBnum == -1)
	{
		mBnum = dev->GetNextFreeBlock();
		if (mBnum == -1)
			return -1;
	}
	FileHeader head;

	head.name = mName;
	head.namesize = mName.size();
	head.nrofblocks = mNumBlocks;
	head.parent = mParent;
	if(mNumBlocks > 0)
		head.blockPointers = &mChildFile[0];
	int err = dev->SaveFile(mData, head, mBnum);
	if (err == -1)
		return -1;

	mNumBlocks = head.nrofblocks;
	mChildFile.clear();
	for (int i = 0; i < mNumBlocks; i++)
		mChildFile.push_back(head.blockPointers[i]);
	return mBnum;
}

int File::Read(MemBlockDevice * dev, int blocknr)
{
	mBnum = blocknr;
	FileHeader head;
	int err = dev->LoadFile(mData, head, blocknr);
	if (err == -1)
		return -1;

	mParent = head.parent;
	mName = head.name;
	mNumBlocks = head.nrofblocks;
	for (int i = 0; i < mNumBlocks; i++)
		mChildFile.push_back(head.blockPointers[i]);
	return 1;
}

string File::PrintContent(MemBlockDevice * dev)
{
	return mData;
}

int File::GetChildDir(MemBlockDevice * dev, string name)
{
	return -1;
}

int File::GetChildFile(MemBlockDevice * dev, string name)
{
	return -1;
}

void File::DeleteDir(MemBlockDevice * dev)
{
}

void File::DeleteFile(MemBlockDevice * dev)
{
	FileHeader head;
	dev->LoadFileHead(head, mBnum);
	for (int i = 0; i < head.nrofblocks; i++)
	{
		dev->FreeMemoryBlock(head.blockPointers[i]);
	}
	dev->FreeMemoryBlock(mBnum);
}

int File::DeleteDir(MemBlockDevice * dev, string name)
{
	return 0;
}

int File::DeleteFile(MemBlockDevice * dev, string name)
{
	return 0;
}

int File::DumpData(MemBlockDevice * dev)
{
	mData.clear();
	int err = Save(dev);
	if (err == -1)
		return -1;
	mChildFile.clear();
	return 1;
}

int File::AddDir(MemBlockDevice * dev, int blockNr)
{
	return -1;
}

int File::AddFile(MemBlockDevice * dev, int blockNr)
{
	return -1;
}

int File::AddData(MemBlockDevice * dev, const string& data)
{
	mData += data;
	return Save(dev);
}

const string File::ToString() const
{
	return mData;
}
