#include "OpenRelTable.h"

#include <cstring>
#include <cstdlib>

OpenRelTableMetaInfo OpenRelTable::tableMetaInfo[MAX_OPEN];

OpenRelTable::OpenRelTable()
{

  // initialize relCache and attrCache with nullptr
  for (int i = 0; i < MAX_OPEN; ++i)
  {
    RelCacheTable::relCache[i] = nullptr;
    AttrCacheTable::attrCache[i] = nullptr;
    tableMetaInfo[i].free = true;
  }

  /************ Setting up Relation Cache entries ************/
  // (we need to populate relation cache with entries for the relation catalog
  //  and attribute catalog.)

  /**** setting up Relation Catalog relation in the Relation Cache Table****/
  RecBuffer relCatBlock(RELCAT_BLOCK);

  Attribute relCatRecord[RELCAT_NO_ATTRS];
  relCatBlock.getRecord(relCatRecord, RELCAT_SLOTNUM_FOR_RELCAT);

  struct RelCacheEntry relCacheEntry;
  RelCacheTable::recordToRelCatEntry(relCatRecord, &relCacheEntry.relCatEntry);
  relCacheEntry.recId.block = RELCAT_BLOCK;
  relCacheEntry.recId.slot = RELCAT_SLOTNUM_FOR_RELCAT;

  // allocate this on the heap because we want it to persist outside this function
  RelCacheTable::relCache[RELCAT_RELID] = (struct RelCacheEntry *)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[RELCAT_RELID]) = relCacheEntry;

  /**** setting up Attribute Catalog relation in the Relation Cache Table ****/

  // set up the relation cache entry for the attribute catalog similarly
  // from the record at RELCAT_SLOTNUM_FOR_ATTRCAT
  relCatBlock.getRecord(relCatRecord, RELCAT_SLOTNUM_FOR_ATTRCAT);

  // set the value at RelCacheTable::relCache[ATTRCAT_RELID]
  RelCacheTable::relCache[ATTRCAT_RELID] = (struct RelCacheEntry *)malloc(sizeof(RelCacheEntry));
  RelCacheTable::recordToRelCatEntry(relCatRecord, &RelCacheTable::relCache[ATTRCAT_RELID]->relCatEntry);
  RelCacheTable::relCache[ATTRCAT_RELID]->recId.block = RELCAT_BLOCK;
  RelCacheTable::relCache[ATTRCAT_RELID]->recId.slot = RELCAT_SLOTNUM_FOR_ATTRCAT;
  // student

  // Attribute StdRelRecord[RELCAT_NO_ATTRS];
  // relCatBlock.getRecord(StdRelRecord,2);

  // RelCacheEntry relCacheStdEntry;
  // RelCacheTable::recordToRelCatEntry(StdRelRecord, &relCacheStdEntry.relCatEntry);
  // relCacheStdEntry.recId.block = RELCAT_BLOCK;
  // relCacheStdEntry.recId.slot = 2;

  // RelCacheTable::relCache[2] = (RelCacheEntry*)malloc(sizeof(RelCacheEntry));
  // *(RelCacheTable::relCache[2]) = relCacheStdEntry;

  /************ Setting up Attribute cache entries ************/
  // (we need to populate attribute cache with entries for the relation catalog
  //  and attribute catalog.)

  /**** setting up Relation Catalog relation in the Attribute Cache Table ****/
  RecBuffer attrCatBlock(ATTRCAT_BLOCK);

  Attribute attrCatRecord[ATTRCAT_NO_ATTRS];

  // iterate through all the attributes of the relation catalog and create a linked
  // list of AttrCacheEntry (slots 0 to 5)
  // for each of the entries, set
  //    attrCacheEntry.recId.block = ATTRCAT_BLOCK;
  //    attrCacheEntry.recId.slot = i   (0 to 5)
  //    and attrCacheEntry.next appropriately
  // NOTE: allocate each entry dynamically using malloc
  AttrCacheEntry *head = nullptr;
  AttrCacheEntry *prev = nullptr;
  for (int i = 0; i < RELCAT_NO_ATTRS; i++)
  {
    Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
    attrCatBlock.getRecord(attrCatRecord, i);
    AttrCacheEntry *entry = (AttrCacheEntry *)malloc(sizeof(AttrCacheEntry));
    AttrCacheTable::recordToAttrCatEntry(attrCatRecord, &entry->attrCatEntry);
    entry->recId.block = ATTRCAT_BLOCK;
    entry->recId.slot = i;
    entry->next = nullptr;

    if (head == nullptr)
    {
      head = entry;
    }
    if (prev != nullptr)
    {
      prev->next = entry;
    }
    prev = entry;
  }
  // set the next field in the last entry to nullptr

  AttrCacheTable::attrCache[RELCAT_RELID] = head /* head of the linked list */;

  /**** setting up Attribute Catalog relation in the Attribute Cache Table ****/

  // set up the attributes of the attribute cache similarly.
  // read slots 6-11 from attrCatBlock and initialise recId appropriately

  // set the value at AttrCacheTable::attrCache[ATTRCAT_RELID]

  head = nullptr;
  prev = nullptr;

  for (int i = RELCAT_NO_ATTRS; i < RELCAT_NO_ATTRS + ATTRCAT_NO_ATTRS; i++)
  {
    Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
    attrCatBlock.getRecord(attrCatRecord, i);

    // Allocate new entry
    AttrCacheEntry *entry = (AttrCacheEntry *)malloc(sizeof(AttrCacheEntry));
    AttrCacheTable::recordToAttrCatEntry(attrCatRecord, &entry->attrCatEntry);
    entry->recId.block = ATTRCAT_BLOCK;
    entry->recId.slot = i;
    entry->next = nullptr;

    if (head == nullptr)
    {
      head = entry;
    }
    if (prev != nullptr)
    {
      prev->next = entry;
    }
    prev = entry;
  }

  AttrCacheTable::attrCache[ATTRCAT_RELID] = head;

  // head=nullptr;
  // prev=nullptr;
  // for(int slot = 12; slot <= 15; slot++){
  //   attrCatBlock.getRecord(attrCatRecord, slot);

  //   AttrCacheEntry* entry = (AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
  //   AttrCacheTable::recordToAttrCatEntry(attrCatRecord, &entry->attrCatEntry);

  //   entry->recId.block = ATTRCAT_BLOCK;
  //   entry->recId.slot = slot;
  //   entry->next = nullptr;

  //    if (head == nullptr) {
  //     head = entry;
  //   }
  //   if (prev != nullptr) {
  //     prev->next = entry;
  //   }
  //   prev = entry;
  // }

  // AttrCacheTable::attrCache[2] = head;
  tableMetaInfo[RELCAT_RELID].free = false;
  tableMetaInfo[ATTRCAT_RELID].free = false;
  strcpy(tableMetaInfo[RELCAT_RELID].relName, RELCAT_RELNAME);
  strcpy(tableMetaInfo[ATTRCAT_RELID].relName, ATTRCAT_RELNAME);
}

OpenRelTable::~OpenRelTable()
{
  // free all the memory that you allocated in the constructor
  for (int i = 0; i < MAX_OPEN; i++)
  {
    if (RelCacheTable::relCache[i] != nullptr)
    {
      free(RelCacheTable::relCache[i]);
      RelCacheTable::relCache[i] = nullptr;
    }
  }

  // Free attribute cache entries (they are linked lists)
  for (int i = 0; i < MAX_OPEN; i++)
  {
    AttrCacheEntry *current = AttrCacheTable::attrCache[i];
    while (current != nullptr)
    {
      AttrCacheEntry *next = current->next;
      free(current);
      current = next;
    }
    AttrCacheTable::attrCache[i] = nullptr;
  }

  // close all open relations (from rel-id = 2 onwards. Why?)
  for (int i = 2; i < MAX_OPEN; ++i)
  {
    if (!tableMetaInfo[i].free)
    {
      OpenRelTable::closeRel(i); // we will implement this function later
    }
  }
}

/* This function will open a relation having name `relName`.
Since we are currently only working with the relation and attribute catalog, we
will just hardcode it. In subsequent stages, we will loop through all the relations
and open the appropriate one.
*/
int OpenRelTable::getRelId(char relName[ATTR_SIZE])
{

  // if relname is RELCAT_RELNAME, return RELCAT_RELID
  if (strcmp(relName, RELCAT_RELNAME) == 0)
    return RELCAT_RELID;
  // if relname is ATTRCAT_RELNAME, return ATTRCAT_RELID
  if (strcmp(relName, ATTRCAT_RELNAME) == 0)
    return ATTRCAT_RELID;

  for (int i = 2; i < MAX_OPEN; ++i)
  {
    if (strcmp(tableMetaInfo[i].relName, relName) == 0)
    {
      return i;
    }
  }

  return E_RELNOTOPEN;
}

int OpenRelTable::getFreeOpenRelTableEntry()
{

  /* traverse through the tableMetaInfo array,
    find a free entry in the Open Relation Table.*/

  for (int i = 2; i < MAX_OPEN; ++i)
  {
    if (tableMetaInfo[i].free)
    {
      return i;
    }
  }
  return E_CACHEFULL;
  // if found return the relation id, else return E_CACHEFULL.
}

int OpenRelTable::openRel(char relName[ATTR_SIZE])
{

  /* the relation `relName` already has an entry in the Open Relation Table */
  int relId = OpenRelTable::getRelId(relName);
  if (relId != E_RELNOTOPEN)
  {
    // (checked using OpenRelTable::getRelId())
    return relId;
    // return that relation id;
  }

  /* find a free slot in the Open Relation Table
     using OpenRelTable::getFreeOpenRelTableEntry(). */
  relId = OpenRelTable::getFreeOpenRelTableEntry();

  if (relId == E_CACHEFULL)
  {
    return E_CACHEFULL;
  }

  // let relId be used to store the free slot.
  // int relId;

  /****** Setting up Relation Cache entry for the relation ******/

  /* search for the entry with relation name, relName, in the Relation Catalog using
      BlockAccess::linearSearch().
      Care should be taken to reset the searchIndex of the relation RELCAT_RELID
      before calling linearSearch().*/

  Attribute attr;
  strcpy(attr.sVal, relName);
  RelCacheTable::resetSearchIndex(RELCAT_RELID);

  // relcatRecId stores the rec-id of the relation `relName` in the Relation Catalog.
  RecId relcatRecId = BlockAccess::linearSearch(RELCAT_RELID, (char *)RELCAT_ATTR_RELNAME, attr, EQ);

  if (relcatRecId.block == -1 && relcatRecId.slot == -1)
  {
    // (the relation is not found in the Relation Catalog.)
    return E_RELNOTEXIST;
  }

  /* read the record entry corresponding to relcatRecId and create a relCacheEntry
      on it using RecBuffer::getRecord() and RelCacheTable::recordToRelCatEntry().
      update the recId field of this Relation Cache entry to relcatRecId.
      use the Relation Cache entry to set the relId-th entry of the RelCacheTable.
    NOTE: make sure to allocate memory for the RelCacheEntry using malloc()
  */

  RelCacheEntry *relEntry = (RelCacheEntry *)malloc(sizeof(RelCacheEntry));
  RecBuffer relBuffer(relcatRecId.block);
  Attribute relRecord[RELCAT_NO_ATTRS];
  relBuffer.getRecord(relRecord, relcatRecId.slot);
  RelCacheTable::recordToRelCatEntry(relRecord, &(relEntry->relCatEntry));
  relEntry->recId = relcatRecId;
  RelCacheTable::relCache[relId] = relEntry;

  /****** Setting up Attribute Cache entry for the relation ******/

  // let listHead be used to hold the head of the linked list of attrCache entries.
  AttrCacheEntry *listHead = (AttrCacheEntry *)malloc(sizeof(AttrCacheEntry));

  RelCacheTable::resetSearchIndex(ATTRCAT_RELID);
  AttrCacheEntry *prev = nullptr;
  int attrCount = 0;

  /*iterate over all the entries in the Attribute Catalog corresponding to each
  attribute of the relation relName by multiple calls of BlockAccess::linearSearch()
  care should be taken to reset the searchIndex of the relation, ATTRCAT_RELID,
  corresponding to Attribute Catalog before the first call to linearSearch().*/
  strcpy(attr.sVal, relName);
  while (true)
  {
    /* let attrcatRecId store a valid record id an entry of the relation, relName,
    in the Attribute Catalog.*/
      RecId attrcatRecId = BlockAccess::linearSearch(ATTRCAT_RELID, (char *)ATTRCAT_ATTR_RELNAME, attr, EQ);
    // attr,  // This should contain relName

    if (attrcatRecId.block == -1 && attrcatRecId.slot == -1)
    {
      break;
    }

    /* read the record entry corresponding to attrcatRecId and create an
    Attribute Cache entry on it using RecBuffer::getRecord() and
    AttrCacheTable::recordToAttrCatEntry().
    update the recId field of this Attribute Cache entry to attrcatRecId.
    add the Attribute Cache entry to the linked list of listHead .*/
    // NOTE: make sure to allocate memory for the AttrCacheEntry using malloc()

    AttrCacheEntry *attrEntry = (AttrCacheEntry *)malloc(sizeof(AttrCacheEntry));

    RecBuffer attrCatBuffer(attrcatRecId.block);
    Attribute attrRecord[ATTRCAT_NO_ATTRS];
    attrCatBuffer.getRecord(attrRecord, attrcatRecId.slot);

    AttrCacheTable::recordToAttrCatEntry(attrRecord, &(attrEntry->attrCatEntry));

    attrEntry->recId = attrcatRecId;
    attrEntry->next = nullptr;

    if (attrCount == 0)
    {
      *listHead = *attrEntry;
      free(attrEntry);
      prev = listHead;
    }
    else
    {
      prev->next = attrEntry;
      prev = attrEntry;
    }
    attrCount++;
  }

  // set the relIdth entry of the AttrCacheTable to listHead.
  AttrCacheTable::attrCache[relId] = listHead;

  /****** Setting up metadata in the Open Relation Table for the relation******/

  // update the relIdth entry of the tableMetaInfo with free as false and
  // relName as the input.
  tableMetaInfo[relId].free = false;
  strcpy(tableMetaInfo[relId].relName, relName);

  return relId;
}

int OpenRelTable::closeRel(int relId)
{
  if (relId == RELCAT_RELID || relId == ATTRCAT_RELID)
  {
    return E_NOTPERMITTED;
  }

  if (relId < 0 or relId >= MAX_OPEN)
  {
    return E_OUTOFBOUND;
  }

  if (tableMetaInfo[relId].free)
  { // free slot
    return E_RELNOTOPEN;
  }

  // free the memory allocated in the relation and attribute caches which was
  // allocated in the OpenRelTable::openRel() function

  // Free the RelCacheEntry
  if (RelCacheTable::relCache[relId] != nullptr)
  {
    free(RelCacheTable::relCache[relId]);
  }

  // Free the linked list of AttrCacheEntry
  AttrCacheEntry *current = AttrCacheTable::attrCache[relId];
  while (current != nullptr)
  {
    AttrCacheEntry *next = current->next;
    free(current);
    current = next;
  }

  // update `tableMetaInfo` to set `relId` as a free slot
  tableMetaInfo[relId].free = true;
  // update `relCache` and `attrCache` to set the entry at `relId` to nullptr
  RelCacheTable::relCache[relId] = nullptr;
  AttrCacheTable::attrCache[relId] = nullptr;

  return SUCCESS;
}
