// VirtualMemoryManager.cpp : Defines the entry point for the console application.
//

#include "tlb.hpp"

/*
 * In C language, there is no binary format in printf
 * You must implement the following functions to print binary format
 * itob16() and itob8() are modified from itob() by Xiao Qin.
 */
char *itob(int x);
char *itob16(int x);
char *itob8(int x);

// Flags used to determine user's choice
bool willDisplayAddr;
bool willUseFIFO;

int main() {
    cout << "Welcome to Group 1's VM Simulator Version 1.0\n\n"
    << "Number of logical pages: " << NUM_PAGES << endl
    << "Page size: " << PAGE_SIZE << " bytes" << endl
    << "Page table size: " << NUM_FRAMES << endl
    << "TLB size: " << TLB_SIZE << endl
    << "Number of physical frames: " << NUM_FRAMES << endl
    << "Physical memory size: 65,536 bytes\n\n";
    
    
    string displayAddrChoice;
    
    
    cout << "Display physical addresses? [y or n] ";
    cin >> displayAddrChoice;
    if (displayAddrChoice == "y" || displayAddrChoice == "Y") {
        willDisplayAddr = true;
    }
    else {
        willDisplayAddr = false;
    }
    
    
    int strategyChoice;
    
    
    cout << "Choose TLB Replacement Strategy [1: FIFO, 2: LRU] ";
    cin >> strategyChoice;
    if (strategyChoice == 1) {
        willUseFIFO = true;
    }
    else {
        willUseFIFO = false;
    }
    
    
    page_t pageNum;
    frame_t frameNum;
    offset_t offset;
    
    
    // Addresses
    laddress_t logicAddress;
    paddress_t physicalAddress;
    value_t value;
    
    
    // The TLB and page table
    tlb tlb;
    pageTable_t pageTable;
    
    
    // Flags to keep track of TLB and page faults
    bool tlbHit;
    bool pageFault;
    
    
    // Simulated main memory
    frame physical_memory[NUM_FRAMES];
    //physical_memory_t physical_memory;
    
    
    // Address Lists
    physAddressList_t physAddressList;
    logicAddressList_t logicAddressList;
    valueList_t valueList;
    
    
    // File Names
    const char input_file[] = "InputFile.txt";
    const char backing_store_file_name[] = "BACKING_STORE";
    
    
    // Initialize the tlb and page table
    TLB_init(&tlb);
    initPageTable(pageTable);
    PhsyMemInit(physical_memory);
    
    // page fault and tlb hit counter
    double amountPageFaults = 0;
    double amountTLBHits = 0;
    
    // Load the logical addresses from the test file
    int count = logicAdrrLoader(input_file, &logicAddressList);
    
    
    for (int i = 0; i < count; i++) {
        // Get a logical address, its pageNum, and offset
        extractLogicAddr(logicAddressList[i], &pageNum, &offset);
        
        
        // Search the TLB
        searchTLB(&pageNum, &tlbHit, &frameNum, &tlb);
        
        
        if (tlbHit == true) {
            amountTLBHits++;
            createPhysicalAddress(frameNum, offset, &physicalAddress);
        }
        
        
        // TLB miss
        else {
            searchPageTable(pageNum, &pageFault, &frameNum, pageTable);
            
            
            // page hit
            if (pageFault == false) {
                createPhysicalAddress(frameNum, offset, &physicalAddress);
                
                
                // TLB replacement methods
                if (willUseFIFO == true) {
                    TLB_replacement_FIFO(pageNum, frameNum, &tlb);
                }
                else {
                    TLB_replacement_LRU(pageNum, frameNum, &tlb);
                }
            }
            
            
            // Page Fault
            else {
                amountPageFaults++;
                handlePageFault(pageNum, &frameNum, physical_memory, pageTable, tlb);
                
                
                // TLB replacement methods
                if (willUseFIFO == true) {
                    TLB_replacement_FIFO(pageNum, frameNum, &tlb);
                }
                else {
                    TLB_replacement_LRU(pageNum, frameNum, &tlb);
                }
                
                
                createPhysicalAddress(frameNum, offset, &physicalAddress);
            }
        } // End of TLB Miss
        
        
        // Read one-byte value from the physical memory
        readPhysicalMemory(physicalAddress, physical_memory, &value);
        
        // Update the address lists
        update_all_lists(physicalAddress, value, &physAddressList, &valueList);
    } // End of logicAddrList
    
    
    // Output and display the address lists into an output file
    output_all_lists(logicAddressList, physAddressList, valueList, count);
    displayAddresses(willDisplayAddr, count, logicAddressList, physAddressList, valueList);
    double pageFaultPercent = (amountPageFaults / count) * 100;
    double tlbHitPercent = (amountTLBHits / count) * 100;
    
    cout << "\nPage Fault Rate: " << pageFaultPercent << "%"
    << "\nTLB Hit Rate: " << tlbHitPercent << "%\n\n"
    << "Check the results in the outputfile: vm_sim_output.txt\n\n";
    
    return 0;
}
