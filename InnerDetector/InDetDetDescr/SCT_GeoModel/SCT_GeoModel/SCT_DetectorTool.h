/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_DETECTORTOOL_H
#define SCT_GEOMODEL_SCT_DETECTORTOOL_H

#include "GeoModelUtilities/GeoModelTool.h"
#include "GaudiKernel/ServiceHandle.h"

class ISiLorentzAngleSvc;
class IGeoDbTagSvc;
class IRDBAccessSvc;
class IGeometryDBSvc;
class SCT_GeoModelAthenaComps;

#include <string>

namespace InDetDD {
  class SCT_DetectorManager;
}

class SCT_DetectorTool : public GeoModelTool {

public:

  // Standard Constructor
  SCT_DetectorTool( const std::string& type, const std::string& name, const IInterface* parent );

  // Standard Destructor
  virtual ~SCT_DetectorTool() override final;

  virtual StatusCode create() override final;
  virtual StatusCode clear() override final;

  // Register callback function on ConDB object
  virtual StatusCode registerCallback() override final;

  // Callback function itself
  virtual StatusCode align(IOVSVC_CALLBACK_ARGS);
	

private:
  std::string m_detectorName;
  bool m_initialLayout;
  bool m_alignable;
  bool m_cosmic;
  
  const InDetDD::SCT_DetectorManager * m_manager;
  
  SCT_GeoModelAthenaComps * m_athenaComps;

  ServiceHandle< IGeoDbTagSvc > m_geoDbTagSvc;
  ServiceHandle< IRDBAccessSvc > m_rdbAccessSvc;
  ServiceHandle< IGeometryDBSvc > m_geometryDBSvc;
  ServiceHandle< ISiLorentzAngleSvc > m_lorentzAngleSvc;

};

#endif // SCT_GEOMODEL_SCT_DETECTORTOOL_H
