#include "BlockBuffer.h"

#include <cstdlib>
#include <cstring>
// the declarations for these functions can be found in "BlockBuffer.h"

BlockBuffer::BlockBuffer(int blockNum)
{
  // initialise this.blockNum with the argument
  this->blockNum = blockNum;
}

// calls the parent class constructor
RecBuffer::RecBuffer(int blockNum) : BlockBuffer::BlockBuffer(blockNum)
{
}

// // load the block header into the argument pointer
// int BlockBuffer::getHeader(struct HeadInfo *head)
// {
//   unsigned char buffer[BLOCK_SIZE];

//   // read the block at this.blockNum into the buffer
//   Disk::readBlock(buffer, this->blockNum);

//   // populate the numEntries, numAttrs and numSlots fields in *head
//   memcpy(&head->numSlots, buffer + 24, 4);
//   memcpy(&head->numEntries, buffer + 16, 4);
//   memcpy(&head->numAttrs, buffer + 20, 4);
//   memcpy(&head->rblock, buffer + 12, 4);
//   memcpy(&head->lblock, buffer + 8, 4);

//   return SUCCESS;
// }

// // load the record at slotNum into the argument pointer
// int RecBuffer::getRecord(union Attribute *rec, int slotNum)
// {
//   struct HeadInfo head;

//   // get the header using this.getHeader() function
//   this->getHeader(&head);

//   int attrCount = head.numAttrs;
//   int slotCount = head.numSlots;

//   unsigned char buffer[BLOCK_SIZE];
//   // read the block at this.blockNum into a buffer
//   Disk::readBlock(buffer, this->blockNum);
//   /* record at slotNum will be at offset HEADER_SIZE + slotMapSize + (recordSize * slotNum)
//      - each record will have size attrCount * ATTR_SIZE
//      - slotMap will be of size slotCount
//   */
//   int recordSize = attrCount * ATTR_SIZE;
//   unsigned char *slotPointer = buffer + HEADER_SIZE + slotCount + recordSize * slotNum;

//   // load the record into the rec data structure
//   memcpy(rec, slotPointer, recordSize);

//   return SUCCESS;
// }

/*
Used to get the header of the block into the location pointed to by `head`
NOTE: this function expects the caller to allocate memory for `head`
*/
int BlockBuffer::getHeader(struct HeadInfo *head) {

  unsigned char *bufferPtr;
  int ret = loadBlockAndGetBufferPtr(&bufferPtr);
  if (ret != SUCCESS) {
    return ret;   // return any errors that might have occured in the process
  }

  // ... (the rest of the logic is as in stage 2)


  // Disk::readBlock(bufferPtr, this->blockNum);

  memcpy(&head->numSlots, bufferPtr + 24, 4);
  memcpy(&head->numEntries, bufferPtr + 16, 4);
  memcpy(&head->numAttrs, bufferPtr + 20, 4);
  memcpy(&head->rblock, bufferPtr + 12, 4);
  memcpy(&head->lblock, bufferPtr + 8, 4);

  return SUCCESS;

  // ... (the rest of the logic is as in stage 2)
}

/*
Used to get the record at slot `slotNum` into the array `rec`
NOTE: this function expects the caller to allocate memory for `rec`
*/
int RecBuffer::getRecord(union Attribute *rec, int slotNum) {
  // ...
  unsigned char *bufferPtr;
  int ret = loadBlockAndGetBufferPtr(&bufferPtr);
  if (ret != SUCCESS) {
    return ret;
  }
  // ... (the rest of the logic is as in stage 2

  struct HeadInfo head;

  this->getHeader(&head);

  int attrCount = head.numAttrs;
  int slotCount = head.numSlots;

  int recordSize = attrCount * ATTR_SIZE;
  unsigned char *slotPointer = bufferPtr + HEADER_SIZE + slotCount + recordSize * slotNum;

  memcpy(rec, slotPointer, recordSize);

  return SUCCESS;
}

int RecBuffer::setRecord(union Attribute *rec, int slotNum) {
    unsigned char *bufferPtr;
    /* get the starting address of the buffer containing the block
       using loadBlockAndGetBufferPtr(&bufferPtr). */
      int res=loadBlockAndGetBufferPtr(&bufferPtr);
    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
        // return the value returned by the call.
        if(res!=SUCCESS)return res;

    /* get the header of the block using the getHeader() function */
    HeadInfo header;
    getHeader(&header);
    // get number of attributes in the block.
    // get the number of slots in the block.
        int attrCount = header.numAttrs;
        int slotCount = header.numSlots;

    // if input slotNum is not in the permitted range return E_OUTOFBOUND.
      if (slotNum < 0 || slotNum >= header.numSlots) {
        return E_OUTOFBOUND ;
    }
    /* offset bufferPtr to point to the beginning of the record at required
       slot. the block contains the header, the slotmap, followed by all
       the records. so, for example,
       record at slot x will be at bufferPtr + HEADER_SIZE + (x*recordSize)
       copy the record from `rec` to buffer using memcpy
       (hint: a record will be of size ATTR_SIZE * numAttrs)
    */
       int recordSize = ATTR_SIZE * attrCount;
    unsigned char* recStart= bufferPtr+HEADER_SIZE+ slotCount+slotNum*recordSize;
    memcpy(recStart,rec,recordSize);
    // update dirty bit using setDirtyBit()
    StaticBuffer::setDirtyBit(this->blockNum);
    /* (the above function call should not fail since the block is already
       in buffer and the blockNum is valid. If the call does fail, there
       exists some other issue in the code) */

    // return SUCCESS
    return SUCCESS;
}

/*
Used to load a block to the buffer and get a pointer to it.
NOTE: this function expects the caller to allocate memory for the argument
*/
int BlockBuffer::loadBlockAndGetBufferPtr(unsigned char **buffPtr) {
  // check whether the block is already present in the buffer using StaticBuffer.getBufferNum()
  int bufferNum = StaticBuffer::getBufferNum(this->blockNum);

  if (bufferNum == E_BLOCKNOTINBUFFER) {
    bufferNum = StaticBuffer::getFreeBuffer(this->blockNum);

    if (bufferNum == E_OUTOFBOUND) { 
      return E_OUTOFBOUND;
    }

    Disk::readBlock(StaticBuffer::blocks[bufferNum], this->blockNum);
  }else{
    for(int i = 0; i < BUFFER_CAPACITY; i++) {
      StaticBuffer::metainfo[i].timeStamp++;
    }
    StaticBuffer::metainfo[bufferNum].timeStamp = 0;  
  }

  // store the pointer to this buffer (blocks[bufferNum]) in *buffPtr
  *buffPtr = StaticBuffer::blocks[bufferNum];

  return SUCCESS;
}

/* used to get the slotmap from a record block
NOTE: this function expects the caller to allocate memory for `*slotMap`
*/
int RecBuffer::getSlotMap(unsigned char *slotMap) {
  unsigned char *bufferPtr;

  // get the starting address of the buffer containing the block using loadBlockAndGetBufferPtr().
  int ret = loadBlockAndGetBufferPtr(&bufferPtr);
  if (ret != SUCCESS) {
    return ret;
  }

  struct HeadInfo head;
  // get the header of the block using getHeader() function
  getHeader(&head);

  int slotCount = head.numSlots;

  // get a pointer to the beginning of the slotmap in memory by offsetting HEADER_SIZE
  unsigned char *slotMapInBuffer = bufferPtr + HEADER_SIZE;

  // copy the values from `slotMapInBuffer` to `slotMap` (size is `slotCount`)
  for(int i=0;i<slotCount;i++){
    slotMap[i]=*(slotMapInBuffer+i);
  }

  return SUCCESS;
}

int compareAttrs(union Attribute attr1, union Attribute attr2, int attrType) {

    double diff;
    if(attrType==1) // represents string from constants
        diff = strcmp(attr1.sVal, attr2.sVal);
    else
        diff = attr1.nVal - attr2.nVal;

    /*
    if diff > 0 then return 1
    if diff < 0 then return -1
    if diff = 0 then return 0
    */
    if(diff>0)
      return 1;
    else if(diff<0)
      return -1;
    else 
      return 0;
}


