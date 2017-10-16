/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_GeoModelFactory_h
#define AFP_GeoModelFactory_h 1

#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "AFP_GeoModel/AFP_GeoModelManager.h"
#include "AFP_Geometry/AFP_Geometry.h"
#include <string>
#include <map>

typedef struct _AFP_BPMCOOLPARAMS {
    float fBpmMRRX, fBpmMRRY; //BPMR.6R1.B1 (231.535 m from IP)
    float fBpmMRLX, fBpmMRLY; //BPMR.6L1.B2 (225.245 m from IP)
    float fBpmSARX, fBpmSARY; //BPMSA.7R1.B1 (on ALFA station A7R1 237.7505 m from IP)
    float fBpmSALX, fBpmSALY; //BPMSA.7L1.B2 (on ALFA station A7L1 237.7505 m from IP)

    float fBpmWBRX, fBpmWBRY; //BPMWB.4R1.B1 (151.0945 m from IP)
    float fBpmYALX, fBpmYALY; //BPMYA.4L1.B2 (172.227 m from the IP)
} AFP_BPMCOOLPARAMS, *PAFP_BPMCOOLPARAMS;


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
    void UpdatePositions(PAFP_BPMCOOLPARAMS pBpmParams);

private:

    std::map<std::string,const GeoMaterial*> m_MapMaterials;

    void DefineMaterials();

    // Illegal operations:
    const AFP_GeoModelFactory & operator=(const AFP_GeoModelFactory &right);
    AFP_GeoModelFactory(const AFP_GeoModelFactory &right);

    // The manager:
    AFP_GeoModelManager       *m_detectorManager;
    StoreGateSvc             *m_detectorStore;

private:
    //common auxiliary map of solid shapes
    std::map<std::string,const GeoShape*> m_MapShape;
    AFP_CONFIGURATION m_CfgParams;
    AFP_Geometry* m_pGeometry;

    //Hamburg pipe part
    GeoShape* GetSolidShortHamburgPipe();
    GeoShape* GetSolidLongHamburgPipe();
    void AddShortHamburgPipe(GeoPhysVol* pPhysMotherVol, const char* pszStationName, HepGeom::Transform3D& TransInMotherVolume);
    void AddLongHamburgPipe(GeoPhysVol* pPhysMotherVol, const char* pszStationName, HepGeom::Transform3D& TransInMotherVolume);
    void AddRomanPot(GeoPhysVol* pPhysMotherVol, const char* pszStationName, HepGeom::Transform3D& TransInMotherVolume);

    //Thin plate for window angle study
    GeoShape* GetSolidShortWindowPlates();
    GeoShape* GetSolidLongWindowPlates();
    void AddShortWindowPlates(GeoPhysVol* pPhysMotherVol, const char* pszStationName, HepGeom::Transform3D& TransInMotherVolume);
    void AddLongWindowPlates(GeoPhysVol* pPhysMotherVol, const char* pszStationName, HepGeom::Transform3D& TransInMotherVolume);

    //Si detector part
    GeoShape* CreateSolidSIDPlate();
    GeoShape* CreateSolidSIDSupport();
    GeoShape* CreateSolidSIDCooling();
    GeoShape* CreateSolidSIDVacuumLayer();
    void AddSiDetector(GeoPhysVol* pPhysMotherVol, const char* pszStationName);

    //TOF part
    GeoOpticalSurface* m_pOpticalSurface;
    GeoOpticalSurface* m_pReflectionOptSurface;
    void InitializeTDParameters();
    StatusCode AddTimingDetector(const char* pszStationName, GeoOpticalPhysVol* pPhysMotherVol, HepGeom::Transform3D& TransInMotherVolume, GeoBorderSurfaceContainer* bsContainer);
    void AddLQBarSegment(const char* pszStationName, const int nQuarticID, const int nLQBarID,AFP_LQBARDIMENSIONS& LQBarDims, GeoOpticalPhysVol* pPhysMotherVolume, HepGeom::Transform3D& TransInMotherVolume, GeoBorderSurfaceContainer* bsContainer);
    void AddSepRadLBar(const char* pszStationName, const int nQuarticID, const int nQLBarID,AFP_LQBARDIMENSIONS& LQBarDims, GeoOpticalPhysVol* pPhysMotherVolume, HepGeom::Transform3D& TransInMotherVolume, GeoBorderSurfaceContainer* bsContainer);
    HepGeom::Vector3D<double> GetBarShift(AFP_LQBARDIMENSIONS& LQBarDims, eLQBarType eSpecType=ELBT_UNDEFINED);
    void AddHorizontalArm(const char* pszStationName, const int nQuarticID, const int nLQBarID, AFP_LQBARDIMENSIONS& LQBarDims, GeoOpticalPhysVol* pPhysMotherVolume, HepGeom::Transform3D& PartialTransInMotherVolume, GeoBorderSurfaceContainer* bsContainer);
    void AddSensor(const char* pszStationName, const int nQuarticID, const int nLQBarID, AFP_LQBARDIMENSIONS& LQBarDims, GeoOpticalPhysVol* pPhysMotherVolume, HepGeom::Transform3D &TransInMotherVolume);
    void GetLQBarDimensions(const int nRowID, const int nColID, PAFP_LQBARDIMENSIONS pLQBarDims);
};

// Class AFP_GeoModelFactory
#endif


