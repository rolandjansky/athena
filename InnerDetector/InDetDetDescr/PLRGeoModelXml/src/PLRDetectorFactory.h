/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PLRGeoModelXml_PLRDETECTORFACTORY_H
#define PLRGeoModelXml_PLRDETECTORFACTORY_H
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

class AthenaComps;
class SiCommonItems;
class PLROptions;

class PLRDetectorFactory : public DetectorFactoryBase
{
public:
  PLRDetectorFactory(AthenaComps *athenaComps,
                     SiCommonItems *commonItems,
                     const PLROptions &options);

  // Creation of geometry:
  virtual void create(GeoPhysVol *world) override final;
  // Access to the results:
  virtual InDetDD::PixelDetectorManager * getDetectorManager() const override final;

private:
  // Print out how many of each layer/eta/phi etc. have been set up.
  void doNumerology();

  // Copy and assignments operations illegal and so are made private
  PLRDetectorFactory(PLRDetectorFactory &right);
  PLRDetectorFactory & operator=(PLRDetectorFactory &right);

  PixelDetectorManager *m_detectorManager{};
  SiCommonItems *m_commonItems{};
  const PLROptions &m_options;
  WaferTree m_moduleTree;
};

} // namespace InDetDD

#endif
