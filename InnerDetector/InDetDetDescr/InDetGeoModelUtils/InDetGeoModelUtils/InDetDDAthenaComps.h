/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetGeoModelUtils_InDetDDAthenaComps_H
#define InDetGeoModelUtils_InDetDDAthenaComps_H

#include "AthenaBaseComps/AthMessaging.h"
#include "CxxUtils/checker_macros.h"
class  StoreGateSvc;
class  IGeoDbTagSvc;
class  IRDBAccessSvc;
class  IGeometryDBSvc; 

#include <string>
namespace InDetDD {

/// Class to hold various Athena components.
class AthenaComps
  : public AthMessaging
{
public:

  AthenaComps(const std::string & msgStreamName);

  void setDetStore(StoreGateSvc *);
  void setGeoDbTagSvc(IGeoDbTagSvc *);
  void setRDBAccessSvc(IRDBAccessSvc *);
  void setGeometryDBSvc(IGeometryDBSvc *);

  const StoreGateSvc * detStore() const;
  const IGeoDbTagSvc * geoDbTagSvc() const;
  const IGeometryDBSvc * geomDB() const;
  
  StoreGateSvc * detStore();
  IGeoDbTagSvc * geoDbTagSvc();
  IRDBAccessSvc * rdbAccessSvc();
  IGeometryDBSvc * geomDB();
  
private:
  StoreGateSvc * m_detStore;
  IGeoDbTagSvc * m_geoDbTagSvc;
  IRDBAccessSvc * m_rdbAccessSvc;
  IGeometryDBSvc * m_geometryDBSvc;

};

inline StoreGateSvc * AthenaComps::detStore()
{
  return m_detStore;
}

inline const StoreGateSvc * AthenaComps::detStore() const
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

