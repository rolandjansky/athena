
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelGeoModelXml_PIXELDETECTORTOOL_H
#define PixelGeoModelXml_PIXELDETECTORTOOL_H
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
  class AthenaComps;
  class PixelDetectorManager;
  class SiCommonItems;
}

namespace ITk
{

class PixelDetectorTool : public GeoModelTool
{
public:
  PixelDetectorTool(const std::string &type, const std::string &name, const IInterface *parent);
  virtual ~PixelDetectorTool() = default;
  virtual StatusCode create() override final;
  virtual StatusCode clear() override final;
  virtual StatusCode registerCallback ATLAS_NOT_THREAD_SAFE () override final;
  virtual StatusCode align(IOVSVC_CALLBACK_ARGS_P(I,keys)) override final;

private:
  const InDetDD::PixelDetectorManager *m_manager{};
  std::unique_ptr<InDetDD::SiCommonItems> m_commonItems{};

  Gaudi::Property<std::string> m_detectorName{this, "DetectorName", "ITkPixel", ""};
  Gaudi::Property<bool> m_alignable{this, "Alignable", false, ""};
  Gaudi::Property<std::string> m_gmxFilename{this, "GmxFilename", "", ""};
  ServiceHandle<IGeoModelSvc> m_geoModelSvc{this, "GeoModelSvc", "GeoModelSvc", ""};
  ServiceHandle<IRDBAccessSvc> m_rdbAccessSvc{this, "RDBAccessSvc", "RDBAccessSvc", ""};
  ServiceHandle<IGeoDbTagSvc> m_geoDbTagSvc{this, "GeoDbTagSvc", "GeoDbTagSvc", ""};
};

} // namespace ITk

#endif // PixelGeoModelXml_PIXELDETECTORTOOL_H
