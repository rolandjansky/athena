/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PLRGeoModelXml_PLRDETECTORFACTORY_H
#define PLRGeoModelXml_PLRDETECTORFACTORY_H
//
//    Main routine to build the GeoModel geometry, and handle the GeometryManager and
//    DetectorManager.
// 
#include "InDetGeoModelUtils/InDetDetectorFactoryBase.h"
#include "PixelReadoutGeometry/PixelDetectorManager.h"
#include "InDetGeoModelUtils/WaferTree.h"

namespace InDetDD {class AthenaComps; class SiCommonItems;}
class GeoPhysVol;

namespace InDetDDSLHC {
  class PLROptions;

  class PLRDetectorFactory : public InDetDD::DetectorFactoryBase {
  public:
    PLRDetectorFactory(InDetDD::AthenaComps *athenaComps,
			 InDetDD::SiCommonItems *commonItems,
			 PLROptions &options);
    virtual ~PLRDetectorFactory();
    // Creation of geometry:
    virtual void create(GeoPhysVol *world);
    // Access to the results:
    virtual InDetDD::PixelDetectorManager * getDetectorManager() const;
    // Print out how many of each layer/eta/phi etc. have been set up.
    void doNumerology();

  private:
    // Copy and assignments operations illegal and so are made private
    PLRDetectorFactory(PLRDetectorFactory &right);
    PLRDetectorFactory & operator=(PLRDetectorFactory &right);

    InDetDD::PixelDetectorManager *m_detectorManager;
    InDetDD::AthenaComps *m_athenaComps;
    InDetDD::SiCommonItems *m_commonItems;
    PLROptions *m_options;
    WaferTree m_moduleTree;
  };
}
#endif
