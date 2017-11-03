/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_INDETSERVMATTOOL_H
#define INDETSERVMATGEOMODEL_INDETSERVMATTOOL_H

#include "GeoModelUtilities/GeoModelTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include <string>

class InDetServMatAthenaComps;
class IGeoDbTagSvc;
class IRDBAccessSvc;
class IGeometryDBSvc;
class IInDetServMatBuilderTool;

namespace InDetDD {
  class InDetServMatManager;
}

class InDetServMatTool : public GeoModelTool {

 public: 
  // Standard Constructor
  InDetServMatTool( const std::string& type, const std::string& name, const IInterface* parent );
  // Standard Destructor
  virtual ~InDetServMatTool() override final;
  
  virtual StatusCode create() override final;
  virtual StatusCode clear() override final;

 private:
  ServiceHandle< IGeoDbTagSvc > m_geoDbTagSvc;
  ServiceHandle< IRDBAccessSvc > m_rdbAccessSvc;
  ServiceHandle< IGeometryDBSvc > m_geometryDBSvc;
  ToolHandle<IInDetServMatBuilderTool> m_builderTool;

  bool m_devVersion;
  bool m_forFrozenShowers;
  std::string m_overrideVersionName;
  const InDetDD::InDetServMatManager* m_manager;
  InDetServMatAthenaComps * m_athenaComps;
};

#endif // INDETSERVMATGEOMODEL_INDETSERVMATTOOL_H

