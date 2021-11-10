/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef StripGeoModelXml_STRIPDETECTORFACTORY_H
#define StripGeoModelXml_STRIPDETECTORFACTORY_H
//
//    Main routine to build the GeoModel geometry, and handle the GeometryManager and
//    DetectorManager.
//
#include <InDetGeoModelUtils/InDetDetectorFactoryBase.h>
#include <InDetGeoModelUtils/WaferTree.h>
#include <SCT_ReadoutGeometry/SCT_DetectorManager.h>

class GeoPhysVol;

namespace InDetDD
{

class SiCommonItems;

namespace ITk
{

class StripOptions;

class StripDetectorFactory : public DetectorFactoryBase
{
public:
  StripDetectorFactory(AthenaComps *athenaComps,
                       SiCommonItems *commonItems,
                       const StripOptions &options);

  // Creation of geometry:
  virtual void create(GeoPhysVol *world) override final;
  // Access to the results:
  virtual InDetDD::SCT_DetectorManager * getDetectorManager() const override final;

private:
  // Print out how many of each layer/eta/phi etc. have been set up.
  void doNumerology();
  // Get the xml from the database instead of a file. Returns gzipped xml as a string.
  std::string getBlob();
  // Determine which alignment folders are loaded to decide if we register old or new folders
  InDetDD::AlignFolderType getAlignFolderType() const;

  // Copy and assignments operations illegal and so are made private
  StripDetectorFactory(StripDetectorFactory &right);
  StripDetectorFactory & operator=(StripDetectorFactory &right);

  SCT_DetectorManager *m_detectorManager{};
  SiCommonItems *m_commonItems{};
  const StripOptions &m_options;
  WaferTree m_waferTree;
};

} // namespace ITk
} // namespace InDetDD

#endif
