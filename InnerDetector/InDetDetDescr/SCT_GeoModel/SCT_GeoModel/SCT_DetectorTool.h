/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_DETECTORTOOL_H
#define SCT_GEOMODEL_SCT_DETECTORTOOL_H

#include "GeoModelUtilities/GeoModelTool.h"
#include "SCT_GeoModel/SCT_GeoModelAthenaComps.h" 

#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include "GaudiKernel/ServiceHandle.h"

#include <string>

namespace InDetDD {
  class SCT_DetectorManager;
}

class SCT_DetectorTool : public GeoModelTool {

public:
  // Standard Constructor
  SCT_DetectorTool(const std::string& type, const std::string& name, const IInterface* parent);

  virtual StatusCode create() override final;
  virtual StatusCode clear() override final;

  // Register callback function on ConDB object
  virtual StatusCode registerCallback() override final;

  // Callback function itself
  virtual StatusCode align(IOVSVC_CALLBACK_ARGS) override;

private:
  StringProperty m_detectorName{this, "DetectorName", "SCT"};
  BooleanProperty m_alignable{this, "Alignable", true};
  BooleanProperty m_useDynamicAlignFolders{this, "useDynamicAlignFolders", false};
  bool m_cosmic;
  
  const InDetDD::SCT_DetectorManager* m_manager;
  
  SCT_GeoModelAthenaComps m_athenaComps;

  ServiceHandle< IGeoDbTagSvc > m_geoDbTagSvc;
  ServiceHandle< IRDBAccessSvc > m_rdbAccessSvc;
  ServiceHandle< IGeometryDBSvc > m_geometryDBSvc;

  StringProperty m_run1Folder{this, "Run1Folder", "/Indet/Align"};
  StringProperty m_run2L1Folder{this, "Run2L1Folder", "/Indet/AlignL1/ID"};
  StringProperty m_run2L2Folder{this, "Run2L2Folder", "/Indet/AlignL2/SCT"};
  StringProperty m_run2L3Folder{this, "Run2L3Folder", "/Indet/AlignL3"};
};

#endif // SCT_GEOMODEL_SCT_DETECTORTOOL_H
