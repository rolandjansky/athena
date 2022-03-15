
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelGeoModelXml_PIXELDETECTORTOOL_H
#define PixelGeoModelXml_PIXELDETECTORTOOL_H
//
// Create an Athena Tool; handle Athena services and Tools needed for
// building the Pixel geometry. Then create the geometry using the PixelDetectorFactory.
// This is the entry to the PixelGeoModelXml package.
//
#include <InDetGeoModelUtils/GeoModelXmlTool.h>
#include <InDetGeoModelUtils/WaferTree.h>
#include <ReadoutGeometryBase/SiCommonItems.h>

#include <memory>

class GeoPhysVol;

namespace InDetDD
{
  class PixelDetectorManager;
}

namespace ITk
{

class PixelDetectorTool : public GeoModelXmlTool
{
public:
  PixelDetectorTool(const std::string &type, const std::string &name, const IInterface *parent);
  virtual ~PixelDetectorTool() = default;
  virtual StatusCode create() override final;
  virtual StatusCode clear() override final;
  virtual StatusCode registerCallback ATLAS_NOT_THREAD_SAFE () override final;
  virtual StatusCode align(IOVSVC_CALLBACK_ARGS_P(I,keys)) override final;

private:
  const InDetDD::PixelDetectorManager *m_detManager{};
  std::unique_ptr<InDetDD::SiCommonItems> m_commonItems{};
  WaferTree m_moduleTree;

  Gaudi::Property<bool> m_alignable{this, "Alignable", false, ""};
  // This should be changed to an ITk-specific one in future, once available
  Gaudi::Property<std::string> m_alignmentFolderName{this, "AlignmentFolderName", "/Indet/Align", ""};
  // Print out how many of each layer/eta/phi etc. have been set up.
  void doNumerology(InDetDD::PixelDetectorManager *manager);
};

} // namespace ITk

#endif // PixelGeoModelXml_PIXELDETECTORTOOL_H
