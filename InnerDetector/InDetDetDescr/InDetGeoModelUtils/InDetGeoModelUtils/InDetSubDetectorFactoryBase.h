/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetGeoModelUtils_SubDetectorFactoryBase_H
#define InDetGeoModelUtils_SubDetectorFactoryBase_H

#include "AthenaKernel/MsgStreamMember.h"
#include "InDetGeoModelUtils/InDetDDAthenaComps.h"

class StoreGateSvc;
class IGeoDbTagSvc;
class IRDBAccessSvc;
class InDetMaterialManager;

namespace InDetDD {

// This is the same as InDet::DetectorFactoryBase but without the
// inheretance of GeoVDetectorFactory and with the addition of 
// access to the material manager.

class SubDetectorFactoryBase
{ 

public:
  SubDetectorFactoryBase(const InDetDD::AthenaComps * athenaComps)
    : m_athenaComps(athenaComps),
      m_materialManager(0)
  {}

  SubDetectorFactoryBase(const InDetDD::AthenaComps * athenaComps,
			 InDetMaterialManager * matManager)
    : m_athenaComps(athenaComps),
      m_materialManager(matManager)
  {}

  StoreGateSvc * detStore() const {return m_athenaComps->detStore();}

  IGeoDbTagSvc * geoDbTagSvc() const {return m_athenaComps->geoDbTagSvc();}

  IRDBAccessSvc * rdbAccessSvc() const {return m_athenaComps->rdbAccessSvc();}
  
  IGeometryDBSvc * geomDB() const {return m_athenaComps->geomDB();}

  InDetMaterialManager * materialManager() const {return m_materialManager;}

 //Declaring the Message method for further use
  MsgStream& msg (MSG::Level lvl) const { return m_athenaComps->msg(lvl); }

  //Declaring the Method providing Verbosity Level
  bool msgLvl (MSG::Level lvl) const { return m_athenaComps->msgLvl(lvl); }

  const InDetDD::AthenaComps *  getAthenaComps() {return m_athenaComps;}
  
private:
  const InDetDD::AthenaComps *  m_athenaComps;
  
protected:
  InDetMaterialManager * m_materialManager;
};

} // end namespace

#endif // InDetGeoModelUtils_SubDetectorFactoryBase_H

