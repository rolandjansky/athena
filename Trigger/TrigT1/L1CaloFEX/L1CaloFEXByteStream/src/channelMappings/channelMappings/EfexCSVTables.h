#include "channelMappings/CSVTablesBase.h"

#ifndef L1CALO_EFEXCSVTABLES_H
#define L1CALO_EFEXCSVTABLES_H

class EfexCSVTables: public CSVTablesBase{
public:
    //Constructor
    static EfexCSVTables& getInstance();
private:
    EfexCSVTables(EfexCSVTables const&) = delete;
    void operator=(EfexCSVTables const&)  = delete;
    EfexCSVTables();
};

#endif //L1CALO_EFEXCSVTABLES_H
