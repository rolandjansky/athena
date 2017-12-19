#include "AFP_Geometry/AFP_ConfigParams.h"

//-------------------------------------------------------------------------------------------------------------
void AFP_CONFIGURATION::clear()
{
	AFP_SIDCONFIGURATION defsidcfg;
	defsidcfg.clear();
	sidcfg[EAS_AFP00]=defsidcfg;
	sidcfg[EAS_AFP01]=defsidcfg;
	sidcfg[EAS_AFP02]=defsidcfg;
	sidcfg[EAS_AFP03]=defsidcfg;

	AFP_TDCONFIGURATION deftofcfg;
	deftofcfg.SetDefault();
	tdcfg[EAS_AFP00]=deftofcfg;
	tdcfg[EAS_AFP03]=deftofcfg;

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
	AFP_CONSTANTS AfpConstants;

	fSlope=AfpConstants.SiT_Plate_rot_xz;
	fLayerCount=AfpConstants.SiT_Plate_amount;
	fLayerSpacing=AfpConstants.SiT_Plate_zsep;
    fZDistanceInRPot=SID_ZDISTANCEINRPOT;
    bAddVacuumSensors=false;

	vecXStaggering.resize(AfpConstants.SiT_Plate_amount);
	vecYStaggering.resize(AfpConstants.SiT_Plate_amount);
	vecChipXPos.resize(AfpConstants.SiT_Plate_amount);
	vecChipYPos.resize(AfpConstants.SiT_Plate_amount);
	vecChipRotAngle.resize(AfpConstants.SiT_Plate_amount);
	vecChipXLength.resize(AfpConstants.SiT_Plate_amount);
	vecChipYLength.resize(AfpConstants.SiT_Plate_amount);
	vecSensorXPos.resize(AfpConstants.SiT_Plate_amount);
	vecSensorYPos.resize(AfpConstants.SiT_Plate_amount);

	std::fill_n(vecXStaggering.begin(),AfpConstants.SiT_Plate_amount,SID_DISTANCETOFLOOR);
	std::fill_n(vecXStaggering.begin(),AfpConstants.SiT_Plate_amount,0.0*CLHEP::mm);
	std::fill_n(vecChipXPos.begin(),AfpConstants.SiT_Plate_amount,AfpConstants.SiT_Chip_x);
	std::fill_n(vecChipYPos.begin(),AfpConstants.SiT_Plate_amount,AfpConstants.SiT_Chip_y);
	std::fill_n(vecChipRotAngle.begin(),AfpConstants.SiT_Plate_amount,AfpConstants.SiT_Chip_rot);
	std::fill_n(vecChipXLength.begin(),AfpConstants.SiT_Plate_amount,AfpConstants.SiT_Chip_length_x);
	std::fill_n(vecChipYLength.begin(),AfpConstants.SiT_Plate_amount,AfpConstants.SiT_Chip_length_y);
	std::fill_n(vecSensorXPos.begin(),AfpConstants.SiT_Plate_amount,AfpConstants.SiT_Pixel_x);
	std::fill_n(vecSensorYPos.begin(),AfpConstants.SiT_Plate_amount,AfpConstants.SiT_Pixel_y);

	TransInStation=HepGeom::Transform3D();
}

//-------------------------------------------------------------------------------------------------------------

void AFPTOF_LBARREFDIMENSIONS::SetDefaults()
{
	nBarX1ID=4;
	nBarX2ID=2;
	fLBarThickness=6.0*CLHEP::mm;
	fRadLength=56.78*CLHEP::mm;
	fLGuideLength=71.3*CLHEP::mm;
}

void AFPTOF_LBARDIMENSIONS::SetDefaults()
{
	nBarX1ID=1;
	nBarX2ID=1;
	nTrainID=4;
	fLBarThickness=6.0*CLHEP::mm;
	fRadLength=50.0*CLHEP::mm;
	fRadYDim=5.0*CLHEP::mm;
	fLGuideWidth=5.0*CLHEP::mm;
	fLGuideLength=70.0*CLHEP::mm;
}
void AFP_TDCONFIGURATION::SetDefault()
{
	AFP_CONSTANTS AfpConstants;

	eType=ELBT_METALELBOW;
	fAlpha=48.0*CLHEP::deg;
	nX1PixCnt=4;
	nX2PixCnt=4;
	fPixelX1Dim=6.25;
	fPixelX2Dim=6.25;
	bApplyBottomCut=true;
	bEmulateImmersion=true;
	bEmulateGlue=true;

	fSkinReflectivity=0.0;
	eSReflecMode=ESRM_SINGLE;

	mapBarDims.clear();

	fXFloorDistance=AfpConstants.ToF_DistanceToFloor;
	fYPosInRPot=-0.5*AfpConstants.SiT_Chip_length_y;
	fZPosInRPot=0.0;

	//setup train info
	AFPTOF_TRAININFO ti;
	mapTrainInfo.clear();

	ti.nPmtRow=4; ti.fLGuideWidth=5.0*CLHEP::mm; ti.fPerpShiftInPixel=0.0*CLHEP::mm;
	ti.bUseTaper=true; ti.fTaperAngle=18.0*CLHEP::deg; ti.fTaperOffset=3.0*CLHEP::mm;
	ti.fLength=-1.0*CLHEP::mm;
	mapTrainInfo[1]=ti;

	ti.nPmtRow=3; ti.fLGuideWidth=5.0*CLHEP::mm; ti.fPerpShiftInPixel=0.0*CLHEP::mm;
	ti.bUseTaper=true; ti.fTaperAngle=18.0*CLHEP::deg; ti.fTaperOffset=1.0*CLHEP::mm;
	ti.fLength=-1.0*CLHEP::mm;
	mapTrainInfo[2]=ti;

	ti.nPmtRow=2; ti.fLGuideWidth=5.0*CLHEP::mm; ti.fPerpShiftInPixel=0.0*CLHEP::mm;
	ti.bUseTaper=false; ti.fTaperAngle=0.0*CLHEP::deg; ti.fTaperOffset=0.0*CLHEP::mm;
	ti.fLength=-1.0*CLHEP::mm;
	mapTrainInfo[3]=ti;

	ti.nPmtRow=1; ti.fLGuideWidth=5.5*CLHEP::mm; ti.fPerpShiftInPixel=0.25*CLHEP::mm;
	ti.bUseTaper=false; ti.fTaperAngle=0.0*CLHEP::deg; ti.fTaperOffset=0.0*CLHEP::mm;
	ti.fLength=-1.0*CLHEP::mm;
	mapTrainInfo[4]=ti;
}
