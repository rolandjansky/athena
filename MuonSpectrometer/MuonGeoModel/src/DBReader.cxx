/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/DBReader.h"
#include "StoreGate/StoreGateSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"

// #include "MuonGeoModel/TgcReadoutParams.h"

namespace MuonGM {
    
DBReader::DBReader(StoreGateSvc *pDetStore)
    :
    _mgr(0),
    _pDetStore(pDetStore)
{
    SCdbaccess = StatusCode::SUCCESS;
    m_msgSvc = Athena::getMessageSvc();
}
    
void DBReader::setGeometryVersion(std::string geoVersion)
{
    version = geoVersion;
}
std::string DBReader::getGeometryVersion() const
{
    return version;
}

std::string DBReader::TGCreadoutName(int ichtyp) 
{
    if ( _tgcReadoutMapping.size() == 0)
    { // first time fill the vector 
        //std::cout<<" filling the map "<<std::endl;
        _tgcReadoutMapping.push_back("T1F1");  // 1

        _tgcReadoutMapping.push_back("T1E1");  // 2
        _tgcReadoutMapping.push_back("T1E2");
        _tgcReadoutMapping.push_back("T1E3");
        _tgcReadoutMapping.push_back("T1E4");

        _tgcReadoutMapping.push_back("T2F1");  // 6

        _tgcReadoutMapping.push_back("T2E1");  // 7
        _tgcReadoutMapping.push_back("T2E2");
        _tgcReadoutMapping.push_back("T2E3");
        _tgcReadoutMapping.push_back("T2E4");
        _tgcReadoutMapping.push_back("T2E5");  // 11

        _tgcReadoutMapping.push_back("T3F1");  // 12 

        _tgcReadoutMapping.push_back("T3E1");  // 13
        _tgcReadoutMapping.push_back("T3E2");
        _tgcReadoutMapping.push_back("T3E3");
        _tgcReadoutMapping.push_back("T3E4");
        _tgcReadoutMapping.push_back("T3E5");  // 17
        
        _tgcReadoutMapping.push_back("T4F1");  // 18 

        _tgcReadoutMapping.push_back("T4E1");  // 19
    }

    if (ichtyp<1 || ichtyp>19) {
        std::cerr<<" DBReader::TGCreadoutName  - ichtype "
                 <<ichtyp<<" out of range 1-19"<<std::endl;
        return "XXXY";
    }
    return _tgcReadoutMapping[ichtyp-1];
}

} // namespace MuonGM

