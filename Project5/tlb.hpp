//
//  tlb.hpp
//  COMP3500_Project5_TLB
//
//  Created by Robby Lagen on 12/3/16.
//  Copyright © 2016 Robby Lagen. All rights reserved.
//

#ifndef tlb_hpp
#define tlb_hpp

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>

using namespace std;

/* offset is used to address the location within a page */
#define OFFSET_BITS 8
/* mask is used to extract the offset from a logical address */
#define OFFSET_MASK 0xFF

#define EMPTY_PAGE 3435973836
#define NUM_PAGES 256
#define NUM_FRAMES 256
#define PAGE_SIZE 256
#define FRAME_SIZE 256
#define TLB_SIZE 16
#define LOGIC_ADDR_LIST_SIZE 1024

/* There is no byte data type in C */
typedef unsigned char byte;

/* new data type of pages, frames, and offset */
typedef unsigned int page_t;
typedef char value_t;
typedef unsigned int frame_t;
typedef unsigned int offset_t;
typedef unsigned int laddress_t;
typedef unsigned int paddress_t;
typedef char physical_memory_t[NUM_FRAMES][PAGE_SIZE];
typedef vector<laddress_t> logicAddressList_t;
typedef vector<paddress_t> physAddressList_t;
typedef vector<char> valueList_t;

typedef struct {
    page_t pageNum;
    frame_t frameNum;
    bool valid;
    int age;
} tlbEntry;


typedef struct {
    tlbEntry tlb_entry[TLB_SIZE];
    unsigned int next_tlb_ptr;
} tlb;


typedef struct {
    char page[PAGE_SIZE];
    bool valid;
} frame;

//typedef frame physical_memory_t[NUM_FRAMES];
typedef frame_t pageTable_t[NUM_PAGES];

int logicAdrrLoader(string fileName, vector<laddress_t> * logicAddrList);
int extractLogicAddr(laddress_t address, page_t * pageNum, offset_t * offset);
int initPageTable(pageTable_t pageTable);
int TLB_init(tlb *tlb);
int PhsyMemInit(frame physical_memory[NUM_FRAMES]);
int searchTLB(page_t * pageNum, bool * isTlbHit, frame_t * frameNum, tlb * tlbSearch);
int searchPageTable(page_t pageNum, bool * isPageFault, frame_t * frameNum, pageTable_t page_Table);
int handlePageFault(page_t p_num, frame_t *frame_num, frame physical_memory[NUM_FRAMES], pageTable_t p_table, tlb tlb);
int load_frame_to_physical_memory(page_t pageNum, const char *backingStoreFileName, frame physical_memory[NUM_FRAMES], frame_t *frameNum);
int createPhysicalAddress(frame_t f_num, offset_t off, paddress_t *physical_addr);
int TLB_replacement_FIFO(page_t pageNum, frame_t frameNum, tlb *tlb);
int TLB_replacement_LRU(page_t pageNum, frame_t frameNum, tlb *tlb);
int readPhysicalMemory (paddress_t p_addr, frame physical_memory[NUM_FRAMES], value_t *value);
int update_all_lists(paddress_t physAddress, value_t value, physAddressList_t *physAddressList, valueList_t *valueList);
int output_all_lists(logicAddressList_t logicAddrList, physAddressList_t physAddrList, valueList_t valueList, int count);
int displayAddresses(bool displayAddrChoice, int count, logicAddressList_t logicAddrList, physAddressList_t physAddrList, valueList_t valueList);

#endif /* tlb_hpp */
