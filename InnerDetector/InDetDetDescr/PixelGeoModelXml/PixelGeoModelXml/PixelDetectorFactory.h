/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelGeoModelXml_PIXELDETECTORFACTORY_H 
#define PixelGeoModelXml_PIXELDETECTORFACTORY_H 
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
  class PixelOptions;

  class PixelDetectorFactory : public InDetDD::DetectorFactoryBase {   
  public: 
    PixelDetectorFactory(InDetDD::AthenaComps *athenaComps, 
			 InDetDD::SiCommonItems *commonItems, 
			 PixelOptions &options); 
    virtual ~PixelDetectorFactory(); 
    // Creation of geometry:
    virtual void create(GeoPhysVol *world);   
    // Access to the results: 
    virtual InDetDD::PixelDetectorManager * getDetectorManager() const;
    // Print out how many of each layer/eta/phi etc. have been set up.
    void doNumerology();
    // Get the xml from the database instead of a file. Returns gzipped xml as a string.
    std::string getBlob();
    // Determine which alignment folders are loaded to decide if we register old or new folders
    //virtual InDetDD::AlignFolderType getAlignFolderType() const;

  private: 
    // Copy and assignments operations illegal and so are made private
    PixelDetectorFactory(PixelDetectorFactory &right); 
    PixelDetectorFactory & operator=(PixelDetectorFactory &right); 

    InDetDD::PixelDetectorManager *m_detectorManager;
    InDetDD::AthenaComps *m_athenaComps;
    InDetDD::SiCommonItems *m_commonItems;
    PixelOptions *m_options;
    WaferTree m_moduleTree;
  }; 
}  
#endif 
