/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef StripGeoModelXml_STRIPDETECTORTOOL_H
#define StripGeoModelXml_STRIPDETECTORTOOL_H
//
// Create an Athena Tool; handle Athena services and Tools needed for 
// building the SCT geometry.
// This is the entry to the StripGeoModelXml package.
//
#include <GaudiKernel/ServiceHandle.h>
#include <GeoModelInterfaces/IGeoDbTagSvc.h>
#include <GeoModelInterfaces/IGeoModelSvc.h>
#include <InDetGeoModelUtils/GeoModelXmlTool.h>
#include <RDBAccessSvc/IRDBAccessSvc.h>
#include <ReadoutGeometryBase/SiCommonItems.h>
#include <InDetGeoModelUtils/WaferTree.h>

#include <memory>

class GeoPhysVol;

namespace InDetDD
{
  class SCT_DetectorManager;
  class SiCommonItems;
}

namespace ITk
{

class StripDetectorTool : public GeoModelXmlTool
{
public:
  StripDetectorTool(const std::string &type, const std::string &name, const IInterface *parent);
  virtual ~StripDetectorTool() = default;
  virtual StatusCode create() override final;
  virtual StatusCode clear() override final;
  virtual StatusCode registerCallback() override final;
  virtual StatusCode align(IOVSVC_CALLBACK_ARGS_P(I,keys)) override final;

private:
  const InDetDD::SCT_DetectorManager *m_detManager{};
  std::unique_ptr<InDetDD::SiCommonItems> m_commonItems{};
  WaferTree m_waferTree;

  Gaudi::Property<bool> m_alignable{this, "Alignable", false, ""};
    //This should be changed to an ITk-specific one in future, once available
  Gaudi::Property<std::string> m_alignmentFolderName{this, "AlignmentFolderName", "/Indet/Align", ""}; 
  ServiceHandle<IGeoModelSvc> m_geoModelSvc{this, "GeoModelSvc", "GeoModelSvc", ""};

  InDetDD::SCT_DetectorManager * createManager(GeoPhysVol * theWorld);
  // Print out how many of each layer/eta/phi etc. have been set up.
  void doNumerology(InDetDD::SCT_DetectorManager * manager);
};

} // namespace ITk

#endif // StripGeoModelXml_STRIPDETECTORTOOL_H
