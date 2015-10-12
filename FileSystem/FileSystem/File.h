#ifndef _FILE_H_
#define _FILE_H_

#pragma once
#include "Directory.h"
class File :
	public Directory
{
public:
	File();
	File(string name);
	File(string name, int parent);


	virtual ~File();

	int Save(MemBlockDevice* dev);
	int Read(MemBlockDevice* dev, int blocknr);

	string PrintContent(MemBlockDevice* dev);

	int GetChildDir(MemBlockDevice* dev, string name);
	int GetChildFile(MemBlockDevice* dev, string name);

	void DeleteDir(MemBlockDevice* dev);
	void DeleteFile(MemBlockDevice* dev);

	int DeleteDir(MemBlockDevice* dev, string name);
	int DeleteFile(MemBlockDevice* dev, string name);

	int AddDir(MemBlockDevice* dev, int blockNr);
	int AddFile(MemBlockDevice* dev, int blockNr);

	void AddData(const string& data);

	const string ToString() const;

private:
	string mData;
	int mNumBlocks;
};

#endif