/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FeetToroidBuilderRDB_H
#define FeetToroidBuilderRDB_H

#include "MuonGeoModel/FeetToroidBuilderV.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

class StoreGateSvc;
class IRDBAccessSvc;
class GeoPhysVol;
class IMessageSvc;

#include <string>

namespace MuonGM {
// General Constants
  const int maxDim = 9;
    const int nFeet = 9;
      const int nVertex = 9;
        const double ANGIPL = 0.3927;
	
class FeetToroidBuilderRDB : public FeetToroidBuilderV 
{ 
 public:

  //  ToroidBuilderRDB(StoreGateSvc *pDetStore);
  FeetToroidBuilderRDB(StoreGateSvc  *pDetStore,
		   IRDBAccessSvc *pRDBAccess, std::string geoTag, std::string geoNode);
  virtual ~FeetToroidBuilderRDB(){};
  
  // Builds the Feet
  void buildStandardFeet    ( GeoPhysVol* container );
  void buildExtremityFeet   ( GeoPhysVol* container );
  void buildFeetGirders     ( GeoPhysVol* container );
  void buildFeetRailSupports( GeoPhysVol* container );
  void buildFeetVoussoirs   ( GeoPhysVol* container );

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
