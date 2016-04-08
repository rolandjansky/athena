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

typedef struct _AFP_TDQUARTICCONFIGURATION {
	int nRowsCnt;
	int nColsCnt;
	double fQuarticLength;
	double fSlope;
	double fCoverThickness;
	double fCoverDistanceToFloor;
	std::vector<double> vecBarXDim;
	std::vector<double> vecBarZDim;
	std::vector<double> vecXGaps;
	std::vector<double> vecZGaps;
} AFP_TDQUARTICCONFIGURATION, *PAFP_TDQUARTICCONFIGURATION;

typedef struct _AFP_INTQUARTICDIMENSIONS {
#ifdef USE_TDLBARS
	std::vector<double> arrLBarZDim;
	std::vector<double> arrLBarZGap;

	std::vector<double> arrVertBarXDim;
	std::vector<double> arrVertBarYDim;
	std::vector<double> arrVertBarXGap;

	std::vector<double> arrHorzBarXDim;
	std::vector<double> arrHorzBarYDim;
	std::vector<double> arrHorzBarYGap;
#else
	std::vector<double> arrTDQuarticXDim;
	std::vector<double> arrTDQuarticGapXDim;
	std::vector<double> arrTDQuarticYDim;
	std::vector<double> arrTDQuarticZDim;
	std::vector<double> arrTDQuarticGapZDim;
#endif
} AFP_INTQUARTICDIMENSIONS, *PAFP_INTQUARTICDIMENSIONS;

typedef struct _AFP_TDCONFIGURATION {
	bool bAddCover;
	AFP_TDQUARTICCONFIGURATION QuarticConf[QUARTICCNT];

	std::map<std::string, HepGeom::Transform3D> mapTransInStation;

	void clear();
} AFP_TDCONFIGURATION, *PAFP_TDCONFIGURATION;

typedef struct _AFP_SIDCONFIGURATION {
	double fSlope;
	double fLayerCount;
	double fLayerSpacing;
	double fDistanceFromPocket;
	bool bAddVacuumSensors;

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
	bool bVP1Mode;

	AFP_SIDCONFIGURATION sidcfg;
	AFP_TDCONFIGURATION tdcfg;
	AFP_HBPCONFIGURATION hbpcfg;

	double fShortHBFloorXPos;
	double fLongHBFloorXPos;
	double fShortHBZPos;
	double fLongHBZPos;
	double fShortHBYPos;
	double fLongHBYPos;

	bool bIsSHBRPotMode; //use Roman Pot instead of Short HBP
	bool bAddSecondQuartic;
	double fQuarticYPosInTube[QUARTICCNT];
	double fQuarticZPosInTube[QUARTICCNT];
	void clear();
} AFP_CONFIGURATION, *PAFP_CONFIGURATION;

#endif // AFP_CONFIGPARAMS_H
