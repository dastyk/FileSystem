#ifndef _DIRECTORY_H_
#define _DIRECTORY_H_

#pragma once
#include <string>
#include <vector>
#include "memblockdevice.h"

using namespace std;
class Directory
{
public:
	Directory();
	Directory(string name);
	Directory(string name, int parent);
	Directory(const Directory* other);

	virtual ~Directory();

	virtual int Save(MemBlockDevice* dev);
	virtual int Read(MemBlockDevice* dev, int blocknr);

	virtual string PrintContent(MemBlockDevice* dev);
	string GetName();

	virtual int GetChildDir(MemBlockDevice* dev, string name);
	virtual int GetChildFile(MemBlockDevice* dev, string name);

	virtual void DeleteDir(MemBlockDevice* dev);

	virtual int DeleteDir(MemBlockDevice* dev, string name);
	virtual int DeleteFile(MemBlockDevice* dev, string name);

	const int GetParent()const;
	const int GetBlockNum()const;

	virtual int AddDir(MemBlockDevice* dev, int blockNr);
	virtual int AddFile(MemBlockDevice* dev, int blockNr);

	void SetName(MemBlockDevice * dev, string name);
	virtual const string ToString() const;

	const string GetPath(MemBlockDevice* dev)const;
protected:
	string mName;
	int mParent;
	vector<int> mChildDir;
	vector<int> mChildFile;

	int mBnum;


};

#endif