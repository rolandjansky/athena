/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetGeoModelUtils_SubDetectorFactoryBase_H
#define InDetGeoModelUtils_SubDetectorFactoryBase_H

#include "AthenaKernel/MsgStreamMember.h"
#include "CxxUtils/checker_macros.h"
#include "InDetGeoModelUtils/InDetDDAthenaComps.h"

#include <memory>

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

  StoreGateSvc * detStore ATLAS_NOT_THREAD_SAFE () const {return m_athenaComps->detStore();} // const method returns non-const pointer

  const IGeoDbTagSvc * geoDbTagSvc() const {return m_athenaComps->geoDbTagSvc();}

  IRDBAccessSvc * rdbAccessSvc ATLAS_NOT_THREAD_SAFE () const {return m_athenaComps->rdbAccessSvc();} // const method returns non-const pointer
  
  const IGeometryDBSvc * geomDB() const {return m_athenaComps->geomDB();}

  InDetMaterialManager * materialManager ATLAS_NOT_THREAD_SAFE () const {return m_materialManager;} // const method returns non-const pointer

 //Declaring the Message method for further use
  MsgStream& msg (MSG::Level lvl) const { return m_athenaComps->msg(lvl); }

  //Declaring the Method providing Verbosity Level
  bool msgLvl (MSG::Level lvl) const { return m_athenaComps->msgLvl(lvl); }

  const InDetDD::AthenaComps *  getAthenaComps() {return m_athenaComps;}
  
private:
  const InDetDD::AthenaComps *  m_athenaComps;
  
protected:
  InDetMaterialManager * m_materialManager;
  // Use this std::unique_ptr when this class owns InDetMaterialManager
  std::unique_ptr<InDetMaterialManager> m_materialManagerUnique;
};

} // end namespace

#endif // InDetGeoModelUtils_SubDetectorFactoryBase_H

