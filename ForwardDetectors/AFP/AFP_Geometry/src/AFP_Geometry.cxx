#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <algorithm>

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"
#include "PathResolver/PathResolver.h"

#include <CLHEP/Matrix/Matrix.h>
#include <CLHEP/Matrix/Vector.h>

#include "AFP_Geometry/AFP_Geometry.h"



AFP_Geometry::AFP_Geometry(const PAFP_CONFIGURATION pCfgParams)
{
    m_CfgParams=*pCfgParams;

	SetupLBarsDims(EAS_AFP00);
	SetupLBarsDims(EAS_AFP03);
}

AFP_Geometry::~AFP_Geometry()
{
    m_CfgParams.clear();
}

HepGeom::Transform3D AFP_Geometry::GetStationTransform(const char* pszStationName)
{
    HepGeom::Transform3D ReqTransform;
    eAFPStation eStation=ParseStationName(pszStationName);

    switch(eStation)
    {
    case EAS_AFP00:
		ReqTransform=HepGeom::Translate3D(LHCXOFFSET,0.0*CLHEP::mm,m_CfgParams.vecStatNominalZPos[0]);
        break;
    case EAS_AFP01:
		ReqTransform=HepGeom::Translate3D(LHCXOFFSET,0.0*CLHEP::mm,m_CfgParams.vecStatNominalZPos[1]);
        break;
    case EAS_AFP02:
		ReqTransform=HepGeom::Translate3D(LHCXOFFSET,0.0*CLHEP::mm,m_CfgParams.vecStatNominalZPos[2]);
        break;
    case EAS_AFP03:
		ReqTransform=HepGeom::Translate3D(LHCXOFFSET,0.0*CLHEP::mm,m_CfgParams.vecStatNominalZPos[3]);
        break;
    default:
        break;
    }

    return ReqTransform;
}

HepGeom::Transform3D AFP_Geometry::GetStationElementTransform(const char* pszStationName, eStationElement eElement, const int nPlateID)
{
    HepGeom::Transform3D ReqTransform;
    eAFPStation eStation=ParseStationName(pszStationName);

	AFP_TDCONFIGURATION tdcfg=m_CfgParams.tdcfg[eStation];
	AFP_SIDCONFIGURATION sidcfg=m_CfgParams.sidcfg[eStation];

    switch(eStation)
    {
    case EAS_AFP00:
        if(eElement==ESE_RPOT) ReqTransform=HepGeom::Translate3D(-m_CfgParams.vecRPotFloorDistance[0],m_CfgParams.vecRPotYPos[0],0.0);
		else if(eElement==ESE_TOF) ReqTransform=HepGeom::Translate3D(-m_CfgParams.vecRPotFloorDistance[0],m_CfgParams.vecRPotYPos[0],0.0)*HepGeom::Translate3D(-tdcfg.fXFloorDistance,tdcfg.fYPosInRPot,tdcfg.fZPosInRPot-sidcfg.fZDistanceInRPot);
		else if(eElement==ESE_SID) ReqTransform=HepGeom::Translate3D(-m_CfgParams.vecRPotFloorDistance[0],m_CfgParams.vecRPotYPos[0],0.0)*HepGeom::Translate3D(-sidcfg.vecXStaggering[nPlateID],0.0,-sidcfg.fZDistanceInRPot);
        else {};
        break;
    case EAS_AFP01:
        if(eElement==ESE_RPOT) ReqTransform=HepGeom::Translate3D(-m_CfgParams.vecRPotFloorDistance[1],m_CfgParams.vecRPotYPos[1],0.0);
		else if(eElement==ESE_SID) ReqTransform=HepGeom::Translate3D(-m_CfgParams.vecRPotFloorDistance[1],m_CfgParams.vecRPotYPos[1],0.0)*HepGeom::TranslateX3D(-sidcfg.vecXStaggering[nPlateID]);
        else {};
        break;
    case EAS_AFP02:
        if(eElement==ESE_RPOT) ReqTransform=HepGeom::Translate3D(-m_CfgParams.vecRPotFloorDistance[2],m_CfgParams.vecRPotYPos[2],0.0)*HepGeom::Reflect3D(0.0,0.0,1.0,0.0);
		else if(eElement==ESE_SID) ReqTransform=HepGeom::Translate3D(-m_CfgParams.vecRPotFloorDistance[2],m_CfgParams.vecRPotYPos[2],0.0)*HepGeom::TranslateX3D(-sidcfg.vecXStaggering[nPlateID])*HepGeom::Reflect3D(0.0,0.0,1.0,0.0);
        else {};
        break;
    case EAS_AFP03:
        if(eElement==ESE_RPOT) ReqTransform=HepGeom::Translate3D(-m_CfgParams.vecRPotFloorDistance[3],m_CfgParams.vecRPotYPos[3],0.0)*HepGeom::Reflect3D(0.0,0.0,1.0,0.0);
		else if(eElement==ESE_TOF) ReqTransform=HepGeom::Translate3D(-m_CfgParams.vecRPotFloorDistance[3],m_CfgParams.vecRPotYPos[3],0.0)*HepGeom::Translate3D(-tdcfg.fXFloorDistance,tdcfg.fYPosInRPot,tdcfg.fZPosInRPot)*HepGeom::Reflect3D(0.0,0.0,1.0,0.0);
		else if(eElement==ESE_SID) ReqTransform=HepGeom::Translate3D(-m_CfgParams.vecRPotFloorDistance[3],m_CfgParams.vecRPotYPos[3],0.0)*HepGeom::Translate3D(-sidcfg.vecXStaggering[nPlateID],0.0,sidcfg.fZDistanceInRPot)*HepGeom::Reflect3D(0.0,0.0,1.0,0.0);
        break;
    default:
        break;
    }

    return ReqTransform;
}

HepGeom::Transform3D AFP_Geometry::GetSIDTransform(const eSIDTransformType eType, const char* pszStationName, const int nPlateID)
{
	AFP_CONSTANTS AfpConstants;
    HepGeom::Transform3D ReqTransform=HepGeom::Transform3D();

    std::string Station=std::string(pszStationName);
    eAFPStation eStation=ParseStationName(pszStationName);
	AFP_SIDCONFIGURATION sidcfg=m_CfgParams.sidcfg[eStation];
	double falpha=sidcfg.fSlope;

	HepGeom::Transform3D TotTransform;
	HepGeom::Transform3D TransInMotherVolume=GetStationElementTransform(pszStationName,ESE_SID,nPlateID);
    HepGeom::Transform3D TransMotherInWorld=GetStationTransform(pszStationName);

	//double fxm=DETXSIDE*(0.5*SID_MAINPLATEXDIM*cos(falpha)+SID_PLATETHICKNESS*sin(falpha)); double fzm=0.0;
	double fxm=-(sidcfg.vecChipXPos[nPlateID]+0.5*sidcfg.vecChipXLength[nPlateID])*cos(falpha); double fzm=0.0;
    double fZCorrOffset=(DETXSIDE==+1 || falpha==0)? -0:4*SID_CORRZOFFSET;
    HepGeom::Transform3D NominalPosInPocket=HepGeom::Translate3D(0.0*CLHEP::mm,0.0*CLHEP::mm,+fzm-fZCorrOffset);

	//staggering of sensor shift in its plane - correction to cosinus needed fo x-staggering to transform to staggering in LHC CS
	HepGeom::Transform3D TransStaggering=HepGeom::Translate3D(sidcfg.vecXStaggering[nPlateID]*cos(falpha),sidcfg.vecYStaggering[nPlateID], 0.0*CLHEP::mm);
	TotTransform=TransInMotherVolume*TransStaggering*NominalPosInPocket;

	HepGeom::Transform3D PlateTotTrans=TotTransform*HepGeom::Translate3D(fxm,0.0*CLHEP::mm,nPlateID*sidcfg.fLayerSpacing/cos(falpha))*HepGeom::RotateY3D(falpha);
	HepGeom::Transform3D TransFEI4=PlateTotTrans*HepGeom::Translate3D(sidcfg.vecChipXPos[nPlateID],sidcfg.vecChipYPos[nPlateID],0.5*AfpConstants.SiT_Plate_Main_thickness+0.5*AfpConstants.SiT_Chip_thickness)*HepGeom::RotateZ3D(sidcfg.vecChipRotAngle[nPlateID]);;
	HepGeom::Transform3D TransSID=PlateTotTrans*HepGeom::Translate3D(sidcfg.vecChipXPos[nPlateID]+sidcfg.vecSensorXPos[nPlateID],sidcfg.vecChipYPos[nPlateID]+sidcfg.vecSensorYPos[nPlateID],0.5*AfpConstants.SiT_Plate_Main_thickness+AfpConstants.SiT_Chip_thickness+0.5*AfpConstants.SiT_Pixel_thickness)*HepGeom::RotateZ3D(sidcfg.vecChipRotAngle[nPlateID]);
	HepGeom::Transform3D TotTransSIDInWorld=TransMotherInWorld*TransSID;

	switch(eType)
	{
	case ESTT_PLATE:
		ReqTransform=PlateTotTrans;
		break;
	case ESTT_SENSOR:
		ReqTransform=TransSID;
		break;
	case ESTT_FEI4CHIP: //CHANGE
		ReqTransform=TransFEI4;
		break;
	case ESTT_SENSORLOCAL:
		ReqTransform=TotTransSIDInWorld.inverse();
	case ESTT_SENSORGLOBAL:
		ReqTransform=TotTransSIDInWorld;
		break;
	case ESTT_VACUUMSENSOR:
		if(nPlateID<=10)
		{
			ReqTransform=HepGeom::Translate3D(0.5*AfpConstants.SiT_Plate_Main_length_x-0.5*AfpConstants.SiT_Chip_length_x, 0.0*CLHEP::mm, -0.5*AfpConstants.SiT_Plate_Main_thickness-0.5*SID_VACUUMSENSORTHICKNESS);
			ReqTransform=TransInMotherVolume*NominalPosInPocket*HepGeom::Translate3D(fxm,0.0*CLHEP::mm,nPlateID*sidcfg.fLayerSpacing/cos(falpha))*HepGeom::RotateY3D(falpha)*ReqTransform;
		}
		else
		{
			if(nPlateID==SID_GLOBALVACUUMSENSORID)
			{
				if(eStation==EAS_AFP00) ReqTransform=HepGeom::TranslateZ3D(-SID_GLOBALVACUUMSENSORZOFFSET);
				else if(eStation==EAS_AFP01) ReqTransform=HepGeom::TranslateZ3D(-SID_GLOBALVACUUMSENSORZOFFSET);
				else if(eStation==EAS_AFP02) ReqTransform=HepGeom::TranslateZ3D(SID_GLOBALVACUUMSENSORZOFFSET);
				else if(eStation==EAS_AFP03) ReqTransform=HepGeom::TranslateZ3D(SID_GLOBALVACUUMSENSORZOFFSET);
			}
		}
		break;
		default:
		break;
	}

    return ReqTransform;
}

eAFPStation AFP_Geometry::ParseStationName(const char* pszStationName)
{
    eAFPStation eStation=EAS_UNKNOWN;

    if(!strcmp(pszStationName,"AFP00")) eStation=EAS_AFP00;
    else if(!strcmp(pszStationName,"AFP01")) eStation=EAS_AFP01;
    else if(!strcmp(pszStationName,"AFP02")) eStation=EAS_AFP02;
    else if(!strcmp(pszStationName,"AFP03")) eStation=EAS_AFP03;
    else eStation=EAS_UNKNOWN;

    return eStation;
}

StatusCode AFP_Geometry::GetPointInSIDSensorLocalCS(const int nStationID, const int nPlateID, const HepGeom::Point3D<double>& GlobalPoint, HepGeom::Point3D<double>& LocalPoint)
{
    StatusCode Status=StatusCode::FAILURE;
    LocalPoint=HepGeom::Point3D<double>();
	AFP_CONSTANTS AfpConstants;
    eAFPStation eStation=(eAFPStation)nStationID;

    if(nStationID>=0 && nStationID<=3){
		if(nPlateID>=0 && nPlateID<GetSIDPlatesCnt(eStation)){
            if(!(m_MapSIDTransToLocal.find(eStation)!=m_MapSIDTransToLocal.end() && m_MapSIDTransToLocal[eStation].find(nPlateID)!=m_MapSIDTransToLocal[eStation].end()))
            {
                char szStationName[8];
                sprintf(szStationName,"AFP%02i",(int)eStation);
                HepGeom::Transform3D Aux=GetSIDTransform(ESTT_SENSORLOCAL,szStationName,nPlateID);
                m_MapSIDTransToLocal[eStation][nPlateID]=Aux;
            }

			LocalPoint=m_MapSIDTransToLocal[eStation][nPlateID]*GlobalPoint-HepGeom::Point3D<double>(0.5*AfpConstants.SiT_Pixel_length_totx,0.5*AfpConstants.SiT_Pixel_length_toty,0.5*AfpConstants.SiT_Pixel_thickness);
            Status=StatusCode::SUCCESS;
        }
    }

    return Status;
}

StatusCode AFP_Geometry::GetPointInSIDSensorGlobalCS(const int nStationID, const int nPlateID, const HepGeom::Point3D<double>& LocalPoint, HepGeom::Point3D<double>& GlobalPoint)
{
	AFP_CONSTANTS AfpConstants;
    StatusCode Status=StatusCode::FAILURE;
    GlobalPoint=HepGeom::Point3D<double>();
    eAFPStation eStation=(eAFPStation)nStationID;

    if(nStationID>=0 && nStationID<=3){
		if(nPlateID>=0 && nPlateID<GetSIDPlatesCnt(eStation)){
            if(!(m_MapSIDTransToGlobal.find(eStation)!=m_MapSIDTransToGlobal.end() && m_MapSIDTransToGlobal[eStation].find(nPlateID)!=m_MapSIDTransToGlobal[eStation].end()))
            {
                char szStationName[8];
                sprintf(szStationName,"AFP%02i",(int)eStation);
                HepGeom::Transform3D Aux=GetSIDTransform(ESTT_SENSORGLOBAL,szStationName,nPlateID);
                m_MapSIDTransToGlobal[eStation][nPlateID]=Aux;
            }

			GlobalPoint=m_MapSIDTransToGlobal[eStation][nPlateID]*static_cast<HepGeom::Point3D<double> >(LocalPoint+HepGeom::Point3D<double>(0.5*AfpConstants.SiT_Pixel_length_totx,0.5*AfpConstants.SiT_Pixel_length_toty,0.5*AfpConstants.SiT_Pixel_thickness));
            Status=StatusCode::SUCCESS;
        }
    }

    return Status;
}


//-----------------------------------------------------------------

void AFP_Geometry::GetPixelLocalPosition(const eAFPStation eStation, const int nPixelID, double* pfX1Pos, double* pfX2Pos)
{
	int i,j;

	i=GetPixelRow(nPixelID)-1;
	j=GetPixelColumn(nPixelID)-1;

	if(pfX1Pos) *pfX1Pos=-(0.0+i)*m_CfgParams.tdcfg[eStation].fPixelX1Dim;
	if(pfX2Pos) *pfX2Pos=(0.0+j)*m_CfgParams.tdcfg[eStation].fPixelX2Dim;
}

void AFP_Geometry::SetupLBarsDims(const eAFPStation eStation)
{
	int i,j,nTrainOfBar11,nRefTrainID,nTrainID,nTrainCnt;
	AFPTOF_LBARDIMENSIONS BarDims;
	double fRadLength11, fLGuideLength11, fRadLength, fLGuideLength;

	AFP_CONSTANTS AfpConstants;
	AFP_TDCONFIGURATION TofCfg=m_CfgParams.tdcfg[eStation];
	AFPTOF_LBARREFDIMENSIONS RefBarDims=TofCfg.RefBarDims;

	nTrainCnt=nTrainOfBar11=TofCfg.nX1PixCnt;

	if(RefBarDims.fLBarThickness>(TofCfg.fPixelX2Dim-AfpConstants.ToF_MinBarGap)) RefBarDims.fLBarThickness=TofCfg.fPixelX2Dim-AfpConstants.ToF_MinBarGap;

	//calculate dimensions for bar (1,1), move from (refx1,refx2)
	nRefTrainID=nTrainCnt-(RefBarDims.nBarX1ID-1); //reference train
	fRadLength11=(RefBarDims.fRadLength+TofCfg.mapTrainInfo[nRefTrainID].fTaperOffset)+(1-RefBarDims.nBarX1ID)*TofCfg.fPixelX1Dim; // move to (refx1,1)
	fRadLength11+=(TofCfg.mapTrainInfo[nRefTrainID].fPerpShiftInPixel-TofCfg.mapTrainInfo[nTrainOfBar11].fPerpShiftInPixel)+0.5*(TofCfg.mapTrainInfo[nTrainOfBar11].fLGuideWidth-TofCfg.mapTrainInfo[nRefTrainID].fLGuideWidth);
	fRadLength11=fRadLength11-((1-RefBarDims.nBarX2ID)*TofCfg.fPixelX2Dim)/tan(TofCfg.fAlpha); //move to (1,1)
	for(fLGuideLength11=RefBarDims.fLGuideLength,i=RefBarDims.nBarX1ID-1;i>=1;i--)
	{
		nTrainID=nTrainCnt-(i-1);
		fLGuideLength11-=AfpConstants.ToF_LGuideTrainOffset+(TofCfg.mapTrainInfo[nTrainID].fLGuideWidth-TofCfg.mapTrainInfo[nTrainID].fTaperOffset);
	}

	//calculate length of light guides
	std::vector<double> vecLGLengths;
	vecLGLengths.resize(TofCfg.nX1PixCnt);
	vecLGLengths[0]=fLGuideLength11;
	for(i=2;i<=TofCfg.nX1PixCnt;i++)
	{
		nTrainID=nTrainCnt-(i-1);
		vecLGLengths[i-1]=vecLGLengths[i-2]+AfpConstants.ToF_LGuideTrainOffset+TofCfg.mapTrainInfo[nTrainID+1].fLGuideWidth-TofCfg.mapTrainInfo[nTrainID+1].fTaperOffset;
		//if(TofCfg.mapTrainInfo[nTrainID+1].bUseTaper) vecLGLengths[i]=vecLGLengths[i-1]+RefBarDims.fRadYDim+LGUIDETRAINOFFSET-TofCfg.mapTrainInfo[nTrainID+1].fTaperOffset;
		//else vecLGLengths[i]=vecLGLengths[i-1]+RefBarDims.fRadYDim+LGUIDETRAINOFFSET;
	}

	m_CfgParams.tdcfg[eStation].mapBarDims.clear();
	for(i=0;i<TofCfg.nX1PixCnt;i++)
	{
		nTrainID=4-i;

		for(j=0;j<TofCfg.nX2PixCnt;j++)
		{
			fRadLength=fRadLength11+i*TofCfg.fPixelX1Dim-j*TofCfg.fPixelX2Dim/tan(TofCfg.fAlpha);
			fRadLength+=(TofCfg.mapTrainInfo[nTrainOfBar11].fPerpShiftInPixel-TofCfg.mapTrainInfo[nTrainID].fPerpShiftInPixel);
			fRadLength+=0.5*(TofCfg.mapTrainInfo[nTrainID].fLGuideWidth-TofCfg.mapTrainInfo[nTrainOfBar11].fLGuideWidth);
			fLGuideLength=vecLGLengths[i];

			BarDims.nBarX1ID=i+1;
			BarDims.nBarX2ID=j+1;
			BarDims.nTrainID=nTrainID;
			BarDims.fLGuideLength=fLGuideLength;
			BarDims.fLBarThickness=RefBarDims.fLBarThickness;
			BarDims.fRadLength=fRadLength-TofCfg.mapTrainInfo[BarDims.nTrainID].fTaperOffset;
			BarDims.fRadYDim=TofCfg.mapTrainInfo[nTrainID].fLGuideWidth-TofCfg.mapTrainInfo[BarDims.nTrainID].fTaperOffset;
			BarDims.fLGuideWidth=TofCfg.mapTrainInfo[nTrainID].fLGuideWidth;

			if(j==0) TofCfg.mapTrainInfo[nTrainCnt-i].fLength=fLGuideLength+0.5*BarDims.fRadYDim;
			m_CfgParams.tdcfg[EAS_AFP00].mapBarDims[10*(i+1)+(j+1)]=BarDims;
		}
	}
}
