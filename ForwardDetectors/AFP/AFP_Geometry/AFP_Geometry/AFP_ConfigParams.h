/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_CONFIGPARAMS_H
#define AFP_CONFIGPARAMS_H

#include <list>
#include <map>
#include <vector>
#include <string>

#include "AFP_Geometry/AFP_constants.h"
#include "GeoModelKernel/GeoTransform.h"

enum eLQBarType { ELBT_UNDEFINED=0, ELBT_REGULAR=1, ELBT_METALELBOW=2, ELBT_AIRGUIDE=3, ELBT_HYBRID=4, ELBT_HYBRIDMETALELBOW=5, ELBT_ONEARM=6, ELBT_ONEARMAIRGUIDE=7, ELBT_MIKELBAR=8 };
enum eSkinReflectivityMode { ESRM_SINGLE=1, ESRM_SPECTRAL1=2 };
enum eMaterial { EM_VACUUM=0, EM_QUARTZ=1 };

typedef struct _AFP_LQBARDIMENSIONS {
    eLQBarType eType;
    double fAlpha;
    double fLBarZDim;
    int nNumOfSensors;
    double fOffsetFromBeam;
    double fSkinReflectivity;
    eSkinReflectivityMode eSReflecMode;

    double fVertBarXDim;
    double fVertBarYDim;
    bool bApplyBottomCut;
    double fRadiatorLength; //extra air guide params
    bool bIs45degElbow; //extra air guide params

    double fHorzBarXDim;
    double fHorzBarYDim;
    double fHorzBarExtYOffset;
    double fHorzBarTaperAngle;
    bool bSepHorzBarInTaper;
    double fHorzBarXTaperOffset;
    eMaterial eTaperMaterial;

    void SetDefault();
} AFP_LQBARDIMENSIONS, *PAFP_LQBARDIMENSIONS;

typedef struct _AFP_TDCONFIGURATION {
    int nRowsCnt;
    int nColsCnt;
    double fSlope;
    double fVertXGap;
    double fVertZGap;
    double fHorzYGap;
    std::vector<double> vecBarXDim;

    AFP_LQBARDIMENSIONS MainLQBarDims;

    double fXFloorDistance;
    double fYPosInRPot;
    double fZPosInRPot;

    void clear();
} AFP_TDCONFIGURATION, *PAFP_TDCONFIGURATION;


typedef struct _AFP_SIDCONFIGURATION {
    double fSlope;
    double fLayerCount;
    double fLayerSpacing;
    double fXFloorDistance;
    double fZDistanceInRPot;
    bool bAddVacuumSensors;
	double fSupportThickness;

    std::map<std::string, std::vector<double> > mapXStaggering;
    std::map<std::string, std::vector<double> > mapYStaggering;
    std::map<std::string, HepGeom::Transform3D> mapTransInStation;

    void clear();
} AFP_SIDCONFIGURATION, *PAFP_SIDCONFIGURATION;

typedef struct _AFP_HBPCONFIGURATION {
    double windowPlateThickness, windowPlateAngle;
    bool windowPlatesInsteadOfHB;
    bool setMaterialToBeryllium;
    void clear();
} AFP_HBPCONFIGURATION, *PAFP_HBPCONFIGURATION;

typedef struct _AFP_CONFIGURATION {
    AFP_SIDCONFIGURATION sidcfg;
    AFP_TDCONFIGURATION tdcfg;
    AFP_HBPCONFIGURATION hbpcfg;

    std::vector<double> vecRPotFloorDistance;
    std::vector<double> vecRPotYPos;
    std::vector<double> vecStatNominalZPos;

    void clear();
} AFP_CONFIGURATION, *PAFP_CONFIGURATION;

typedef struct _AFP_RAWPROPERTYPARAMS
{


} AFP_RAWPROPERTYPARAMS, *PAFP_RAWPROPERTYPARAMS;

#endif // AFP_CONFIGPARAMS_H
