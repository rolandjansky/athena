/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PLRGeoModelXml_PLRDETECTORTOOL_H
#define PLRGeoModelXml_PLRDETECTORTOOL_H
//
//    Create an Athena Tool; handle Athena services and Tools needed for
//    building the Pixel geometry. Then create the geometry using the PixelDetectorFactory.
//    This is the entry to the PixelGeoModelXml package.
//
#include "GeoModelUtilities/GeoModelTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>

class IGeoModelSvc;
class IRDBAccessSvc;
class IGeoDbTagSvc;

namespace InDetDD {
  class PixelDetectorManager;
  class AthenaComps;
  class SiCommonItems;
}

class PLRDetectorTool : public GeoModelTool {
 public: 
  PLRDetectorTool(const std::string &type, const std::string &name, const IInterface *parent);
  virtual ~PLRDetectorTool();
  StatusCode create() override final;
  StatusCode clear() override final;
  StatusCode registerCallback() override final;
  virtual StatusCode align(IOVSVC_CALLBACK_ARGS_P(I,keys)) override;
  
 private:
  std::string m_detectorName;
  bool m_alignable;
  std::string m_gmxFilename;
  const InDetDD::PixelDetectorManager *m_manager;
  InDetDD::AthenaComps *m_athenaComps;
  InDetDD::SiCommonItems *m_commonItems;
  ServiceHandle<IGeoModelSvc> m_geoModelSvc;
  ServiceHandle<IRDBAccessSvc> m_rdbAccessSvc;
  ServiceHandle< IGeoDbTagSvc > m_geoDbTagSvc;
};

#endif // PLRGEOMODELXML_PLRDETECTORTOOL_H