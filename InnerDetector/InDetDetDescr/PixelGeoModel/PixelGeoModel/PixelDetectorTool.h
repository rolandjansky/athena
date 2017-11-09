/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELGEOMODEL_PIXELDETECTORTOOL_H
#define PIXELGEOMODEL_PIXELDETECTORTOOL_H

#include "GeoModelUtilities/GeoModelTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

class PixelGeoModelAthenaComps;
class IGeoDbTagSvc;
class IRDBAccessSvc;
class IGeometryDBSvc;
class ISiLorentzAngleSvc;
class IGeoSubDetTool;
class IInDetServMatBuilderTool;
class IBLParameterSvc;


namespace InDetDD {
  class PixelDetectorManager;
}

class PixelDetectorTool : public GeoModelTool {

 public: 
  // Standard Constructor
  PixelDetectorTool( const std::string& type, const std::string& name, const IInterface* parent );
  // Standard Destructor
  virtual ~PixelDetectorTool() override final;

  virtual StatusCode create() override final;
  virtual StatusCode clear() override final;

  // Register callback function on ConDB object
  virtual StatusCode registerCallback() override final;

  // Callback function itself
  virtual StatusCode align(IOVSVC_CALLBACK_ARGS) override;

private:
  //
  bool m_services;
  bool m_servicesOnLadder; //JBdV
  std::string m_detectorName;
  ServiceHandle<IBLParameterSvc>                        m_IBLParameterSvc; 
  bool m_dc1Geometry;
  bool m_alignable;
  bool m_tweakIBLDist; // whether to tweak L3 transforms with IBLDist
  bool m_initialLayout;
  bool m_devVersion;
  bool m_buildDBM;
  ToolHandle< IGeoSubDetTool > m_bcmTool;
  ToolHandle< IGeoSubDetTool > m_blmTool;
  ToolHandle< IInDetServMatBuilderTool > m_serviceBuilderTool;
  ServiceHandle< IGeoDbTagSvc > m_geoDbTagSvc;
  ServiceHandle< IRDBAccessSvc > m_rdbAccessSvc;
  ServiceHandle< IGeometryDBSvc > m_geometryDBSvc;
  ServiceHandle< ISiLorentzAngleSvc > m_lorentzAngleSvc;
  const InDetDD::PixelDetectorManager * m_manager;
  std::string m_overrideVersionName;

  PixelGeoModelAthenaComps * m_athenaComps;

};

#endif // PIXELGEOMODEL_PIXELDETECTORTOOL_H
