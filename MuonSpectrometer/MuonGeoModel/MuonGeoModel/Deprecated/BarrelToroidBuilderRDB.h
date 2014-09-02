/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BarrelToroidBuilderRDB_H
#define BarrelToroidBuilderRDB_H

#include "MuonGeoModel/BarrelToroidBuilderV.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

class StoreGateSvc;
class IRDBAccessSvc;
class GeoPhysVol;
class IMessageSvc;

#include <string>

namespace MuonGM {

class BarrelToroidBuilderRDB : public BarrelToroidBuilderV 
{ 
 public:

  //  ToroidBuilderRDB(StoreGateSvc *pDetStore);
  BarrelToroidBuilderRDB(StoreGateSvc  *pDetStore,
		   IRDBAccessSvc *pRDBAccess, std::string geoTag, std::string geoNode);
  virtual ~BarrelToroidBuilderRDB(){};
  
  // Builds the Barrel Toroid
  void buildBTVacuumVessel( GeoPhysVol* container );
  void buildBTColdMass    ( GeoPhysVol* container );
  void buildBTVoussoirs   ( GeoPhysVol* container );
  void buildBTStruts      ( GeoPhysVol* container );
  void buildBTCryoring    ( GeoPhysVol* container );
  void buildRails	  ( GeoPhysVol* container );

 private:
  std::string getMaterial( std::string materialName );

  IRDBAccessSvc* m_pRDBAccess;
  StoreGateSvc*  m_pDetStore;

  IRDBRecordset_ptr m_Abrt;
  IRDBRecordset_ptr m_Aect;
  IRDBRecordset_ptr m_Feet;
  IRDBRecordset_ptr m_Rail;
  IMessageSvc*    m_msgSvc;

};

} // namespace MuonGM

#endif
