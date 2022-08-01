#include "channelMappings/CSVTablesBase.h"
#ifndef OFFLINE_DECODER
#include "ersL1Calo/Exceptions.h"
#else
#include "PathResolver/PathResolver.h"
#endif
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <vector>
/* \class CSVTables
 *  Simple class that when called generates a CSVWrapper
 *  for all 5 tables within data with access methods only when 
 *  an individual table is called
 */

CSVTablesBase::CSVTablesBase(std::string subsystem) : m_subsystem(subsystem){
    //Loop to set all table 'open' flags to false
    for(auto& openbools: m_table_opens){
        openbools = false;
    }
}
std::shared_ptr<CSVWrapper> CSVTablesBase::GetTable(int table_num) {
    //Check if table not open, the construct table
    std::string suffixName = m_subsystem + "Map"
                           + std::to_string(table_num)
                           + ".csv";
    if( not (m_table_opens.at(table_num-1) )){
#ifdef OFFLINE_DECODER
        auto mappingfilename = PathResolver::find_file("L1CaloFEXByteStream/2022-07-22/" + suffixName,"CALIBPATH");
        auto file = std::ifstream(mappingfilename, std::ifstream::in);
        if (!mappingfilename.empty() && file.good()) {
            m_tables.at(table_num-1) = std::make_shared<CSVWrapper> (file);
            m_table_opens.at(table_num-1) = true;
            return m_tables.at(table_num-1);
        } else {
            throw std::runtime_error(std::string("Cannot find file ") + suffixName);
        }
#else
        const char* l1caloRootEnv = std::getenv("L1CALO_ROOT");
        if (!l1caloRootEnv) {
            throw l1calo::EnvUndefined(ERS_HERE,"L1CALO_ROOT");
        }
        // To be able to use updated mappings while building the release
        // check for files in the source area then the installed area.
        std::string l1caloRoot(l1caloRootEnv);
        auto baseDir = l1caloRoot.substr(0,l1caloRoot.find_last_of("/"));
        std::vector<std::string> mapDirs = {
          baseDir + "/channelMappings/data/",
          baseDir + "/installed/share/data/channelMappings/",
        };
        for (auto& basePath: mapDirs) {
            auto mappingfilename = basePath + suffixName;
            auto file = std::ifstream(mappingfilename, std::ifstream::in);
            if (file.good()) {
                m_tables.at(table_num-1) = std::make_shared<CSVWrapper> (file);
                m_table_opens.at(table_num-1) = true;
                return m_tables.at(table_num-1);
            }
        }
        std::string suffixPath = suffixName + " (under " + baseDir + ")";
        throw l1calo::FileError(ERS_HERE,"open",suffixPath);
#endif
    }
    return m_tables.at(table_num-1);
}
