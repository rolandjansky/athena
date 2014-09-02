/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ShieldBuilderRDB_H
#define ShieldBuilderRDB_H

#include "MuonGeoModel/ShieldBuilderV.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

class StoreGateSvc;
class IRDBAccessSvc;
class GeoPhysVol;
class IMessageSvc;

#include <string>

namespace MuonGM {

class ShieldBuilderRDB : public ShieldBuilderV 
{
 
 public:

  ShieldBuilderRDB( StoreGateSvc  *pDetStore,
		    IRDBAccessSvc *pRDBAccess, 
                    std::string    geoTag, std::string geoNode );
  virtual ~ShieldBuilderRDB(){};
  
  // Builds the JD, JF, JT Shielding
  void buildDiskShielding   ( GeoPhysVol* container );
  void buildToroidShielding ( GeoPhysVol* container );
  void buildForwardShielding( GeoPhysVol* container );


 private:

  std::string getMaterial( std::string materialName );
  IRDBAccessSvc* m_pRDBAccess;
  StoreGateSvc*  m_pDetStore;

  IRDBRecordset_ptr m_Etvv;
  IRDBRecordset_ptr m_Jdsh;
  IRDBRecordset_ptr m_Jfsh;
  IRDBRecordset_ptr m_Jtsh;
  
  IMessageSvc*    m_msgSvc;
};

} // namespace MuonGM

#endif
