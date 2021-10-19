/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef StripGeoModelXml_STRIPDETECTORTOOL_H
#define StripGeoModelXml_STRIPDETECTORTOOL_H
//
// Create an Athena Tool; handle Athena services and Tools needed for 
// building the SCT geometry. Then create the geometry using the SCT_DetectorFactory.
// This is the entry to the StripGeoModelXml package.
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
  class SCT_DetectorManager;
  class AthenaComps;
  class SiCommonItems;
}

namespace ITk
{

class StripDetectorTool : public GeoModelTool
{
public:
  StripDetectorTool(const std::string &type, const std::string &name, const IInterface *parent);
  virtual ~StripDetectorTool() = default;
  virtual StatusCode create() override final;
  virtual StatusCode clear() override final;
  virtual StatusCode registerCallback() override final;
  virtual StatusCode align(IOVSVC_CALLBACK_ARGS_P(I,keys)) override final;

private:
  const InDetDD::SCT_DetectorManager *m_manager{};
  std::unique_ptr<InDetDD::SiCommonItems> m_commonItems{};

  Gaudi::Property<std::string> m_detectorName{this, "DetectorName", "ITkStrip", ""};
  Gaudi::Property<bool> m_alignable{this, "Alignable", false, ""};
  Gaudi::Property<std::string> m_gmxFilename{this, "GmxFilename", "", ""};
  ServiceHandle<IGeoModelSvc> m_geoModelSvc{this, "GeoModelSvc", "GeoModelSvc", ""};
  ServiceHandle<IRDBAccessSvc> m_rdbAccessSvc{this, "RDBAccessSvc", "RDBAccessSvc", ""};
  ServiceHandle<IGeoDbTagSvc> m_geoDbTagSvc{this, "GeoDbTagSvc", "GeoDbTagSvc", ""};
};

} // namespace ITk

#endif // StripGeoModelXml_STRIPDETECTORTOOL_H
