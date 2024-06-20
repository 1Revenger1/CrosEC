

#include <IOKit/IOService.h>

struct CorebootTableHeader {
    uint8_t signature[4];
    uint32_t headerBytes;
    uint32_t headerChecksum;
    uint32_t tableBytes;
    uint32_t tableChecksum;
    uint32_t tableEntries;
};

struct CorebootEntry {
    uint32_t type;
    uint32_t size;
    union {
        IOPhysicalAddress64 entryAddr;
    };
};

static_assert(sizeof(CorebootTableHeader) == sizeof(uint32_t) * 6, "Invalid table header size!");

class CBTables : public IOService {
    OSDeclareDefaultStructors(CBTables);

public:
    virtual IOService *probe(IOService *service, int32_t *score) override;
    virtual bool start(IOService *service) override;
    
    virtual void free() override;
    
private:
    IOMemoryMap *memory{nullptr};
};
