/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetGeoModelUtils_DetectorFactoryBase_H
#define InDetGeoModelUtils_DetectorFactoryBase_H

#include "AthenaKernel/MsgStreamMember.h"
#include "GeoModelKernel/GeoVDetectorFactory.h" 
#include "InDetGeoModelUtils/InDetDDAthenaComps.h"

class StoreGateSvc;
class IGeoDbTagSvc;
class IRDBAccessSvc;

namespace InDetDD {

class DetectorFactoryBase : public GeoVDetectorFactory  
{ 

public:
  DetectorFactoryBase(const InDetDD::AthenaComps * athenaComps)
    : m_athenaComps(athenaComps)
  {}

  StoreGateSvc * detStore() const {return m_athenaComps->detStore();}

  IGeoDbTagSvc * geoDbTagSvc() const {return m_athenaComps->geoDbTagSvc();}

  IRDBAccessSvc * rdbAccessSvc() const {return m_athenaComps->rdbAccessSvc();}
  
  IGeometryDBSvc * geomDB() const {return m_athenaComps->geomDB();}

 //Declaring the Message method for further use
  MsgStream& msg (MSG::Level lvl) const { return m_athenaComps->msg(lvl); }

  //Declaring the Method providing Verbosity Level
  bool msgLvl (MSG::Level lvl) const { return m_athenaComps->msgLvl(lvl); }

  const InDetDD::AthenaComps *  getAthenaComps() {return m_athenaComps;}

private:
  
  const InDetDD::AthenaComps *  m_athenaComps;

};

} // end namespace

#endif // InDetGeoModelUtils_DetectorFactoryBase_H

