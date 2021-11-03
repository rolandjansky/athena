/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelGeoModelXml_PIXELDETECTORFACTORY_H
#define PixelGeoModelXml_PIXELDETECTORFACTORY_H
//
//    Main routine to build the GeoModel geometry, and handle the GeometryManager and
//    DetectorManager.
//
#include <InDetGeoModelUtils/InDetDetectorFactoryBase.h>
#include <InDetGeoModelUtils/WaferTree.h>
#include <PixelReadoutGeometry/PixelDetectorManager.h>

class GeoPhysVol;

namespace InDetDD
{

class SiCommonItems;

namespace ITk
{

class PixelOptions;

class PixelDetectorFactory : public DetectorFactoryBase
{
public:
  PixelDetectorFactory(AthenaComps *athenaComps,
                       SiCommonItems *commonItems,
                       const PixelOptions &options);

  // Creation of geometry:
  virtual void create(GeoPhysVol *world) override final;
  // Access to the results:
  virtual const InDetDD::PixelDetectorManager * getDetectorManager() const override final;

private:
  // Print out how many of each layer/eta/phi etc. have been set up.
  void doNumerology();
  // Get the xml from the database instead of a file. Returns gzipped xml as a string.
  std::string getBlob();
  // Determine which alignment folders are loaded to decide if we register old or new folders
  // InDetDD::AlignFolderType getAlignFolderType() const;

  // Copy and assignments operations illegal and so are made private
  PixelDetectorFactory(PixelDetectorFactory &right);
  PixelDetectorFactory & operator=(PixelDetectorFactory &right);

  PixelDetectorManager *m_detectorManager{};
  SiCommonItems *m_commonItems{};
  const PixelOptions &m_options;
  WaferTree m_moduleTree;
};

} // namespace ITk
} // namespace InDetDD

#endif
