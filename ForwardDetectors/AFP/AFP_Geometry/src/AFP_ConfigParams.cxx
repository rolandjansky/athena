
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

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
    deftofcfg.setDefault();
    tdcfg[EAS_AFP00]=deftofcfg;
    tdcfg[EAS_AFP01]=deftofcfg; // only stations 0 and 3 have ToF, so these shouldn't be needed
    tdcfg[EAS_AFP02]=deftofcfg; // however, the code crashes without them
    tdcfg[EAS_AFP03]=deftofcfg;

    vecRPotFloorDistance.assign(4,AFP_CONSTANTS::Stat_RPotFloorDistance);
    vecRPotYPos.assign(4,AFP_CONSTANTS::Stat_ShiftInYAxis);

    vecStatNominalZPos.resize(4);
    vecStatNominalZPos[0]=AFP_CONSTANTS::Stat_OuterZDistance;
    vecStatNominalZPos[1]=AFP_CONSTANTS::Stat_InnerZDistance;
    vecStatNominalZPos[2]=-AFP_CONSTANTS::Stat_InnerZDistance;
    vecStatNominalZPos[3]=-AFP_CONSTANTS::Stat_OuterZDistance;
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
    fSlope=AFP_CONSTANTS::SiT_Plate_rot_xz;
    fLayerCount=AFP_CONSTANTS::SiT_Plate_amount;
    fLayerSpacing=AFP_CONSTANTS::SiT_Plate_zsep;
    fZDistanceInRPot=AFP_CONSTANTS::SiT_ZDistanceInRPot;
    bAddVacuumSensors=false;

    vecXStaggering.assign(AFP_CONSTANTS::SiT_Plate_amount, AFP_CONSTANTS::SiT_DistanceToFloor);
    vecYStaggering.assign(AFP_CONSTANTS::SiT_Plate_amount, 0.0*CLHEP::mm);
    vecChipXPos.assign(AFP_CONSTANTS::SiT_Plate_amount, AFP_CONSTANTS::SiT_Chip_x);
    vecChipYPos.assign(AFP_CONSTANTS::SiT_Plate_amount, AFP_CONSTANTS::SiT_Chip_y);
    vecChipRotAngle.assign(AFP_CONSTANTS::SiT_Plate_amount, AFP_CONSTANTS::SiT_Chip_rot);
    vecChipXLength.assign(AFP_CONSTANTS::SiT_Plate_amount, AFP_CONSTANTS::SiT_Chip_length_x);
    vecChipYLength.assign(AFP_CONSTANTS::SiT_Plate_amount, AFP_CONSTANTS::SiT_Chip_length_y);
    vecSensorXPos.assign(AFP_CONSTANTS::SiT_Plate_amount, AFP_CONSTANTS::SiT_Pixel_x);
    vecSensorYPos.assign(AFP_CONSTANTS::SiT_Plate_amount, AFP_CONSTANTS::SiT_Pixel_y);


    TransInStation=HepGeom::Transform3D();
}

//-------------------------------------------------------------------------------------------------------------

void AFPTOF_LBARREFDIMENSIONS::setDefaults()
{
    nBarX1ID=4;
    nBarX2ID=2;
    fLBarThickness=6.0*CLHEP::mm;
    fRadLength=56.78*CLHEP::mm;
    fLGuideLength=71.3*CLHEP::mm;
}

void AFPTOF_LBARDIMENSIONS::setDefaults()
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
void AFP_TDCONFIGURATION::setDefault()
{
    eType=ELBT_METALELBOW;
    fAlpha=AFP_CONSTANTS::ToF_NominalSlope;
    nX1PixCnt=AFP_CONSTANTS::ToF_TrainsCnt;
    nX2PixCnt=AFP_CONSTANTS::ToF_ColumnsCnt;
    fPixelX1Dim=6.25;
    fPixelX2Dim=6.25;
    bApplyBottomCut=true;
    bEmulateImmersion=true;
    bEmulateGlue=true;

    fSkinReflectivity=0.0;
    eSReflecMode=ESRM_SINGLE;

    mapBarDims.clear();

    fXFloorDistance=AFP_CONSTANTS::ToF_DistanceToFloor;
    fYPosInRPot=-0.5*AFP_CONSTANTS::SiT_Chip_length_y;
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
