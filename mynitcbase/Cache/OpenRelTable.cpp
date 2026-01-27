#include "OpenRelTable.h"

#include <cstring>
#include <cstdlib>


OpenRelTable::OpenRelTable() {

  // initialize relCache and attrCache with nullptr
  for (int i = 0; i < MAX_OPEN; ++i) {
    RelCacheTable::relCache[i] = nullptr;
    AttrCacheTable::attrCache[i] = nullptr;

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
  RelCacheTable::relCache[RELCAT_RELID] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[RELCAT_RELID]) = relCacheEntry;

   tableMetaInfo[RELCAT_RELID].free = false;
  strcpy(tableMetaInfo[RELCAT_RELID].relName, RELCAT_RELNAME);


  /**** setting up Attribute Catalog relation in the Relation Cache Table ****/

  // set up the relation cache entry for the attribute catalog similarly
  // from the record at RELCAT_SLOTNUM_FOR_ATTRCAT
    relCatBlock.getRecord(relCatRecord, RELCAT_SLOTNUM_FOR_ATTRCAT);
    
    // set the value at RelCacheTable::relCache[ATTRCAT_RELID]
      RelCacheTable::relCache[ATTRCAT_RELID] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
    RelCacheTable::recordToRelCatEntry(relCatRecord, &RelCacheTable::relCache[ATTRCAT_RELID]->relCatEntry);
      RelCacheTable::relCache[ATTRCAT_RELID]->recId.block = RELCAT_BLOCK;
  RelCacheTable::relCache[ATTRCAT_RELID]->recId.slot = RELCAT_SLOTNUM_FOR_ATTRCAT;

    tableMetaInfo[ATTRCAT_RELID].free = false;
  strcpy(tableMetaInfo[ATTRCAT_RELID].relName, ATTRCAT_RELNAME);

//student

  Attribute StdRelRecord[RELCAT_NO_ATTRS];
  relCatBlock.getRecord(StdRelRecord,2);

  RelCacheEntry relCacheStdEntry;
  RelCacheTable::recordToRelCatEntry(StdRelRecord, &relCacheStdEntry.relCatEntry);
  relCacheStdEntry.recId.block = RELCAT_BLOCK;
  relCacheStdEntry.recId.slot = 2;
  

  RelCacheTable::relCache[2] = (RelCacheEntry*)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[2]) = relCacheStdEntry;

    tableMetaInfo[2].free = false;
  strcpy(tableMetaInfo[2].relName, "Students");

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
    AttrCacheEntry* head = nullptr;
  AttrCacheEntry* prev = nullptr;
   for (int i = 0; i < RELCAT_NO_ATTRS; i++) {
    Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
    attrCatBlock.getRecord(attrCatRecord, i);
     AttrCacheEntry* entry = (AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
    AttrCacheTable::recordToAttrCatEntry(attrCatRecord, &entry->attrCatEntry);
    entry->recId.block = ATTRCAT_BLOCK;
    entry->recId.slot = i;
    entry->next = nullptr;
    
    if (head == nullptr) {
      head = entry;
    }
    if (prev != nullptr) {
      prev->next = entry;
    }
    prev = entry;
  }
  // set the next field in the last entry to nullptr

  AttrCacheTable::attrCache[RELCAT_RELID] = head/* head of the linked list */;

  /**** setting up Attribute Catalog relation in the Attribute Cache Table ****/

  // set up the attributes of the attribute cache similarly.
  // read slots 6-11 from attrCatBlock and initialise recId appropriately

  // set the value at AttrCacheTable::attrCache[ATTRCAT_RELID]

  head = nullptr;
  prev = nullptr;
  
  for (int i = RELCAT_NO_ATTRS; i < RELCAT_NO_ATTRS + ATTRCAT_NO_ATTRS; i++) {
    Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
    attrCatBlock.getRecord(attrCatRecord, i);
    
    // Allocate new entry
    AttrCacheEntry* entry = (AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
    AttrCacheTable::recordToAttrCatEntry(attrCatRecord, &entry->attrCatEntry);
    entry->recId.block = ATTRCAT_BLOCK;
    entry->recId.slot = i;
    entry->next = nullptr;
    
    if (head == nullptr) {
      head = entry;
    }
    if (prev != nullptr) {
      prev->next = entry;
    }
    prev = entry;
  }

  AttrCacheTable::attrCache[ATTRCAT_RELID] = head;

  head=nullptr;
  prev=nullptr;
  for(int slot = 12; slot <= 15; slot++){
    attrCatBlock.getRecord(attrCatRecord, slot);

    AttrCacheEntry* entry = (AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
    AttrCacheTable::recordToAttrCatEntry(attrCatRecord, &entry->attrCatEntry);

    entry->recId.block = ATTRCAT_BLOCK;
    entry->recId.slot = slot;
    entry->next = nullptr;

     if (head == nullptr) {
      head = entry;
    }
    if (prev != nullptr) {
      prev->next = entry;
    }
    prev = entry;
  }

  AttrCacheTable::attrCache[2] = head;


  // Q1
    

}

OpenRelTable::~OpenRelTable() {
  // free all the memory that you allocated in the constructor
   for (int i = 0; i < MAX_OPEN; i++) {
    if (RelCacheTable::relCache[i] != nullptr) {
      free(RelCacheTable::relCache[i]);
      RelCacheTable::relCache[i] = nullptr;
    }
  }
  
  // Free attribute cache entries (they are linked lists)
  for (int i = 0; i < MAX_OPEN; i++) {
    AttrCacheEntry* current = AttrCacheTable::attrCache[i];
    while (current != nullptr) {
      AttrCacheEntry* next = current->next;
      free(current);
      current = next;
    }
    AttrCacheTable::attrCache[i] = nullptr;
  }
}

/* This function will open a relation having name `relName`.
Since we are currently only working with the relation and attribute catalog, we
will just hardcode it. In subsequent stages, we will loop through all the relations
and open the appropriate one.
*/
int OpenRelTable::getRelId(char relName[ATTR_SIZE]) {

  // if relname is RELCAT_RELNAME, return RELCAT_RELID
  if(strcmp(relName,RELCAT_RELNAME)==0)return RELCAT_RELID;
  // if relname is ATTRCAT_RELNAME, return ATTRCAT_RELID
  if(strcmp(relName,ATTRCAT_RELNAME)==0)return ATTRCAT_RELID;


  return E_RELNOTOPEN;
}