/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetGeoModelUtils_InDetDDAthenaComps_H
#define InDetGeoModelUtils_InDetDDAthenaComps_H

// Message Stream Member
#include "AthenaKernel/MsgStreamMember.h"
#include "CxxUtils/checker_macros.h"
class  StoreGateSvc;
class  IGeoDbTagSvc;
class  IRDBAccessSvc;
class  IGeometryDBSvc; 

#include <string>
namespace InDetDD {

/// Class to hold various Athena components.
class AthenaComps {

public:

  AthenaComps(const std::string & msgStreamName);

  //Declaring the Message method for further use
  MsgStream& msg (MSG::Level lvl) { return m_msg << lvl; }

  //Declaring the Method providing Verbosity Level
  bool msgLvl (MSG::Level lvl) { return m_msg.get().level() <= lvl; }

  void setDetStore(StoreGateSvc *);
  void setGeoDbTagSvc(IGeoDbTagSvc *);
  void setRDBAccessSvc(IRDBAccessSvc *);
  void setGeometryDBSvc(IGeometryDBSvc *);

  const IGeoDbTagSvc * geoDbTagSvc() const;
  const IGeometryDBSvc * geomDB() const;
  
  StoreGateSvc * detStore();
  IGeoDbTagSvc * geoDbTagSvc();
  IRDBAccessSvc * rdbAccessSvc();
  IGeometryDBSvc * geomDB();
  
private:
  //Declaring private message stream member.
  Athena::MsgStreamMember m_msg;
  
  StoreGateSvc * m_detStore;
  IGeoDbTagSvc * m_geoDbTagSvc;
  IRDBAccessSvc * m_rdbAccessSvc;
  IGeometryDBSvc * m_geometryDBSvc;

};

inline StoreGateSvc * AthenaComps::detStore()
{
  return m_detStore;
}

inline const IGeoDbTagSvc * AthenaComps::geoDbTagSvc() const
{
  return m_geoDbTagSvc;
}

inline const IGeometryDBSvc * AthenaComps::geomDB() const
{
  return m_geometryDBSvc;
}

inline IGeoDbTagSvc * AthenaComps::geoDbTagSvc()
{
  return m_geoDbTagSvc;
}

inline IRDBAccessSvc * AthenaComps::rdbAccessSvc()
{
  return m_rdbAccessSvc;
}

inline IGeometryDBSvc * AthenaComps::geomDB()
{
  return m_geometryDBSvc;
}

} // endnamespace

#endif // InDetGeoModelUtils_InDetDDAthenaComps_H

