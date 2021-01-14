/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HGTD_GEOMODEL_HGTD_DETECTORFACTORY_H
#define HGTD_GEOMODEL_HGTD_DETECTORFACTORY_H
//
//    Main routine to build the GeoModel geometry, and handle the GeometryManager and
//    DetectorManager.
//
#include "StoreGate/DataHandle.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "InDetGeoModelUtils/InDetDetectorFactoryBase.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorManager.h"

namespace InDetDD {
    class AthenaComps;
    class SiCommonItems;
    class HGTD_ModuleDesign;
}
class GeoPhysVol;

namespace HGTDGeo {

struct HgtdGeoParams {
    double rMid;
    double rOuter;
    double diskRotation;
    double rowSpaceSide;
    double rowBacksideInnerShift;
    double rowBacksideOuterShift;
    double moduleSpaceInner;
    double moduleSpaceOuter;
    double flexSheetSpacing;
};

struct ModulePosition {
    double x;
    double y;
    double phiRotation;
    bool flipped;
    int row;
    int el_in_row;
};

struct GeoCylVolParams {
    std::string name;
    double rMin;
    double rMax;
    double zHalf;
    double zOffsetLocal;
    std::string material;
};

struct GeoBoxVolParams {
    std::string name;
    double xHalf;
    double yHalf;
    double zHalf;
    double zOffsetLocal;
    std::string material;
};


class HGTD_DetectorFactory : public InDetDD::DetectorFactoryBase {
public:
    HGTD_DetectorFactory(InDetDD::AthenaComps* athenaComps, InDetDD::SiCommonItems* commonItems, bool fullGeo);
    virtual ~HGTD_DetectorFactory();

    // Creation of geometry:
    virtual void create(GeoPhysVol *world);

    // Access to the results:
    virtual HGTD_DetectorManager* getDetectorManager() const;

    void setHGTDBaseline(bool flag) {m_HGTD_isbaseline = flag;}

private:
    // Copy and assignments operations illegal and so are made private
    HGTD_DetectorFactory(HGTD_DetectorFactory &right);
    HGTD_DetectorFactory & operator=(HGTD_DetectorFactory &right);

    GeoFullPhysVol* createEnvelope(bool bPos);

    HGTD_DetectorManager* m_detectorManager;
    InDetDD::AthenaComps* m_athenaComps;
    InDetDD::SiCommonItems* m_commonItems;

    bool m_fullGeo;  // true->FULL, false->RECO

    // this is for the HGTD readout definition
    bool m_HGTD_isbaseline;

    std::map<std::string,GeoCylVolParams> m_cylVolPars;
    std::map<std::string,GeoBoxVolParams> m_boxVolPars;
    HgtdGeoParams m_hgtdPars;

    void buildHgtdGeoTDR(const DataHandle<StoredMaterialManager>& matmanager, GeoFullPhysVol* parent, bool bPos);
    std::vector<ModulePosition> calculateHgtdModulePositionsInQuadrant(int layer);
    std::vector<ModulePosition> calculateHgtdModulePositionsInRow(int row, bool back = false);

    InDetDD::HGTD_ModuleDesign* createHgtdDesign(double thickness, bool isBaseline = true, bool isflipped = false);

};

} // End HGTDGeo namespace

#endif // HGTD_GEOMODEL_HGTD_DETECTORFACTORY_H
