//
//  tlb.cpp
//  COMP3500_Project5_TLB
//
//  Created by Robby Lagen on 12/3/16.
//  Copyright © 2016 Robby Lagen. All rights reserved.
//

#include "tlb.hpp"

int displayAddresses(bool displayAddrChoice, int count, logicAddressList_t logicAddrList, physAddressList_t physAddrList, valueList_t valueList) {
    if (displayAddrChoice == true) {
        for (int i = 0; i < count; i++) {
            cout << "Virtual Address: " << logicAddrList[i] << "; Physical Address: " << physAddrList[i] << "; Value: " << (int) valueList[i] << endl;
        }
    }
    
    return 0;
}

int readPhysicalMemory (paddress_t p_addr, frame physical_memory[NUM_FRAMES], value_t *value) {
    int offset = p_addr % FRAME_SIZE;
    int row = p_addr / FRAME_SIZE;
    
    const char x = physical_memory[row].page[offset];
    *value = x;
    
    return 0;
}

int update_all_lists(paddress_t physAddress, value_t value, physAddressList_t *physAddressList, valueList_t *valueList) {
    
    physAddressList->push_back(physAddress);
    valueList->push_back(value);
    
    return 0;
}

int output_all_lists(logicAddressList_t logicAddrList, physAddressList_t physAddrList, valueList_t valueList, int count) {
    ofstream file;
    file.open("vim_sim_output.txt");
    for (int i = 0; i < count; i++) {
        file << "Virtual Address: " << logicAddrList[i] << "; Physical Address: " << physAddrList[i] << "; Value: " << (int) valueList[i] << endl;
    }
    file.close();
    
    return 0;
}

int logicAdrrLoader(string fileName, vector<laddress_t> * logicAddrList) {
    int count = 0;
    ifstream instream(fileName);
    if (instream.fail()) {
        cout << "File failed to open.";
        exit(1);
    }
    
    unsigned int nextLogicAddr;
    while (!instream.eof()) {
        instream >> nextLogicAddr;
        logicAddrList->push_back(nextLogicAddr);
        count++;
    }
    instream.close();
    return count;
}

int extractLogicAddr(laddress_t address, page_t * pageNum, offset_t * offset) {
    *pageNum = address >> OFFSET_BITS;
    *offset = address & OFFSET_MASK;
    return 0;
}

// Page Table Initialization
int initPageTable(pageTable_t pageTable) {
    for (int i = 0; i < NUM_PAGES; i++) {
        pageTable[i] = EMPTY_PAGE;
    }
    return 0;
}

// TLB functions
int TLB_init(tlb *tlb) {
    unsigned int i;
    tlb->next_tlb_ptr = 0;
    for (i = 0; i < TLB_SIZE; i++)
        tlb->tlb_entry[i].valid = false;
    tlb->tlb_entry[i].age = 0;
    return 0;
}

int PhsyMemInit(frame physical_memory[NUM_FRAMES]){
    for (int i = 0; i < NUM_PAGES; i++) {
        physical_memory[i].valid = false;
    }
    return 0;
    
    
}

int searchTLB(page_t * pageNum, bool * isTlbHit, frame_t * frameNum, tlb * tlbSearch) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlbSearch->tlb_entry[i].valid && tlbSearch->tlb_entry[i].pageNum == *pageNum) {
            *isTlbHit = true;
            *frameNum = tlbSearch->tlb_entry[i].frameNum;
            return 0;
        }
    }
    *isTlbHit = false;
    return 0;
}

int searchPageTable(page_t pageNum, bool * isPageFault, frame_t * frameNum, pageTable_t page_Table) {
    if (page_Table[pageNum] == EMPTY_PAGE) {
        *isPageFault = true;
    }
    else {
        *frameNum = page_Table[pageNum];
    }
    return 0;
}

int TLB_display(tlb * tlb) {
    for (int i = 0; i < TLB_SIZE; i++) {
        cout << "TLB entry " << i << ", page num: " << tlb->tlb_entry[i].pageNum
        << ", frame num: " << tlb->tlb_entry[i].frameNum;
        if (tlb->tlb_entry[i].valid == false)             printf("Invalid\n");
        else printf("Valide\n");
    }
    
    
    return 0;
}

// Loading from backingstore
int load_frame_to_physical_memory(page_t pageNum, const char *backingStoreFileName, frame physical_memory[NUM_FRAMES], frame_t *frameNum) {
    FILE *file = fopen(backingStoreFileName, "r");
    fpos_t pos;
    char one_byte;
    
    
    if (file == 0) {
        printf("Could not open file: %s.\n", backingStoreFileName);
    }
    
    
    else {
        fseek(file, pageNum * FRAME_SIZE, SEEK_SET);
        //fgetpos(file, &pos);
        frame_t frame;
        for (frame = 0; frame < 256; frame++) {
            //If the frame is empty
            if (!physical_memory[frame].valid) {
                physical_memory[frame].valid = true;
                break;
            }
        }
        
        
        for (int i = 0; i < 256; i++) {
            fread(&one_byte, 1, 1, file);
            physical_memory[frame].page[i] = one_byte;
        }
        
        
        fclose(file);
        *frameNum = frame;
    }
    return 0;
}

int createPhysicalAddress(frame_t f_num, offset_t off, paddress_t *physical_addr) {
    *physical_addr = f_num * FRAME_SIZE + off;
    return 0;
}

int TLB_replacement_FIFO(page_t pageNum, frame_t frameNum, tlb *tlb) {
    for (int i = 0; i < TLB_SIZE; i++) {
        // If the tlb isn't full yet
        if (tlb->tlb_entry[i].valid == false) {
            tlb->tlb_entry[i].pageNum = pageNum;
            tlb->tlb_entry[i].frameNum = frameNum;
            tlb->tlb_entry[i].valid = true;
            return 0;
        }
    }
    
    
    // If the tlb is full
    tlb->tlb_entry[tlb->next_tlb_ptr].pageNum = pageNum;
    tlb->tlb_entry[tlb->next_tlb_ptr].frameNum = frameNum;
    tlb->tlb_entry[tlb->next_tlb_ptr].valid = true;
    if (tlb->next_tlb_ptr == 15) {
        tlb->next_tlb_ptr = 0;
    }
    else {
        tlb->next_tlb_ptr++;
    }
    return 0;
}

int TLB_replacement_LRU(page_t pageNum, frame_t frameNum, tlb *tlb) {
    for (int i = 0; i < TLB_SIZE; i++) {
        // If the tlb isn't full yet
        if (tlb->tlb_entry[i].valid == false) {
            tlb->tlb_entry[i].pageNum = pageNum;
            tlb->tlb_entry[i].frameNum = frameNum;
            tlb->tlb_entry[i].valid = true;
            return 0;
        }
    }
    // Find the oldest tlb
    int oldestAge = 0;
    int oldestIndex = 0;
    for (int i = 0; i < TLB_SIZE; i++) {
        if (oldestAge < tlb->tlb_entry[i].age) {
            oldestAge = tlb->tlb_entry[i].age;
            oldestIndex = i;
        }
    }    
    // Replace the oldest tlb
    tlb->tlb_entry[oldestIndex].pageNum = pageNum;
    tlb->tlb_entry[oldestIndex].frameNum = frameNum;
    tlb->tlb_entry[oldestIndex].age = 0;
    tlb->tlb_entry[oldestIndex].valid = true;
    
    
    return 0;
}

int handlePageFault(page_t p_num, frame_t *frame_num, frame physical_memory[NUM_FRAMES], pageTable_t p_table, tlb tlb) {
    load_frame_to_physical_memory(p_num, "BACKING_STORE", physical_memory, frame_num);
    p_table[p_num] = *frame_num;
    
    
    return 0;
}




