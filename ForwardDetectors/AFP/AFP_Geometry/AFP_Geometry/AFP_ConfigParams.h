#ifndef AFP_CONFIGPARAMS_H
#define AFP_CONFIGPARAMS_H

#include <list>
#include <map>
#include <vector>
#include <string>

#include "AFP_Geometry/AFP_constants.h"
#include "GeoModelKernel/GeoTransform.h"

enum eStationElement { ESE_RPOT, ESE_SID, ESE_TOF};
enum eSIDTransformType { ESTT_SUPPORT, ESTT_PLATE, ESTT_SENSOR, ESTT_FEI4CHIP, ESTT_VACUUMSENSOR, ESTT_SENSORLOCAL, ESTT_SENSORGLOBAL };
enum eAFPStation { EAS_UNKNOWN=-1, EAS_AFP00=0, EAS_AFP01=1, EAS_AFP02=2, EAS_AFP03=3 };

enum eLBarType { ELBT_UNDEFINED=0, ELBT_REGULAR=1, ELBT_METALELBOW=2, ELBT_AIRGUIDE=3, ELBT_HYBRID=4, ELBT_HYBRIDMETALELBOW=5, ELBT_ONEARM=6, ELBT_ONEARMAIRGUIDE=7, ELBT_MIKELBAR=8 };
enum eSkinReflectivityMode { ESRM_SINGLE=1, ESRM_SPECTRAL1=2 };
enum eMaterial { EM_VACUUM=0, EM_QUARTZ=1 };

//--------------------------------------------------------------------------------------------
typedef struct _AFPTOF_TRAININFO {
	_AFPTOF_TRAININFO()
	{
		nPmtRow=-1;
		bUseTaper=false;
		fTaperOffset=0.0;
		fTaperAngle=0.0;
		fLength=-1.0;
		fLGuideWidth=-1.0;
		fPerpShiftInPixel=0.0;
	}

	int nPmtRow;
	bool bUseTaper;
	double fTaperOffset;
	double fTaperAngle;
	double fLength;
	double fLGuideWidth;
	double fPerpShiftInPixel;
} AFPTOF_TRAININFO,*PAFPTOF_TRAININFO;

typedef struct _AFPTOF_LBARREFDIMENSIONS {
	_AFPTOF_LBARREFDIMENSIONS()
	{
		SetDefaults();
	}

	int nBarX1ID;
	int nBarX2ID;
	double fLBarThickness;
	double fRadLength;
	double fLGuideLength;

	void SetDefaults();
} AFPTOF_LBARREFDIMENSIONS, *PAFPTOF_LBARREFDIMENSIONS;

typedef struct _AFPTOF_LBARDIMENSIONS {
	_AFPTOF_LBARDIMENSIONS()
	{
		SetDefaults();
	}

	int nBarX1ID;
	int nBarX2ID;
	int nTrainID;
	double fLBarThickness;
	double fRadLength;
	double fRadYDim;
	double fLGuideWidth;
	double fLGuideLength;

	void SetDefaults();
} AFPTOF_LBARDIMENSIONS, *PAFPTOF_LBARDIMENSIONS;
//--------------------------------------------------------------------------------------------

typedef struct _AFP_TDCONFIGURATION {
	eLBarType eType;
	double fAlpha;
	int nX1PixCnt;
	int nX2PixCnt;
	double fPixelX1Dim;
	double fPixelX2Dim;
	bool bApplyBottomCut;
	bool bEmulateImmersion;
	bool bEmulateGlue;

	double fSkinReflectivity;
	eSkinReflectivityMode eSReflecMode;
	std::map<int,AFPTOF_LBARDIMENSIONS> mapBarDims;
	std::map<int,AFPTOF_TRAININFO> mapTrainInfo;
	AFPTOF_LBARREFDIMENSIONS RefBarDims;

    double fXFloorDistance;
    double fYPosInRPot;
    double fZPosInRPot;

	void SetDefault();
} AFP_TDCONFIGURATION, *PAFP_TDCONFIGURATION;


typedef struct _AFP_SIDCONFIGURATION {
    double fSlope;
    double fLayerCount;
    double fLayerSpacing;
	//double fXFloorDistance;
    double fZDistanceInRPot;
    bool bAddVacuumSensors;

	std::vector<double> vecXStaggering; //distance to floor
	std::vector<double> vecYStaggering; // zero for now

	std::vector<double> vecChipXPos;
	std::vector<double> vecChipYPos;
	std::vector<double> vecChipRotAngle;
	std::vector<double> vecChipXLength;
	std::vector<double> vecChipYLength;
	std::vector<double> vecSensorXPos;
	std::vector<double> vecSensorYPos;

	HepGeom::Transform3D TransInStation;

    void clear();
} AFP_SIDCONFIGURATION, *PAFP_SIDCONFIGURATION;

typedef struct _AFP_HBPCONFIGURATION {
    double windowPlateThickness, windowPlateAngle;
    bool windowPlatesInsteadOfHB;
    bool setMaterialToBeryllium;
    void clear();
} AFP_HBPCONFIGURATION, *PAFP_HBPCONFIGURATION;

typedef struct _AFP_CONFIGURATION {
	std::map<eAFPStation, AFP_SIDCONFIGURATION> sidcfg;
	std::map<eAFPStation, AFP_TDCONFIGURATION> tdcfg;

    std::vector<double> vecRPotFloorDistance;
    std::vector<double> vecRPotYPos;
    std::vector<double> vecStatNominalZPos;

    void clear();
} AFP_CONFIGURATION, *PAFP_CONFIGURATION;

typedef struct _AFP_RAWPROPERTYPARAMS
{


} AFP_RAWPROPERTYPARAMS, *PAFP_RAWPROPERTYPARAMS;

#endif // AFP_CONFIGPARAMS_H
