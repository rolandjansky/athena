/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EndCapToroidBuilderRDB_H
#define EndCapToroidBuilderRDB_H

#include "MuonGeoModel/EndCapToroidBuilderV.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

class StoreGateSvc;
class IRDBAccessSvc;
class GeoPhysVol;
class IMessageSvc;

#include <string>

namespace MuonGM {
	
class EndCapToroidBuilderRDB : public EndCapToroidBuilderV 
{ 
 public:

  //  ToroidBuilderRDB(StoreGateSvc *pDetStore);
  EndCapToroidBuilderRDB(StoreGateSvc  *pDetStore,
		   IRDBAccessSvc *pRDBAccess, std::string geoTag, std::string geoNode);
  virtual ~EndCapToroidBuilderRDB(){};
  
    //  Builds the Endcap Toroid
  void buildECTVacuumVessel( GeoPhysVol* container ); 
  void buildECTConductorBox( GeoPhysVol* container );
  void buildECTKeystoneBox ( GeoPhysVol* container );
  void buildECTServiceTower( GeoPhysVol* container );

 private:
  std::string getMaterial( std::string materialName );

  IRDBAccessSvc* m_pRDBAccess;
  StoreGateSvc*  m_pDetStore;

  IRDBRecordset_ptr m_Abrt;
  IRDBRecordset_ptr m_Aect;
  IRDBRecordset_ptr m_Feet;
  IRDBRecordset_ptr m_Rail;
  IRDBRecordset_ptr m_Ecst;

  IMessageSvc*    m_msgSvc;
};

} // namespace MuonGM

#endif
