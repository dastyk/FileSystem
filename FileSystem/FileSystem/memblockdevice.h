#ifndef MEMBLOCKDEVICE_H
#define MEMBLOCKDEVICE_H
#include "blockdevice.h"
#include <fstream>

struct FileHeader
{
	int parent;
	int namesize;	
	int size;
	int nrofblocks;
	std::string name;

	int* blockPointers;
	FileHeader()
	{
		nrofblocks = 0;
		blockPointers = 0;
	}
	~FileHeader()
	{
		if (blockPointers)
		{
			delete[] blockPointers;
		}
	}
};
class MemBlockDevice: public BlockDevice
{
private:
	int* mFreeBlocks;
	int* mFilledBlocks;


	bool file_exist(const std::string& fileName);
	
public:
	const int GetNextFreeBlock()const;
	const int SaveFile(const std::string & data, FileHeader& file, int blocknr);
	const int LoadFile(std::string & data, FileHeader& file, int blocknr);
	const int LoadFileHead(FileHeader& file, int blocknr);

	const void FreeMemoryBlock(int blocknr)const;
	const int Init();
	const int Format();

public:
    MemBlockDevice(int nrOfBlocks = 250);
    MemBlockDevice(const MemBlockDevice &other);

    ~MemBlockDevice();

    /* Operators */
    MemBlockDevice& operator=(const MemBlockDevice &other);
    Block &operator[] (int index) const;

    /* Returns amount of free blocks */
    int spaceLeft() const;

    /* Writes a block */
    int writeBlock(int blockNr, const std::vector<char> &vec);
    int writeBlock(int blockNr, const std::string &strBlock);
    int writeBlock(int blockNr, const char cArr[], int bytewidth, int offset);     // Use with care, make sure that cArr has at least the same dimension as block

    /* Reads a block */
    Block readBlock(int blockNr) const;

    /* Resets all the blocks */
    void reset();

    /* Return the size */
    int size() const;





};

#endif // MEMBLOCKDEVICE_H
