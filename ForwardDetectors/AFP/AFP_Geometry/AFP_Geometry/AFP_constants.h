/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_CONSTANTS_H
#define AFP_CONSTANTS_H 1

#include "CLHEP/Units/defs.h"
#include "CLHEP/Units/SystemOfUnits.h"

//#define USE_TDLBARS 1

struct AFP_CONSTANTS {
	const int Stat_GlobalVacuumSensorID=11;
	const double Stat_GlobalVacuumSensorThickness=0.0001*CLHEP::mm;
	const double Stat_GlobalVacuumSensorZOffset=275.0*CLHEP::mm;

	const double SiT_Plate_amount=4;
	const double SiT_DistanceToFloor=0.4*CLHEP::mm;
	const double SiT_NearDistanceToFloor=0.4*CLHEP::mm;
	const double SiT_FarDistanceToFloor=0.3*CLHEP::mm;
	const double SiT_ZDistanceInRPot=20.0*CLHEP::mm;
	const double SiT_CorrZOffset=5.0*CLHEP::mm;

	const double SiT_Plate_Main_length_x=47.5*CLHEP::mm;
	const double SiT_Plate_Main_length_y=58.0*CLHEP::mm;
	const double SiT_Plate_Main_thickness=1.0*CLHEP::mm;

	const double SiT_Plate_Window_length_x=18.0*CLHEP::mm;
	const double SiT_Plate_Window_length_y=20.0*CLHEP::mm;
	const double SiT_Plate_Window_thickness=0.5*CLHEP::mm;

	const double SiT_Plate_Window_x=+14.75*CLHEP::mm;
	const double SiT_Plate_Window_y=0.0*CLHEP::mm;

	const double SiT_Plate_CutEdge_length_x=28.0*CLHEP::mm;
	const double SiT_Plate_CutEdge_length_y=14.0*CLHEP::mm;
	const double SiT_Plate_CutEdge_thickness=1.0*CLHEP::mm;

	const double SiT_Plate_CutEdge_x1=0.0*CLHEP::mm; //TO BE DEFINED
	const double SiT_Plate_CutEdge_y1=0.0*CLHEP::mm; //TO BE DEFINED
	const double SiT_Plate_CutEdge_x2=0.0*CLHEP::mm; //TO BE DEFINED
	const double SiT_Plate_CutEdge_y2=0.0*CLHEP::mm; //TO BE DEFINED

	const double SiT_Plate_zsep=9.0*CLHEP::mm;
	const double SiT_Plate_rot_xz=14.0*CLHEP::deg;

	const double SiT_Chip_length_x=18.8*CLHEP::mm;
	const double SiT_Chip_length_y=20.2*CLHEP::mm;
	const double SiT_Chip_thickness=0.7*CLHEP::mm;

	const double SiT_Chip_x=0.5*(SiT_Plate_Main_length_x-SiT_Chip_length_x);
	const double SiT_Chip_y=0.0*CLHEP::mm;
	const double SiT_Chip_rot=0.0*CLHEP::deg;

	const double SiT_Pixel_amount_x=336;
	const double SiT_Pixel_amount_y=80;
	const double SiT_Pixel_length_x=0.050*CLHEP::mm;
	const double SiT_Pixel_length_y=0.250*CLHEP::mm;
	const double SiT_Pixel_thickness=0.23*CLHEP::mm;
	const double SiT_Pixel_length_totx=SiT_Pixel_amount_x*SiT_Pixel_length_x;
	const double SiT_Pixel_length_toty=SiT_Pixel_amount_y*SiT_Pixel_length_y;

	const double SiT_Pixel_x=0.5*(SiT_Chip_length_x-SiT_Pixel_length_totx);
	const double SiT_Pixel_y=0.0*CLHEP::mm;

	const double SiT_DeathEdge=0.0*CLHEP::mm;
	const double SiT_LowerEdge=0.1*CLHEP::mm;

	const double ToF_MinBarGap=0.02*CLHEP::mm;
	const double ToF_Sensor2BarDist=0.001*CLHEP::mm;
	const double ToF_LGuideTrainOffset=0.1*CLHEP::mm;
	const double ToF_SensorCentreXPos=-31.502*CLHEP::mm;
	const double ToF_SensorCentreZPos=45.017*CLHEP::mm;
	const double ToF_HolderPinXPos=-38.304*CLHEP::mm;
	const double ToF_HolderPinZPos=47.137*CLHEP::mm;
	const double ToF_DistanceToFloor=0.200*CLHEP::mm;
    const double ToF_SeparationWindowDiameter=54.5*CLHEP::mm;
    const double ToF_SeparationWindowThickness=2.9*CLHEP::mm;

	const double ToF_TrainsCnt=4;
	const double ToF_ColumnsCnt=4;
	const double ToF_NominalSlope=48*CLHEP::deg;
	const double ToF_SensorThickness=0.050*CLHEP::mm;

	const double Stat_InnerZDistance=205.217*CLHEP::m; //204.500*CLHEP::m
	const double Stat_OuterZDistance=217.302*CLHEP::m; //212.675*CLHEP::m
	const double Stat_ShiftInYAxis=0.0*CLHEP::mm;
	const double Stat_RPotFloorDistance=1.5*CLHEP::mm;
};

#define DETXSIDE (-1)
// #define LHCXOFFSET (-97.0*CLHEP::mm)
#define LHCXOFFSET (0.0*CLHEP::mm)


#endif //AFP_CONSTANTS_H
