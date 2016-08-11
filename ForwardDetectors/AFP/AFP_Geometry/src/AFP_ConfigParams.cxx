/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_Geometry/AFP_ConfigParams.h"

//-------------------------------------------------------------------------------------------------------------
void AFP_CONFIGURATION::clear()
{
    sidcfg.clear();
    tdcfg.clear();
    hbpcfg.clear();

    vecRPotFloorDistance.resize(4);
    vecRPotYPos.resize(4);
    fill_n(vecRPotFloorDistance.begin(),vecRPotFloorDistance.size(),RPFLOORDISTANCE);
    fill_n(vecRPotYPos.begin(),vecRPotYPos.size(),STATIONSHIFTINYAXIS);

    vecStatNominalZPos.resize(4);
    vecStatNominalZPos[0]=OUTERSTATZDISTANCE;
    vecStatNominalZPos[1]=INNERSTATZDISTANCE;
    vecStatNominalZPos[2]=-INNERSTATZDISTANCE;
    vecStatNominalZPos[3]=-OUTERSTATZDISTANCE;
}

//-------------------------------------------------------------------------------------------------------------
void AFP_HBPCONFIGURATION::clear()
{
    windowPlateThickness = 0.3;
    windowPlateAngle = 90.0;
    windowPlatesInsteadOfHB = false;
    setMaterialToBeryllium = false;
}

//-------------------------------------------------------------------------------------------------------------
void AFP_SIDCONFIGURATION::clear()
{
    fSlope=SID_NOMINALSLOPE;
    fLayerCount=SIDCNT;
    fLayerSpacing=SID_NOMINALSPACING;
    fXFloorDistance=SID_DISTANCETOFLOOR;
    fZDistanceInRPot=SID_ZDISTANCEINRPOT;
    bAddVacuumSensors=false;
	fSupportThickness=SID_PLATETHICKNESS;

    mapXStaggering.clear();
    mapYStaggering.clear();
    mapTransInStation.clear();
}

//-------------------------------------------------------------------------------------------------------------
void AFP_LQBARDIMENSIONS::SetDefault()
{
    nNumOfSensors=1;
    eType=ELBT_METALELBOW;
    fAlpha=48.0*CLHEP::deg;
    fLBarZDim=5.0*CLHEP::mm;

    fVertBarXDim=2.0*CLHEP::mm;
    fVertBarYDim=50.3*CLHEP::mm;
    fHorzBarXDim=71.0*CLHEP::mm-fVertBarXDim;
    fHorzBarYDim=5.0*CLHEP::mm;

    eSReflecMode=ESRM_SINGLE;
    fSkinReflectivity=0.90;
    fOffsetFromBeam=0.0*CLHEP::mm;
    fRadiatorLength=20.0*CLHEP::mm;

    bIs45degElbow=true;
    bApplyBottomCut=true;

    bSepHorzBarInTaper=false;
    fHorzBarExtYOffset=0.0*CLHEP::mm;
    fHorzBarTaperAngle=0.0*CLHEP::deg;
    fHorzBarXTaperOffset=0.0*CLHEP::mm;
    eTaperMaterial=EM_QUARTZ;
}

void AFP_TDCONFIGURATION::clear()
{
    nRowsCnt=TD_TRAINSCNT;
    nColsCnt=TD_COLUMNSCNT;
    fSlope=TD_NOMINALSLOPE;
    fVertXGap=TD_VERTXGAP;
    fVertZGap=TD_VERTZGAP;
    fHorzYGap=TD_HORZYGAP;

    vecBarXDim.resize(nRowsCnt);
    fill_n(vecBarXDim.begin(),vecBarXDim.size(),4.0*CLHEP::mm);
    vecBarXDim[0]=2.0*CLHEP::mm;

    MainLQBarDims.SetDefault();
    MainLQBarDims.fVertBarXDim=vecBarXDim[0];

    fXFloorDistance=TD_DISTANCETOFLOOR;
    fYPosInRPot=0.0*CLHEP::mm;
    fZPosInRPot=TD_ZDISTANCEINRPOT;
}
