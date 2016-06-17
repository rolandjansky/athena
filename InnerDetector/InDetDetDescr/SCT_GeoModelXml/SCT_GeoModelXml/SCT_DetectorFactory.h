/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GeoModelXml_SCT_DETECTORFACTORY_H 
#define SCT_GeoModelXml_SCT_DETECTORFACTORY_H 
//
//    Main routine to build the GeoModel geometry, and handle the GeometryManager and 
//    DetectorManager.
// 
#include "InDetGeoModelUtils/InDetDetectorFactoryBase.h" 
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "SCT_GeoModelXml/WaferTree.h"

namespace InDetDD {class AthenaComps; class SiCommonItems;}
class GeoPhysVol;

namespace InDetDDSLHC {
class SCT_Options;

class SCT_DetectorFactory : public InDetDD::DetectorFactoryBase {   
public: 
    SCT_DetectorFactory(InDetDD::AthenaComps *athenaComps, 
                        InDetDD::SiCommonItems *commonItems, 
                        SCT_Options &options); 
    virtual ~SCT_DetectorFactory(); 
    // Creation of geometry:
    virtual void create(GeoPhysVol *world);   
    // Access to the results: 
    virtual InDetDD::SCT_DetectorManager * getDetectorManager() const;
    // Print out how many of each layer/eta/phi etc. have been set up.
    void doNumerology();
    // Get the xml from the database instead of a file. Returns gzipped xml as a string.
    std::string getBlob();

private: 
    // Copy and assignments operations illegal and so are made private
    SCT_DetectorFactory(SCT_DetectorFactory &right); 
    SCT_DetectorFactory & operator=(SCT_DetectorFactory &right); 

    InDetDD::SCT_DetectorManager *m_detectorManager;
    InDetDD::AthenaComps *m_athenaComps;
    InDetDD::SiCommonItems *m_commonItems;
    SCT_Options *m_options;
    WaferTree m_waferTree;
}; 
}  
#endif 
