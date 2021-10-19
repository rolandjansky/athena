/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PLRGeoModelXml_PLRDETECTORTOOL_H
#define PLRGeoModelXml_PLRDETECTORTOOL_H
//
// Create an Athena Tool; handle Athena services and Tools needed for
// building the Pixel geometry. Then create the geometry using the PixelDetectorFactory.
// This is the entry to the PixelGeoModelXml package.
//
#include <GaudiKernel/ServiceHandle.h>
#include <GeoModelInterfaces/IGeoDbTagSvc.h>
#include <GeoModelInterfaces/IGeoModelSvc.h>
#include <GeoModelUtilities/GeoModelTool.h>
#include <RDBAccessSvc/IRDBAccessSvc.h>
#include <ReadoutGeometryBase/SiCommonItems.h>

#include <memory>


namespace InDetDD
{
  class PixelDetectorManager;
  class AthenaComps;
  class SiCommonItems;
}

class PLRDetectorTool : public GeoModelTool
{
public: 
  PLRDetectorTool(const std::string &type, const std::string &name, const IInterface *parent);
  virtual ~PLRDetectorTool() = default;
  virtual StatusCode create() override final;
  virtual StatusCode clear() override final;
  virtual StatusCode registerCallback() override final;
  virtual StatusCode align(IOVSVC_CALLBACK_ARGS_P(I,keys)) override final;
  
private:
  const InDetDD::PixelDetectorManager *m_manager{};
  std::unique_ptr<InDetDD::SiCommonItems> m_commonItems{};

  Gaudi::Property<std::string> m_detectorName{this, "DetectorName", "PLR", ""};
  Gaudi::Property<bool> m_alignable{this, "Alignable", false, ""};
  Gaudi::Property<std::string> m_gmxFilename{this, "GmxFilename", "", ""};
  ServiceHandle<IGeoModelSvc> m_geoModelSvc{this, "GeoModelSvc", "GeoModelSvc", ""};
  ServiceHandle<IRDBAccessSvc> m_rdbAccessSvc{this, "RDBAccessSvc", "RDBAccessSvc", ""};
  ServiceHandle<IGeoDbTagSvc> m_geoDbTagSvc{this, "GeoDbTagSvc", "GeoDbTagSvc", ""};
};

#endif // PLRGEOMODELXML_PLRDETECTORTOOL_H
