/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_GeoModelFactory_h
#define AFP_GeoModelFactory_h 1

#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "AFP_GeoModelManager.h"
#include "AFP_Geometry/AFP_Geometry.h"
#include <string>
#include <map>

#define SLIMCUT (0.01*CLHEP::mm)

struct AFP_BPMCOOLPARAMS {
    float fBpmMRRX, fBpmMRRY; //BPMR.6R1.B1 (231.535 m from IP)
    float fBpmMRLX, fBpmMRLY; //BPMR.6L1.B2 (225.245 m from IP)
    float fBpmSARX, fBpmSARY; //BPMSA.7R1.B1 (on ALFA station A7R1 237.7505 m from IP)
    float fBpmSALX, fBpmSALY; //BPMSA.7L1.B2 (on ALFA station A7L1 237.7505 m from IP)

    float fBpmWBRX, fBpmWBRY; //BPMWB.4R1.B1 (151.0945 m from IP)
    float fBpmYALX, fBpmYALY; //BPMYA.4L1.B2 (172.227 m from the IP)
};


class StoreGateSvc;
class AFP_GeoModelFactory : public GeoVDetectorFactory
{

public:
    AFP_GeoModelFactory(StoreGateSvc *pDetStore, AFP_Geometry* pGeometry);
    ~AFP_GeoModelFactory();

    // Creation of geometry:
    virtual void create(GeoPhysVol *world);

    // Access to the results:
    virtual const AFP_GeoModelManager * getDetectorManager() const;
    void updatePositions(AFP_BPMCOOLPARAMS* pBpmParams);

private:
    AFP_CONSTANTS m_AfpConstants;
    std::map<std::string,const GeoMaterial*> m_MapMaterials;

    void defineMaterials();

    // Illegal operations:
    const AFP_GeoModelFactory & operator=(const AFP_GeoModelFactory &right);
    AFP_GeoModelFactory(const AFP_GeoModelFactory &right);

    // The manager:
    AFP_GeoModelManager *m_pDetectorManager;
    StoreGateSvc *m_pDetectorStore;

private:
    //common auxiliary map of solid shapes
    std::map<std::string,const GeoShape*> m_MapShape;
    AFP_CONFIGURATION m_CfgParams;
    AFP_Geometry* m_pGeometry;
    const bool m_addSeparationWindow;

    //Si detector part
    GeoShape* createSolidSIDPlate();
    void addSiDetector(GeoPhysVol* pPhysMotherVol, const char* pszStationName, HepGeom::Transform3D& TransInMotherVolume);
    void addRomanPot(GeoPhysVol* pPhysMotherVol, const char* pszStationName, HepGeom::Transform3D& TransInMotherVolume);

    //TOF part
    GeoOpticalSurface* m_pOpticalSurface;
    GeoOpticalSurface* m_pReflectionOptSurface;
    void initializeTDParameters();
    StatusCode addTimingDetector(const char* pszStationName, GeoOpticalPhysVol* pPhysMotherVol, HepGeom::Transform3D& TransInMotherVolume, GeoBorderSurfaceContainer* bsContainer);
    void addLQBarSegment(const char* pszStationName, const int nQuarticID, const int nLQBarID,AFPTOF_LBARDIMENSIONS& LQBarDims, GeoOpticalPhysVol* pPhysMotherVolume, HepGeom::Transform3D& TransInMotherVolume, GeoBorderSurfaceContainer* bsContainer);
    void addSepRadLBar(const char* pszStationName, const int nQuarticID, const int nBarID, GeoOpticalPhysVol* pPhysMotherVolume, HepGeom::Transform3D& TransInMotherVolume, GeoBorderSurfaceContainer* bsContainer);
    HepGeom::Vector3D<double> getBarShift(AFPTOF_LBARDIMENSIONS& LQBarDims, eLBarType eSpecType=ELBT_UNDEFINED);
    void addHorizontalArm(const char* pszStationName, const int nQuarticID, const int nLQBarID, AFPTOF_LBARDIMENSIONS& LQBarDims, GeoOpticalPhysVol* pPhysMotherVolume, HepGeom::Transform3D& PartialTransInMotherVolume, GeoBorderSurfaceContainer* bsContainer);
    void addSensor(const char* pszStationName, const int nQuarticID, GeoOpticalPhysVol* pPhysMotherVolume, HepGeom::Transform3D &TransInMotherVolume, GeoBorderSurfaceContainer* bsContainer);
    void addLBarSensorSeparationWindow(const char* pszStationName, const int nQuarticID, GeoOpticalPhysVol* pPhysMotherVolume, HepGeom::Transform3D &TransInMotherVolume, GeoBorderSurfaceContainer* bsContainer);
    void getLQBarDimensions(const int nRowID, const int nColID, AFPTOF_LBARDIMENSIONS* pLQBarDims);
};

// Class AFP_GeoModelFactory
#endif


