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
#include <InDetGeoModelUtils/GeoModelXmlTool.h>
#include <RDBAccessSvc/IRDBAccessSvc.h>
#include <ReadoutGeometryBase/SiCommonItems.h>
#include <InDetGeoModelUtils/WaferTree.h>

#include <memory>


namespace InDetDD
{
  class PixelDetectorManager;
  class SiCommonItems;
}

class PLRDetectorTool : public GeoModelXmlTool
{
public: 
  PLRDetectorTool(const std::string &type, const std::string &name, const IInterface *parent);
  virtual ~PLRDetectorTool() = default;
  virtual StatusCode create() override final;
  virtual StatusCode clear() override final;
  virtual StatusCode registerCallback ATLAS_NOT_THREAD_SAFE () override final;
  virtual StatusCode align(IOVSVC_CALLBACK_ARGS_P(I,keys)) override final;
  
private:
  const InDetDD::PixelDetectorManager *m_detManager{};
  std::unique_ptr<InDetDD::SiCommonItems> m_commonItems{};
  WaferTree m_moduleTree;

  Gaudi::Property<std::string> m_detectorName{this, "DetectorName", "PLR", ""};
  Gaudi::Property<bool> m_alignable{this, "Alignable", false, ""};
  //This should be changed to an ITk-specific one in future, once available
  Gaudi::Property<std::string> m_alignmentFolderName{this, "AlignmentFolderName", "/Indet/Align", ""}; 
  ServiceHandle<IGeoModelSvc> m_geoModelSvc{this, "GeoModelSvc", "GeoModelSvc", ""};
  InDetDD::PixelDetectorManager * createManager(GeoPhysVol * theWorld);
  // Print out how many of each layer/eta/phi etc. have been set up. Not yet implemented here... 
  //may not be necessary in the end?
  void doNumerology();
};

#endif // PLRGEOMODELXML_PLRDETECTORTOOL_H
