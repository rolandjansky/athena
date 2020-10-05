/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELGEOMODEL_PIXELDETECTORTOOL_H
#define PIXELGEOMODEL_PIXELDETECTORTOOL_H

#include "GeoModelUtilities/GeoModelTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "GeoModelInterfaces/IGeoSubDetTool.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "PixelGeoModel/IBLParameterSvc.h"
#include "InDetGeoModelUtils/IInDetServMatBuilderTool.h"

#include "CxxUtils/checker_macros.h"

class PixelGeoModelAthenaComps;

namespace InDetDD {
  class PixelDetectorManager;
}

class PixelDetectorTool final : public GeoModelTool {

 public:
  // Standard Constructor
  PixelDetectorTool( const std::string& type, const std::string& name, const IInterface* parent );
  // Standard Destructor
  virtual ~PixelDetectorTool() override final;

  virtual StatusCode initialize() override final;

  virtual StatusCode create() override final;
  virtual StatusCode clear() override final;

  // Register callback function on CondDB object
  virtual StatusCode registerCallback ATLAS_NOT_THREAD_SAFE () override final;

  // Callback function itself
  virtual StatusCode align(IOVSVC_CALLBACK_ARGS) override;

private:
  //
  bool m_services{true};
  bool m_servicesOnLadder{true}; //JBdV
  std::string m_detectorName{"PixelDetector"};
  ServiceHandle<IBLParameterSvc> m_IBLParameterSvc{this,"IBLParameterSvc","IBLParameterSvc",""};
  bool m_dc1Geometry{false};
  bool m_alignable{true};
  bool m_tweakIBLDist{true}; // whether to tweak L3 transforms with IBLDist
  bool m_initialLayout{false};
  bool m_devVersion{false};
  bool m_buildDBM{false};
  bool m_useDynamicAlignFolders{false};
  ToolHandle< IGeoSubDetTool > m_bcmTool{this,"BCM_Tool","",""};
  ToolHandle< IGeoSubDetTool > m_blmTool{this,"BLM_Tool","",""};
  PublicToolHandle< IInDetServMatBuilderTool > m_serviceBuilderTool{this,"ServiceBuilderTool","",""};
  ServiceHandle< IGeoDbTagSvc > m_geoDbTagSvc{this,"GeoDbTagSvc","GeoDbTagSvc",""};
  ServiceHandle< IRDBAccessSvc > m_rdbAccessSvc{this,"RDBAccessSvc","RDBAccessSvc",""};
  ServiceHandle< IGeometryDBSvc > m_geometryDBSvc{this,"GeometryDBSvc","InDetGeometryDBSvc",""};
  const InDetDD::PixelDetectorManager * m_manager{};
  std::string m_overrideVersionName;

  PixelGeoModelAthenaComps * m_athenaComps{};

};

#endif // PIXELGEOMODEL_PIXELDETECTORTOOL_H
