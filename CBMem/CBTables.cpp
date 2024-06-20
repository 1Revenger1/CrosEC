//
//  CBTables.cpp
//  CrosEC
//
//  Created by Gwydien on 6/19/24.
//

#include "CBTables.hpp"

#include <IOKit/IOLib.h>

#define super IOService
OSDefineMetaClassAndStructors(CBTables, IOService);

IOService *CBTables::probe(IOService *service, int32_t *score) {
    CorebootTableHeader *header;
    
    if (super::probe(service, score) == nullptr || service == nullptr) {
        return nullptr;
    }
    
    memory = service->mapDeviceMemoryWithIndex(0);
    if (memory == nullptr) {
        IOLog("CBTables - Unable to get device memory!\n");
        return nullptr;
    }
    
    header = reinterpret_cast<CorebootTableHeader *>(memory->getVirtualAddress());
    
    if (memcmp(header->signature, "LBIO", sizeof(header->signature)) != 0) {
        IOLog("CBTables - Table header does not match!\n");
        return nullptr;
    }
    
    setProperty("Number of Table Entries", header->tableEntries, 32);
    setProperty("Size of table", header->tableBytes, 32);
    return this;
}

bool CBTables::start(IOService *service) {
    CorebootTableHeader *header;
    UInt8 *memPtr;
    
    if (!super::start(service)) {
        return false;
    }
    
    header = reinterpret_cast<CorebootTableHeader *>(memory->getVirtualAddress());
    memPtr = reinterpret_cast<UInt8 *>(memory->getVirtualAddress()) + sizeof(CorebootTableHeader);
    
    IOLog("CBTables - Parsing %d entries\n", header->tableEntries);
    
    for (size_t i = 0; i < header->tableEntries; i++) {
        CorebootEntry *entry = reinterpret_cast<CorebootEntry *>(memPtr);
        IOLog("CBTables - [%d] Type 0x%x of size 0x%x\n", header->tableEntries, entry->type, entry->size);
        
        switch (entry->type) {
            case 0x17:
                IOLog("CBTables - Coreboot Memory Console\n");
            case 0x2c:
                IOLog("CBTables - VPD Table at %p\n", reinterpret_cast<void *>(entry->entryAddr));
                break;
                
            default: break;
        }
        
        memPtr += entry->size;
    }
    
    return false;
}

void CBTables::free() {
    OSSafeReleaseNULL(memory);
    return super::free();
}
