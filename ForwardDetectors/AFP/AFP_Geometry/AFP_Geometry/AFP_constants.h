#ifndef AFP_CONSTANTS_H
#define AFP_CONSTANTS_H 1

#include "CLHEP/Units/defs.h"
#include "CLHEP/Units/SystemOfUnits.h"

//#define USE_TDLBARS 1

typedef struct _AFP_CONSTANTS {
	const double SiT_Plate_amount=4;

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
	const double SiT_Pixel_thickness=0.7*CLHEP::mm;
	const double SiT_Pixel_length_totx=SiT_Pixel_amount_x*SiT_Pixel_length_x;
	const double SiT_Pixel_length_toty=SiT_Pixel_amount_y*SiT_Pixel_length_y;

	const double SiT_Pixel_x=0.5*(SiT_Chip_length_x-SiT_Pixel_length_totx);
	const double SiT_Pixel_y=0.0*CLHEP::mm;

	const double ToF_MinBarGap=0.02*CLHEP::mm;
	const double ToF_Sensor2BarDist=0.001*CLHEP::mm;
	const double ToF_LGuideTrainOffset=0.1*CLHEP::mm;
	const double ToF_SensorCentreXPos=-31.502*CLHEP::mm;
	const double ToF_SensorCentreZPos=45.017*CLHEP::mm;
	const double ToF_HolderPinXPos=-38.304*CLHEP::mm;
	const double ToF_HolderPinZPos=47.137*CLHEP::mm;
	const double ToF_DistanceToFloor=0.200*CLHEP::mm;

} AFP_CONSTANTS, *PAFP_CONSTANTS;

#define DETXSIDE (-1)
#define LHCXOFFSET (-97.0*CLHEP::mm)

//#define SIDCNT 4
//#define SID_NOMINALSLOPE (14*CLHEP::deg) //CHANGE
//#define SID_NOMINALSPACING (9.0*CLHEP::mm) //CHANGE
#define SID_DISTANCETOFLOOR (0.4*CLHEP::mm) //CHANGE
#define SID_NEARDISTANCETOFLOOR (0.4*CLHEP::mm) //CHANGE
#define SID_FARDISTANCETOFLOOR (0.3*CLHEP::mm) //CHANGE
#define SID_ZDISTANCEINRPOT (20.0*CLHEP::mm)
#define SID_CORRZOFFSET (5.0*CLHEP::mm)
#define SID_SENSORXDIM (18.8*CLHEP::mm)
#define SID_SENSORYDIM (20.2*CLHEP::mm)
//#define SID_SENSORTHICKNESS (0.230*CLHEP::mm) //CHANGE
//#define SID_FEI4XDIM (18.8*CLHEP::mm) //CHANGE
//#define SID_FEI4YDIM (20.2*CLHEP::mm)
//#define SID_FEI4THICKNESS (0.7*CLHEP::mm)
///#define SID_MAINPLATEXDIM (75.6*CLHEP::mm)
//#define SID_MAINPLATEYDIM (80.0*CLHEP::mm)
//#define SID_PLATETHICKNESS (1.0*CLHEP::mm)
#define SID_VACUUMSENSORTHICKNESS (0.0001*CLHEP::mm)
#define SID_GLOBALVACUUMSENSORID 11
#define SID_GLOBALVACUUMSENSORZOFFSET (275*CLHEP::mm)
#define SID_DEATH_EDGE (1.9*CLHEP::mm)
#define SID_LOWER_EDGE (0.1*CLHEP::mm)

#define TD_TRAINSCNT 4
#define TD_COLUMNSCNT 2
#define TD_NOMINALSLOPE (48*CLHEP::deg)
#define TD_FIRSTBARVERTLENGTH (150.0*CLHEP::mm)
#define TD_FIRSTBARHORZLENGTH (50.0*CLHEP::mm)
#define TD_SLIMBARBORDERWIDTH (0.001*CLHEP::mm)
#define TD_SLIMCUT (0.01*CLHEP::mm)
#define TD_SENSORTHICKNESS (1.0*CLHEP::mm)
//#define TD_DISTANCETOFLOOR (0.4*CLHEP::mm)
#define TD_ZDISTANCEINRPOT (10.0*CLHEP::mm)
#define TD_VERTXGAP (0.03*CLHEP::mm)
#define TD_VERTZGAP (0.13*CLHEP::mm)
#define TD_HORZYGAP (0.13*CLHEP::mm)
#define TD_SENSOR2BARDIST (0.001*CLHEP::mm)
#define TD_SENSORWIDTH (0.050*CLHEP::mm)

#define INNERSTATZDISTANCE (205.217*CLHEP::m)//(204.500*CLHEP::m)
#define OUTERSTATZDISTANCE (217.302*CLHEP::m)//(212.675*CLHEP::m)
#define STATIONSHIFTINYAXIS (0.0*CLHEP::mm) //CHANGE
#define RPFLOORDISTANCE (1.5*CLHEP::mm) //nominal
#define RP00FLOORDISTANCE (1.5*CLHEP::mm)
#define RP01FLOORDISTANCE (1.5*CLHEP::mm)
#define RP02FLOORDISTANCE (4.04*CLHEP::mm)
#define RP03FLOORDISTANCE (2.16*CLHEP::mm)

#define HBPTHINWINDOWTHICKNESS (0.3*CLHEP::mm) // design window thickness; maximum is 3 mm (wall thickness)
#define HBPWINDOWCUTYSIGN (1) // y axis sign, where thin window should have cutted corner

#endif //AFP_CONSTANTS_H
