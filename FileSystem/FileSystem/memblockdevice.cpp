#include "memblockdevice.h"
#include <stdexcept>

bool MemBlockDevice::file_exist(const std::string& fileName)
{
	std::ifstream infile(fileName);
	bool out = infile.good();
	return out;
}

const int MemBlockDevice::GetNextFreeBlock()const
{
	for (int i = 0; i < this->nrOfBlocks; i++)
	{
		if (mFreeBlocks[i] == 1)
		{
			mFreeBlocks[i] = 0;
			mFilledBlocks[i] = 1;

			return i;
		}
	}
	return -1;
}

const int MemBlockDevice::SaveFile(const std::string & data, FileHeader& file, int blocknr)
{
	for (int i = 0; i < file.nrofblocks; i++)
	{
		FreeMemoryBlock(file.blockPointers[i]);
	}
	if (file.nrofblocks > 0)
		file.blockPointers = 0;

	file.size = data.size();
	file.nrofblocks = (file.size / blocksize) + ((file.size % blocksize) ? 1 : 0);
	if(file.nrofblocks > 0)
		file.blockPointers = new int[file.nrofblocks];



	for (int i = 0; i < file.nrofblocks; i++)
	{
		file.blockPointers[i] = GetNextFreeBlock();
		if (file.blockPointers[i] == -1)
		{
			return -1;
		}
	}
	int err = writeBlock(blocknr, (char*)&file, sizeof(int)*4, 0 );
	if (err != 1)
	{
		return -1;
	}
	
	err = writeBlock(blocknr, file.name.c_str(), file.namesize, sizeof(int) * 4);
	if (err != 1)
	{
		return -1;
	}
	err = writeBlock(blocknr, (char*)file.blockPointers, sizeof(int)*file.nrofblocks, sizeof(int) * 4 + file.namesize);
	if (err != 1)
	{
		return -1;
	}
	int size = file.size;
	for (int i = 0; i < file.nrofblocks; i++)
	{
		int s = (size < blocksize) ? size%blocksize : blocksize;
		int err = writeBlock(file.blockPointers[i], (data.c_str() + i*blocksize), s, 0);
		if (err != 1)
		{
			return -1;
		}
		size -= blocksize;
	}

	return 1;
}

const int MemBlockDevice::LoadFile(std::string & data, FileHeader & file, int blocknr)
{
	Block b = readBlock(blocknr);
	std::string dat = b.toString();

	int offset = 0;

	memcpy(&file.parent, dat.c_str() + offset, sizeof(int));
	offset += sizeof(int);

	memcpy(&file.namesize, dat.c_str() + offset, sizeof(int));
	offset += sizeof(int);

	memcpy(&file.size, dat.c_str() + offset, sizeof(int));
	offset += sizeof(int);

	memcpy(&file.nrofblocks, dat.c_str() + offset, sizeof(int));
	offset += sizeof(int);

	char buff[blocksize + 1];
	memcpy(buff, dat.c_str() + offset, file.namesize);
	buff[file.namesize] = '\0';
	offset += file.namesize;
	file.name = buff;


	if (file.nrofblocks > 0)
		file.blockPointers = new int[file.nrofblocks];

	int temp;

	memcpy(file.blockPointers, dat.c_str() + offset, sizeof(int)*file.nrofblocks);
	offset += sizeof(int)*file.nrofblocks;

	char arr[blocksize];

	int size = file.size;
	for (int i = 0; i < file.nrofblocks; i++)
	{
		Block b = readBlock(file.blockPointers[i]);
		dat = b.toString();

		int s = (size < blocksize) ? size%blocksize : blocksize;
		memcpy(arr, dat.c_str(), s);
		size -= blocksize;
		for (int i = 0; i < s; i++)
		{
			data += arr[i];
		}
	}
	return 0;
}

const int MemBlockDevice::LoadFileHead(FileHeader & file, int blocknr)
{
	Block b = readBlock(blocknr);
	std::string dat = b.toString();

	int offset = 0;

	memcpy(&file.parent, dat.c_str() + offset, sizeof(int));
	offset += sizeof(int);

	memcpy(&file.namesize, dat.c_str() + offset, sizeof(int));
	offset += sizeof(int);

	memcpy(&file.size, dat.c_str() + offset, sizeof(int));
	offset += sizeof(int);

	memcpy(&file.nrofblocks, dat.c_str() + offset, sizeof(int));
	offset += sizeof(int);

	char buff[blocksize + 1];
	memcpy(buff, dat.c_str() + offset, file.namesize);
	buff[file.namesize] = '\0';
	offset += file.namesize;
	file.name = buff;


	if (file.nrofblocks > 0)
		file.blockPointers = new int[file.nrofblocks];

	int temp;

	memcpy(file.blockPointers, dat.c_str() + offset, sizeof(int)*file.nrofblocks);
	offset += sizeof(int)*file.nrofblocks;
	return 0;
}

const int MemBlockDevice::SaveToFile(const std::string & path)
{
	char arr[blocksize];

	//memBlocks[0].writeBlock((char*)&mFillSize, sizeof(int), 0);

	int size = nrOfBlocks*sizeof(int);
	int point = 0;
	for (int i = 0; i < mFillSize; i++)
	{
		int s = (size < blocksize) ? size : blocksize;
		memBlocks[1 + i * 2].writeBlock((char*)mFreeBlocks + point, s, 0);
		memBlocks[2 + i * 2].writeBlock((char*)mFilledBlocks + point, s, 0);
		point += s / sizeof(int);
		size -= blocksize;
	}

	std::ofstream out;//("disk", std::ios::binary | std::ios::out);
	out.open(path, std::ios::binary);
	if (out.bad())
		return -1;
	for (int i = 0; i < nrOfBlocks; i++)
	{
		std::string in = memBlocks[i].toString();
		out.write(in.c_str(), blocksize);
	}

	out.close();

	return 1;
}

const int MemBlockDevice::LoadFromFile(const std::string & path)
{
	FILE* filePtr;
	if (!file_exist(path))
		return -1;

	Format();

	fopen_s(&filePtr, path.c_str(), "rb");

	
	char arr[blocksize];
	for (int i = 0; i < nrOfBlocks; i++)
	{
		fread(arr, blocksize, 1, filePtr);
		memBlocks[i].writeBlock(arr, blocksize, 0);
	}

	int size = nrOfBlocks*sizeof(int);
	int point = 0;
	for (int i = 0; i < mFillSize; i++)
	{
		int s = (size < blocksize) ? size : blocksize;
		memcpy(mFreeBlocks + point, memBlocks[1 + i * 2].toString().c_str(), s);
		memcpy(mFilledBlocks + point, memBlocks[2 + i * 2].toString().c_str(), s);
		point += s/sizeof(int);
		size -= blocksize;
	}

	return GetStart();
}

const void MemBlockDevice::FreeMemoryBlock(int blocknr) const
{
	mFreeBlocks[blocknr] = 1;
	mFilledBlocks[blocknr] = 0;
	return void();
}


const int MemBlockDevice::Init()
{
	if (file_exist("dev"))
	{
		// Load from file
		return 1;
	}
	else
	{
		return Format();
		

	}
	return -1;
}

const int MemBlockDevice::Format()
{
	// Create the free blockarray
	if (mFreeBlocks)
	{
		delete[] mFreeBlocks;
	}
	if (mFilledBlocks)
	{
		delete[] mFilledBlocks;
	}
	mFreeBlocks = new int[this->nrOfBlocks];
	mFilledBlocks = new int[this->nrOfBlocks];
	for (int i = 0; i < this->nrOfBlocks; i++)
	{
		mFreeBlocks[i] = 1;
		mFilledBlocks[i] = 0;
	}

	// Reserve the first block and blocks for the filled and free arrays
	GetNextFreeBlock();

	mFillSize = ((nrOfBlocks*sizeof(int)) / blocksize) + (((nrOfBlocks*sizeof(int)) % blocksize) ? 1 : 0);
	for (int i = 0; i < mFillSize; i++)
	{
		GetNextFreeBlock();
		GetNextFreeBlock();
	}

	return 0;
}

const int MemBlockDevice::GetStart()
{
	return 1 + mFillSize*2;
}

MemBlockDevice::MemBlockDevice(int nrOfBlocks): BlockDevice(nrOfBlocks) {
	mFilledBlocks = 0;
	mFreeBlocks = 0;
}

MemBlockDevice::MemBlockDevice(const MemBlockDevice &other) : BlockDevice(other) {

}

MemBlockDevice::~MemBlockDevice() {
    /* Implicit call to base-class destructor */
	delete [] mFreeBlocks;
	delete [] mFilledBlocks;
}

MemBlockDevice& MemBlockDevice::operator=(const MemBlockDevice &other) {
    delete [] this->memBlocks;
    this->nrOfBlocks = other.nrOfBlocks;
    this->memBlocks = new Block[this->nrOfBlocks];

    for (int i = 0; i < this->nrOfBlocks; ++i)
        this->memBlocks[i] = other.memBlocks[i];

    return *this;
}

Block& MemBlockDevice::operator[](int index) const {
    if (index < 0 || index >= this->nrOfBlocks) {
        throw std::out_of_range("Illegal access\n");
    }
    else {
        return this->memBlocks[index];
    }
}

int MemBlockDevice::spaceLeft() const {
    /* Not yet implemented */
    return 0;
}

int MemBlockDevice::writeBlock(int blockNr, const std::vector<char> &vec) {
    int output = -1;    // Assume blockNr out-of-range

    if (blockNr < this->nrOfBlocks && blockNr >= 0) {
        /* -2 = vec and block dont have same dimensions */
        /* 1 = success */
		output = 1;
        output = this->memBlocks[blockNr].writeBlock(vec);
    }
    return output;
}

int MemBlockDevice::writeBlock(int blockNr, const std::string &strBlock) {
    int output = -1;    // Assume blockNr out-of-range

    if (blockNr < this->nrOfBlocks && blockNr >= 0) {
        /* -2 = str-length and block dont have same dimensions */
        /* 1 = success */
		output = 1;
        output = this->memBlocks[blockNr].writeBlock(strBlock);
    }
    return output;
}

int MemBlockDevice::writeBlock(int blockNr, const char cArr[], int bytewidth, int offset) {
    int output = -1;    // Assume blockNr out-of-range
    if (blockNr < this->nrOfBlocks && blockNr >= 0 && bytewidth + offset <= blocksize) {
        output = 1;
        // Underlying function writeBlock cannot check array-dimension.
        this->memBlocks[blockNr].writeBlock(cArr, bytewidth, offset);
    }
    return output;
}

Block MemBlockDevice::readBlock(int blockNr) const {
    if (blockNr < 0 || blockNr >= this->nrOfBlocks)
        throw std::out_of_range("Block out of range");
    else {
        Block a(this->memBlocks[blockNr]);
        return a;
    }
}

/* Resets all the blocks */
void MemBlockDevice::reset() {
    for (int i = 0; i < this->nrOfBlocks; ++i) {
        this->memBlocks[i].reset('0');
    }
}

int MemBlockDevice::size() const {
    return this->nrOfBlocks;
}
