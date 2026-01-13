#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include "FrontendInterface/FrontendInterface.h"
#include <iostream>
using namespace std;

// int main(int argc, char *argv[]) {
//   Disk disk_run;

//   unsigned char buffer[BLOCK_SIZE];
//   Disk::readBlock(buffer, 7000);
//   char message[] = "hello";
//   memcpy(buffer + 20, message, 6);
//   Disk::writeBlock(buffer, 7000);

//   unsigned char buffer2[BLOCK_SIZE];
//   char message2[6];
//   Disk::readBlock(buffer2, 7000);
//   memcpy(message2, buffer2 + 20, 6);
//   std::cout << message2;

//   unsigned char buffer3[BLOCK_SIZE];
//   Disk::readBlock(buffer3, 0);
//   for(int i=0;i<8;i++){
//     cout<<(int)buffer3[i]<<" ";
//   }
//   return 0;
// }

int main(int argc, char *argv[])
{
  Disk disk_run;

  // create objects for the relation catalog and attribute catalog
  RecBuffer relCatBuffer(RELCAT_BLOCK);
  RecBuffer attrCatBuffer(ATTRCAT_BLOCK);

  HeadInfo relCatHeader;
  HeadInfo attrCatHeader;

  // load the headers of both the blocks into relCatHeader and attrCatHeader.
  // (we will implement these functions later)
  relCatBuffer.getHeader(&relCatHeader);
  attrCatBuffer.getHeader(&attrCatHeader);

  for (int i = 0; i < relCatHeader.numEntries; i++)
  {

    Attribute relCatRecord[RELCAT_NO_ATTRS]; // will store the record from the relation catalog

    relCatBuffer.getRecord(relCatRecord, i);

    printf("Relation: %s\n", relCatRecord[RELCAT_REL_NAME_INDEX].sVal);

  // for (int j=0;j<attrCatHeader.numEntries;j++) {

  //   // declare attrCatRecord and load the attribute catalog entry into it
  // Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
  // attrCatBuffer.getRecord(attrCatRecord,j);

  //   if (strcmp(attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal, relCatRecord[RELCAT_REL_NAME_INDEX].sVal) == 0) {
  //     const char *attrType = attrCatRecord[ATTRCAT_ATTR_TYPE_INDEX].nVal == NUMBER ? "NUM" : "STR";
  //     printf("  %s: %s\n", attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, attrType);
  //   }
  // }

  int curr = ATTRCAT_BLOCK;
  while (curr != -1)
  {
    RecBuffer attrCatBuffer(curr);
    HeadInfo attrCatHeader;
    attrCatBuffer.getHeader(&attrCatHeader);

    for (int j = 0; j < attrCatHeader.numEntries; j++)
    {

      // declare attrCatRecord and load the attribute catalog entry into it
      Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
      attrCatBuffer.getRecord(attrCatRecord, j);

      if (strcmp(attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal, "Students") == 0)
      {
        const char *attrType = attrCatRecord[ATTRCAT_ATTR_TYPE_INDEX].nVal == NUMBER ? "NUM" : "STR";
        printf("  %s: %s\n", attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, attrType);

        // if (strcmp(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, "Class") == 0)
        // {
        //   strcpy(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, "Batch");
        //   unsigned char buffer[BLOCK_SIZE];
        //   Disk::readBlock(buffer, curr);

        //   int recordSize = attrCatHeader.numAttrs * ATTR_SIZE;
        //   int offset = HEADER_SIZE + attrCatHeader.numSlots + (recordSize * j);

        //   // Copy the modified record back to the buffer
        //   memcpy(buffer + offset, attrCatRecord, recordSize);

        //   // Write the modified block back to disk
        //   Disk::writeBlock(buffer, curr);
        //   return 0;
        // }
      }
    }
    curr = attrCatHeader.rblock;
  }
  printf("\n");
  }

  return 0;
}