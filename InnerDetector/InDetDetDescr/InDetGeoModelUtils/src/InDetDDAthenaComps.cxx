/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#include "InDetGeoModelUtils/InDetDDAthenaComps.h"

namespace InDetDD {
  AthenaComps::AthenaComps(const std::string& msgStreamName)
    : m_msg(msgStreamName),
    m_detStore(0),
    m_geoDbTagSvc(0),
    m_rdbAccessSvc(0),
    m_geometryDBSvc(0)
  {}

  void
  AthenaComps::setDetStore(StoreGateSvc* detStore) {
    m_detStore = detStore;
  }

  void
  AthenaComps::setGeoDbTagSvc(IGeoDbTagSvc* geoDbTagSvc) {
    m_geoDbTagSvc = geoDbTagSvc;
  }

  void
  AthenaComps::setRDBAccessSvc(IRDBAccessSvc* rdbAccessSvc) {
    m_rdbAccessSvc = rdbAccessSvc;
  }

  void
  AthenaComps::setGeometryDBSvc(IGeometryDBSvc* geometryDBSvc) {
    m_geometryDBSvc = geometryDBSvc;
  }
}
