/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef StripGeoModelXml_STRIPDETECTORFACTORY_H 
#define StripGeoModelXml_STRIPDETECTORFACTORY_H 
//
//    Main routine to build the GeoModel geometry, and handle the GeometryManager and 
//    DetectorManager.
// 
#include "InDetGeoModelUtils/InDetDetectorFactoryBase.h" 
#include "SCT_ReadoutGeometry/SCT_DetectorManager.h"
#include "StripGeoModelXml/WaferTree.h"

namespace InDetDD {class AthenaComps; class SiCommonItems;}
class GeoPhysVol;

namespace InDetDDSLHC {
class StripOptions;

class StripDetectorFactory : public InDetDD::DetectorFactoryBase {   
public: 
    StripDetectorFactory(InDetDD::AthenaComps *athenaComps, 
                        InDetDD::SiCommonItems *commonItems, 
                        StripOptions &options); 
    virtual ~StripDetectorFactory(); 
    // Creation of geometry:
    virtual void create(GeoPhysVol *world);   
    // Access to the results: 
    virtual InDetDD::SCT_DetectorManager * getDetectorManager() const;
    // Print out how many of each layer/eta/phi etc. have been set up.
    void doNumerology();
    // Get the xml from the database instead of a file. Returns gzipped xml as a string.
    std::string getBlob();
    // Determine which alignment folders are loaded to decide if we register old or new folders
    virtual InDetDD::AlignFolderType getAlignFolderType() const;

private: 
    // Copy and assignments operations illegal and so are made private
    StripDetectorFactory(StripDetectorFactory &right); 
    StripDetectorFactory & operator=(StripDetectorFactory &right); 

    InDetDD::SCT_DetectorManager *m_detectorManager;
    InDetDD::AthenaComps *m_athenaComps;
    InDetDD::SiCommonItems *m_commonItems;
    StripOptions *m_options;
    WaferTree m_waferTree;
}; 
}  
#endif 
