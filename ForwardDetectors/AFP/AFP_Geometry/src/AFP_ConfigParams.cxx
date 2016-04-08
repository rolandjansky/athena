/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_Geometry/AFP_ConfigParams.h"

//-------------------------------------------------------------------------------------------------------------
void AFP_CONFIGURATION::clear()
{
	bVP1Mode=false;

	sidcfg.clear();
	tdcfg.clear();
	hbpcfg.clear();

	fShortHBFloorXPos=SHORTHBFLOORXPOS;
	fShortHBYPos=STATIONSHIFTINYAXIS;
	fShortHBZPos=SHORTHBPZPOS;

	fLongHBFloorXPos=LONGHBFLOORXPOS;
	fLongHBYPos=STATIONSHIFTINYAXIS;
	fLongHBZPos=LONGHBPZPOS;

	bIsSHBRPotMode=false;
	bAddSecondQuartic=true;
	for(int i=0;i<QUARTICCNT;i++){
		fQuarticYPosInTube[i]=QUARTICYPOSINLHB;
	}

	fQuarticZPosInTube[0]=QUARTIC1ZPOSINLHB;
	fQuarticZPosInTube[1]=QUARTIC2ZPOSINLHB;
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
	fSlope=NOMINALSIDSLOPE;
	fLayerCount=SIDCNT;
	fLayerSpacing=NOMINALSIDSPACING;
	fDistanceFromPocket=NOMINALSIDPOCKETDISTANCE;
	bAddVacuumSensors=false;

	mapXStaggering.clear();
	mapYStaggering.clear();
	mapTransInStation.clear();
}

//-------------------------------------------------------------------------------------------------------------
void AFP_TDCONFIGURATION::clear()
{
#ifdef USE_TDLBARS
#else
	bAddCover=false;

	for(int i=0;i<QUARTICCNT;i++){
		QuarticConf[i].nRowsCnt=TDQUARTICROWSCNT;
		QuarticConf[i].nColsCnt=TDQUARTICCOLSCNT;
		QuarticConf[i].fQuarticLength=TDQUARTICLENGTH;
		QuarticConf[i].fSlope=NOMINALTDSLOPE;
		QuarticConf[i].fCoverThickness=bAddCover? TDCOVERTHICKNESS:(0.0*CLHEP::mm);
	}

	//Quartic 1
	QuarticConf[0].fCoverDistanceToFloor=NOMINALTDPOCKETDISTANCE;
	QuarticConf[0].vecBarXDim.resize(QuarticConf[0].nRowsCnt);
	QuarticConf[0].vecBarXDim[0]=1.0*CLHEP::mm; QuarticConf[0].vecBarXDim[1]=3.0*CLHEP::mm;
	QuarticConf[0].vecBarXDim[2]=3.0*CLHEP::mm; QuarticConf[0].vecBarXDim[3]=3.0*CLHEP::mm;
	QuarticConf[0].vecXGaps.resize(QuarticConf[0].nRowsCnt-1);
	QuarticConf[0].vecXGaps[0]=2.0*CLHEP::mm; QuarticConf[0].vecXGaps[1]=3.0*CLHEP::mm; QuarticConf[0].vecXGaps[2]=3.0*CLHEP::mm;

	QuarticConf[0].vecBarZDim.resize(QuarticConf[0].nColsCnt);
	fill_n(QuarticConf[0].vecBarZDim.begin(),QuarticConf[0].vecBarZDim.size(),6.0*CLHEP::mm);
	QuarticConf[0].vecZGaps.resize(QuarticConf[0].nColsCnt-1);
	fill_n(QuarticConf[0].vecZGaps.begin(),QuarticConf[0].vecZGaps.size(),0.0*CLHEP::mm);

	//Quartic 2
	QuarticConf[1].fCoverDistanceToFloor=NOMINALTDPOCKETDISTANCE+QuarticConf[0].vecBarXDim[0];
	QuarticConf[1].vecBarXDim.resize(QuarticConf[1].nRowsCnt);
	QuarticConf[1].vecBarXDim[0]=2.0*CLHEP::mm; QuarticConf[1].vecBarXDim[1]=3.0*CLHEP::mm;
	QuarticConf[1].vecBarXDim[2]=3.0*CLHEP::mm; QuarticConf[1].vecBarXDim[3]=3.0*CLHEP::mm;
	QuarticConf[1].vecXGaps.resize(QuarticConf[1].nRowsCnt-1);
	QuarticConf[1].vecXGaps[0]=3.0*CLHEP::mm; QuarticConf[1].vecXGaps[1]=3.0*CLHEP::mm; QuarticConf[1].vecXGaps[2]=3.0*CLHEP::mm;

	QuarticConf[1].vecBarZDim.resize(QuarticConf[1].nColsCnt);
	fill_n(QuarticConf[1].vecBarZDim.begin(),QuarticConf[1].vecBarZDim.size(),6.0*CLHEP::mm);
	QuarticConf[1].vecZGaps.resize(QuarticConf[1].nColsCnt-1);
	fill_n(QuarticConf[1].vecZGaps.begin(),QuarticConf[1].vecZGaps.size(),0.0*CLHEP::mm);
#endif

	mapTransInStation.clear();
}
