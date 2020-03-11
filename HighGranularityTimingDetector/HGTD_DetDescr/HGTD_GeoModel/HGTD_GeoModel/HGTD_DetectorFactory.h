/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HGTD_GEOMODEL_HGTD_DETECTORFACTORY_H
#define HGTD_GEOMODEL_HGTD_DETECTORFACTORY_H
//
//    Main routine to build the GeoModel geometry, and handle the GeometryManager and
//    DetectorManager.
//
#include "InDetGeoModelUtils/InDetDetectorFactoryBase.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorManager.h"
namespace InDetDD {
    class AthenaComps;
}
class GeoPhysVol;

namespace HGTDGeo {

class HGTD_DetectorFactory : public InDetDD::DetectorFactoryBase {
public:
    HGTD_DetectorFactory(InDetDD::AthenaComps *athenaComps);
    virtual ~HGTD_DetectorFactory();
    // Creation of geometry:
    virtual void create(GeoPhysVol *world);
    // Access to the results:
    virtual HGTD_DetectorManager* getDetectorManager() const;
    // // // Print out how many of each layer/eta/phi etc. have been set up.
    // // void doNumerology();
    // // // Get the xml from the database instead of a file. Returns gzipped xml as a string.
    // // std::string getBlob();
    // // // Determine which alignment folders are loaded to decide if we register old or new folders
    // // virtual InDetDD::AlignFolderType getAlignFolderType() const;

private:
    // Copy and assignments operations illegal and so are made private
    HGTD_DetectorFactory(HGTD_DetectorFactory &right);
    HGTD_DetectorFactory & operator=(HGTD_DetectorFactory &right);

    HGTD_DetectorManager* m_detectorManager;
    InDetDD::AthenaComps* m_athenaComps;
}

}
#endif // HGTD_GEOMODEL_HGTD_DETECTORFACTORY_H
