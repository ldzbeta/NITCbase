#include "StaticBuffer.h"

// the declarations for this class can be found at "StaticBuffer.h"

unsigned char StaticBuffer::blocks[BUFFER_CAPACITY][BLOCK_SIZE];
struct BufferMetaInfo StaticBuffer::metainfo[BUFFER_CAPACITY];

StaticBuffer::StaticBuffer()
{

  // initialise all blocks as free
  for (int bufferIndex = 0; bufferIndex < BUFFER_CAPACITY; bufferIndex++)
  {
    metainfo[bufferIndex].free = true;
    metainfo[bufferIndex].dirty=false;
    metainfo[bufferIndex].timeStamp=-1;
    metainfo[bufferIndex].blockNum=-1;
  }
}

/*
At this stage, we are not writing back from the buffer to the disk since we are
not modifying the buffer. So, we will define an empty destructor for now. In
subsequent stages, we will implement the write-back functionality here.
*/
StaticBuffer::~StaticBuffer() {
  for (int i = 0; i < BUFFER_CAPACITY; i++)
  {
    if(!metainfo[i].free && metainfo[i].dirty)
      Disk::writeBlock(blocks[i],metainfo[i].blockNum);
  }
  
}

int StaticBuffer::getFreeBuffer(int blockNum)
{
  if (blockNum < 0 || blockNum > DISK_BLOCKS)
  {
    return E_OUTOFBOUND;
  }

  for(int i = 0; i < BUFFER_CAPACITY; i++) {
        if(metainfo[i].free == false) metainfo[i].timeStamp++ ;
    }
  int allocatedBuffer=-1;

  // iterate through all the blocks in the StaticBuffer
  // find the first free block in the buffer (check metainfo)
  // assign allocatedBuffer = index of the free block
  int maxTimeStamp=0,maxTimeIdx=-1;
  for (int bufferIndex = 0; bufferIndex < BUFFER_CAPACITY; bufferIndex++)
  {
    if(metainfo[bufferIndex].timeStamp>maxTimeStamp){
      maxTimeIdx=bufferIndex;
      maxTimeStamp= metainfo[bufferIndex].timeStamp;
    }

    if(metainfo[bufferIndex].free){
      allocatedBuffer=bufferIndex;
      break;
    }
  }

  if(allocatedBuffer==-1 && metainfo[maxTimeIdx].dirty){
    Disk::writeBlock(blocks[maxTimeIdx],metainfo[maxTimeIdx].blockNum);
    allocatedBuffer=maxTimeIdx;
  }

  metainfo[allocatedBuffer].free = false;
  metainfo[allocatedBuffer].blockNum = blockNum;
  metainfo[allocatedBuffer].dirty = false ;
  metainfo[allocatedBuffer].timeStamp = 0 ;

  return allocatedBuffer;
}

/* Get the buffer index where a particular block is stored
   or E_BLOCKNOTINBUFFER otherwise
*/
int StaticBuffer::getBufferNum(int blockNum)
{
  // Check if blockNum is valid (between zero and DISK_BLOCKS)
  // and return E_OUTOFBOUND if not valid.
  if(blockNum<0 || blockNum>DISK_BLOCKS)
    return E_OUTOFBOUND;

  // find and return the bufferIndex which corresponds to blockNum (check metainfo)
    for (int bufferIndex=0;bufferIndex<BUFFER_CAPACITY;bufferIndex++) {
      if(metainfo[bufferIndex].blockNum==blockNum && metainfo[bufferIndex].free==false)
        return bufferIndex;
    }

  // if block is not in the buffer
  return E_BLOCKNOTINBUFFER;
}


int StaticBuffer::setDirtyBit(int blockNum){
    // find the buffer index corresponding to the block using getBufferNum().
  int bufferIdx=getBufferNum(blockNum);
    // if block is not present in the buffer (bufferNum = E_BLOCKNOTINBUFFER)
    //     return E_BLOCKNOTINBUFFER
    if(bufferIdx==E_BLOCKNOTINBUFFER)
      return E_BLOCKNOTINBUFFER;

    // if blockNum is out of bound (bufferNum = E_OUTOFBOUND)
    //     return E_OUTOFBOUND
    if(bufferIdx== E_OUTOFBOUND)
      return E_OUTOFBOUND;
    else
      metainfo[bufferIdx].dirty=true;

    // else
    //     (the bufferNum is valid)
    //     set the dirty bit of that buffer to true in metainfo

    // return SUCCESS
    return SUCCESS; 
}
