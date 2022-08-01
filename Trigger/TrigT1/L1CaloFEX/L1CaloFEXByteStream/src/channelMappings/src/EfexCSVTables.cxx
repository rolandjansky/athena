#include "channelMappings/EfexCSVTables.h"

EfexCSVTables::EfexCSVTables() : CSVTablesBase("eFex") {}

EfexCSVTables& EfexCSVTables::getInstance(){
    static thread_local EfexCSVTables instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
}

