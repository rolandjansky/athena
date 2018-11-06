/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_DETECTORTOOL_H
#define SCT_SLHC_GeoModel_SCT_DETECTORTOOL_H

#include "GeoModelUtilities/GeoModelTool.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>

class IGeoDbTagSvc;
class IRDBAccessSvc;
class IGeometryDBSvc;
class IInDetServMatBuilderTool;

namespace InDetDD {
  class SCT_DetectorManager;
}

namespace InDetDDSLHC {
  class SCT_GeoModelAthenaComps;
}

class SCT_SLHC_DetectorTool : public GeoModelTool {

public:

  // Standard Constructor
  SCT_SLHC_DetectorTool(const std::string& type, const std::string& name,
                        const IInterface* parent);

  // Standard Destructor
  virtual ~SCT_SLHC_DetectorTool() override final;

  virtual StatusCode create() override final;
  virtual StatusCode clear() override final;

private:
  StringProperty m_detectorName{this, "DetectorName", "SCT"};
  BooleanProperty m_alignable{this, "Alignable", false};
  BooleanProperty m_forceBuild{this, "ForceBuild", false};

  const InDetDD::SCT_DetectorManager* m_manager;
  
  InDetDDSLHC::SCT_GeoModelAthenaComps* m_athenaComps;

  ToolHandle< IInDetServMatBuilderTool > m_serviceBuilderTool;
  ServiceHandle< IGeoDbTagSvc > m_geoDbTagSvc;
  ServiceHandle< IRDBAccessSvc > m_rdbAccessSvc;
  ServiceHandle< IGeometryDBSvc > m_geometryDBSvc;
};

#endif // SCT_SLHC_GeoModel_SCT_DETECTORTOOL_H
