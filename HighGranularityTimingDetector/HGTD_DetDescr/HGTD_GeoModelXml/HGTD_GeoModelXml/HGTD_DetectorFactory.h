/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HGTD_GeoModelXml_HGTD_DETECTORFACTORY_H
#define HGTD_GeoModelXml_HGTD_DETECTORFACTORY_H
//
//    Main class for building the GeoModel geometry, and handle the GeometryManager and
//    DetectorManager.
//
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "InDetGeoModelUtils/InDetDetectorFactoryBase.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorManager.h"

namespace InDetDD {
    class SiCommonItems;
}

class GeoPhysVol;

class HGTD_GeoModelAthenaComps;

namespace InDetDDSLHC {
    class HGTD_Options;
}

class HGTD_DetectorFactory : public InDetDD::DetectorFactoryBase {
public:
    HGTD_DetectorFactory(HGTD_GeoModelAthenaComps* athenaComps,
                         InDetDD::SiCommonItems* commonItems,
                         InDetDDSLHC::HGTD_Options& options);

    // Creation of geometry:
    virtual void create(GeoPhysVol* world);

    // Access to the results:
    virtual HGTD_DetectorManager* getDetectorManager() const;
    // Get the xml from the database instead of a file. Returns gzipped xml as a string.
    std::string getBlob();

private:
    // Copy and assignments operations illegal and so are made private
    HGTD_DetectorFactory(HGTD_DetectorFactory &right);
    HGTD_DetectorFactory & operator=(HGTD_DetectorFactory &right);

    HGTD_DetectorManager* m_detectorManager;
    HGTD_GeoModelAthenaComps* m_athComps;
    InDetDD::SiCommonItems* m_commonItems;
    InDetDDSLHC::HGTD_Options* m_options;
};

#endif // HGTD_GeoModelXml_HGTD_DETECTORFACTORY_H
