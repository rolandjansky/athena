/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

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



AFP_Geometry::AFP_Geometry(const AFP_CONFIGURATION* pCfgParams)
{
    m_CfgParams=*pCfgParams;

    setupLBarsDims(EAS_AFP00);
    setupLBarsDims(EAS_AFP03);

    for(int nStationID=0; nStationID<=3; ++nStationID){
        eAFPStation eStation=(eAFPStation)nStationID;
        for(int nPlateID=0; nPlateID<getSIDPlatesCnt(eStation); ++nPlateID){
            if(!(m_MapSIDTransToGlobal.find(eStation)!=m_MapSIDTransToGlobal.end() && m_MapSIDTransToGlobal[eStation].find(nPlateID)!=m_MapSIDTransToGlobal[eStation].end()))
            {
                char szStationName[8];
                sprintf(szStationName,"AFP%02i",(int)eStation);
                HepGeom::Transform3D Aux=getSIDTransform(ESTT_SENSORGLOBAL,szStationName,nPlateID);
                m_MapSIDTransToGlobal[eStation][nPlateID]=Aux;
            }

            if(!(m_MapSIDTransToLocal.find(eStation)!=m_MapSIDTransToLocal.end() && m_MapSIDTransToLocal[eStation].find(nPlateID)!=m_MapSIDTransToLocal[eStation].end()))
            {
                char szStationName[8];
                sprintf(szStationName,"AFP%02i",(int)eStation);
                HepGeom::Transform3D Aux=getSIDTransform(ESTT_SENSORLOCAL,szStationName,nPlateID);
                m_MapSIDTransToLocal[eStation][nPlateID]=Aux;
            }
        }
    }
}

AFP_Geometry::~AFP_Geometry()
{
    m_CfgParams.clear();
}

HepGeom::Transform3D AFP_Geometry::getStationTransform(const char* pszStationName) const
{
    HepGeom::Transform3D ReqTransform;
    eAFPStation eStation=parseStationName(pszStationName);

    switch(eStation)
    {
    case EAS_AFP00:
	ReqTransform=HepGeom::Translate3D(LHCXOFFSET, 0.0*CLHEP::mm, m_CfgParams.vecStatNominalZPos[0]);
        break;
    case EAS_AFP01:
	ReqTransform=HepGeom::Translate3D(LHCXOFFSET, 0.0*CLHEP::mm, m_CfgParams.vecStatNominalZPos[1]);
        break;
    case EAS_AFP02:
	ReqTransform=HepGeom::Translate3D(LHCXOFFSET, 0.0*CLHEP::mm, m_CfgParams.vecStatNominalZPos[2]) * HepGeom::RotateX3D(180*CLHEP::deg);
        break;
    case EAS_AFP03:
	ReqTransform=HepGeom::Translate3D(LHCXOFFSET, 0.0*CLHEP::mm, m_CfgParams.vecStatNominalZPos[3]) * HepGeom::RotateX3D(180*CLHEP::deg);
        break;
    default:
        break;
    }

    return ReqTransform;
}

HepGeom::Transform3D AFP_Geometry::getStationElementTransform(const char* pszStationName, eStationElement eElement, const int nPlateID) const
{
    HepGeom::Transform3D ReqTransform;
    eAFPStation eStation=parseStationName(pszStationName);

    AFP_TDCONFIGURATION tdcfg=m_CfgParams.tdcfg.at(eStation);
    AFP_SIDCONFIGURATION sidcfg=m_CfgParams.sidcfg.at(eStation);
    AFP_CONSTANTS AfpConstants;

    double xComponent = -m_CfgParams.vecRPotFloorDistance[eStation];
    double yComponent = m_CfgParams.vecRPotYPos[eStation];

    double xStag = 0.0;
    if(eElement==ESE_SID && nPlateID!=AfpConstants.Stat_GlobalVacuumSensorID)
        xStag = (nPlateID>-1) ? -sidcfg.vecXStaggering[nPlateID] : 0.0;
    
    switch(eStation)
    {
    case EAS_AFP00:
        if(eElement==ESE_RPOT)
            ReqTransform=HepGeom::Translate3D(xComponent, yComponent, 0.0);
        else if(eElement==ESE_TOF)
            ReqTransform=HepGeom::Translate3D(xComponent, yComponent, 0.0)*HepGeom::Translate3D(-tdcfg.fXFloorDistance,tdcfg.fYPosInRPot,tdcfg.fZPosInRPot-sidcfg.fZDistanceInRPot);
        else if(eElement==ESE_SID)
            ReqTransform=HepGeom::Translate3D(xComponent, yComponent, 0.0)*HepGeom::Translate3D( xStag,0.0,-sidcfg.fZDistanceInRPot);
        else {};
        break;
    case EAS_AFP01:
        if(eElement==ESE_RPOT)
            ReqTransform=HepGeom::Translate3D(xComponent, yComponent, 0.0);
        else if(eElement==ESE_SID)
            ReqTransform=HepGeom::Translate3D(xComponent, yComponent, 0.0)*HepGeom::TranslateX3D( xStag);
        else {};
        break;
    case EAS_AFP02:
        if(eElement==ESE_RPOT)
            ReqTransform=HepGeom::Translate3D(xComponent, yComponent, 0.0);
        else if(eElement==ESE_SID)
            ReqTransform=HepGeom::Translate3D(xComponent, yComponent ,0.0)*HepGeom::TranslateX3D( xStag);
        else {};
        break;
    case EAS_AFP03:
        if(eElement==ESE_RPOT)
            ReqTransform=HepGeom::Translate3D(xComponent, yComponent, 0.0);
	else if(eElement==ESE_TOF)
            ReqTransform=HepGeom::Translate3D(xComponent, yComponent, 0.0)*HepGeom::Translate3D(-tdcfg.fXFloorDistance,tdcfg.fYPosInRPot,tdcfg.fZPosInRPot-sidcfg.fZDistanceInRPot);
	else if(eElement==ESE_SID)
            ReqTransform=HepGeom::Translate3D(xComponent, yComponent, 0.0)*HepGeom::Translate3D( xStag,0.0,-sidcfg.fZDistanceInRPot);
        break;
    default:
        break;
    }

    return ReqTransform;
}

HepGeom::Transform3D AFP_Geometry::getSIDTransform(const eSIDTransformType eType, const char* pszStationName, const int nPlateID) const
{
    AFP_CONSTANTS AfpConstants;
    HepGeom::Transform3D ReqTransform=HepGeom::Transform3D();

    std::string Station=std::string(pszStationName);
    eAFPStation eStation=parseStationName(pszStationName);
    AFP_SIDCONFIGURATION sidcfg=m_CfgParams.sidcfg.at(eStation);
    double falpha = sidcfg.fSlope;

    HepGeom::Transform3D TotTransform;
    HepGeom::Transform3D TransInMotherVolume=getStationElementTransform(pszStationName,ESE_SID,nPlateID);
    HepGeom::Transform3D TransMotherInWorld=getStationTransform(pszStationName);

    //double fxm=DETXSIDE*(0.5*SID_MAINPLATEXDIM*std::cos(falpha)+SID_PLATETHICKNESS*std::sin(falpha)); double fzm=0.0;
    double fxm=-(sidcfg.vecChipXPos[nPlateID]+0.5*sidcfg.vecChipXLength[nPlateID])*std::cos(falpha); double fzm=0.0;
    double fZCorrOffset=(DETXSIDE==+1 || falpha==0)? -0:4*AfpConstants.SiT_CorrZOffset;
    HepGeom::Transform3D NominalPosInPocket=HepGeom::Translate3D(0.0*CLHEP::mm,0.0*CLHEP::mm, (fzm-fZCorrOffset));

    //staggering of sensor shift in its plane - correction to cosinus needed fo x-staggering to transform to staggering in LHC CS
    HepGeom::Transform3D TransStaggering=HepGeom::Translate3D(sidcfg.vecXStaggering[nPlateID]*std::cos(falpha),sidcfg.vecYStaggering[nPlateID], 0.0*CLHEP::mm);
    TotTransform=TransInMotherVolume*TransStaggering*NominalPosInPocket;

    HepGeom::Transform3D PlateTotTrans=TotTransform*HepGeom::Translate3D(fxm,0.0*CLHEP::mm, nPlateID*sidcfg.fLayerSpacing/std::cos(falpha))*HepGeom::RotateY3D(falpha);
    HepGeom::Transform3D TransFEI4=PlateTotTrans*HepGeom::Translate3D(sidcfg.vecChipXPos[nPlateID],sidcfg.vecChipYPos[nPlateID],0.5*AfpConstants.SiT_Plate_Main_thickness+0.5*AfpConstants.SiT_Chip_thickness)*HepGeom::RotateZ3D(sidcfg.vecChipRotAngle[nPlateID]);;
    HepGeom::Transform3D TransSID=TotTransform*HepGeom::Translate3D(fxm,0.0*CLHEP::mm, nPlateID*sidcfg.fLayerSpacing/std::cos(falpha))*
    HepGeom::Translate3D(sidcfg.vecChipXPos[nPlateID]+sidcfg.vecSensorXPos[nPlateID], sidcfg.vecChipYPos[nPlateID]+sidcfg.vecSensorYPos[nPlateID],  (0.5*AfpConstants.SiT_Plate_Main_thickness+AfpConstants.SiT_Chip_thickness+0.5*AfpConstants.SiT_Pixel_thickness))*HepGeom::RotateY3D(falpha) * HepGeom::RotateZ3D(sidcfg.vecChipRotAngle[nPlateID]);
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
        break;
    case ESTT_SENSORGLOBAL:
        ReqTransform=TotTransSIDInWorld;
        break;
    case ESTT_VACUUMSENSOR:
        if(nPlateID<=10)
        {
            ReqTransform=HepGeom::Translate3D(0.5*AfpConstants.SiT_Plate_Main_length_x-0.5*AfpConstants.SiT_Chip_length_x, 0.0*CLHEP::mm, -0.5*AfpConstants.SiT_Plate_Main_thickness-0.5*AfpConstants.Stat_GlobalVacuumSensorThickness);
            ReqTransform=TransInMotherVolume*NominalPosInPocket*HepGeom::Translate3D(fxm,0.0*CLHEP::mm,nPlateID*sidcfg.fLayerSpacing/std::cos(falpha))*HepGeom::RotateY3D(falpha)*ReqTransform;
        }
        else
        {
            if(nPlateID==AfpConstants.Stat_GlobalVacuumSensorID)
            {
                if(eStation==EAS_AFP00) ReqTransform=HepGeom::TranslateZ3D(-AfpConstants.Stat_GlobalVacuumSensorZOffset);
                else if(eStation==EAS_AFP01) ReqTransform=HepGeom::TranslateZ3D(-AfpConstants.Stat_GlobalVacuumSensorZOffset);
                else if(eStation==EAS_AFP02) ReqTransform=HepGeom::TranslateZ3D(-AfpConstants.Stat_GlobalVacuumSensorZOffset);
                else if(eStation==EAS_AFP03) ReqTransform=HepGeom::TranslateZ3D(-AfpConstants.Stat_GlobalVacuumSensorZOffset);
            }
        }
        break;
        default:
        break;
    }

    return ReqTransform;
}

eAFPStation AFP_Geometry::parseStationName(const char* pszStationName) const
{
    eAFPStation eStation=EAS_UNKNOWN;

    if(!strcmp(pszStationName,"AFP00")) eStation=EAS_AFP00;
    else if(!strcmp(pszStationName,"AFP01")) eStation=EAS_AFP01;
    else if(!strcmp(pszStationName,"AFP02")) eStation=EAS_AFP02;
    else if(!strcmp(pszStationName,"AFP03")) eStation=EAS_AFP03;
    else eStation=EAS_UNKNOWN;

    return eStation;
}

StatusCode AFP_Geometry::getPointInSIDSensorLocalCS(const int nStationID, const int nPlateID, const HepGeom::Point3D<double>& GlobalPoint, HepGeom::Point3D<double>& LocalPoint) const
{
    StatusCode Status=StatusCode::FAILURE;
    LocalPoint=HepGeom::Point3D<double>();
    AFP_CONSTANTS AfpConstants;
    eAFPStation eStation=(eAFPStation)nStationID;

    AFP_TDCONFIGURATION tdcfg=m_CfgParams.tdcfg.at(eStation);

    if(nStationID>=0 && nStationID<=3){
        if(nPlateID>=0 && nPlateID<getSIDPlatesCnt(eStation)){
            LocalPoint=m_MapSIDTransToLocal.at(eStation).at(nPlateID)*GlobalPoint-HepGeom::Point3D<double>(0.5*AfpConstants.SiT_Pixel_length_totx,0.5*AfpConstants.SiT_Pixel_length_toty,0.5*AfpConstants.SiT_Pixel_thickness);
            Status=StatusCode::SUCCESS;
        }
    }

    return Status;
}

StatusCode AFP_Geometry::getPointInSIDSensorGlobalCS(const int nStationID, const int nPlateID, const HepGeom::Point3D<double>& LocalPoint, HepGeom::Point3D<double>& GlobalPoint) const
{
    AFP_CONSTANTS AfpConstants;
    StatusCode Status=StatusCode::FAILURE;
    GlobalPoint=HepGeom::Point3D<double>();
    eAFPStation eStation=(eAFPStation)nStationID;

    if(nStationID>=0 && nStationID<=3){
        if(nPlateID>=0 && nPlateID<getSIDPlatesCnt(eStation)){
            GlobalPoint=m_MapSIDTransToGlobal.at(eStation).at(nPlateID)*static_cast<HepGeom::Point3D<double> >(LocalPoint+HepGeom::Point3D<double>(0.5*AfpConstants.SiT_Pixel_length_totx,0.5*AfpConstants.SiT_Pixel_length_toty,0.5*AfpConstants.SiT_Pixel_thickness));
            Status=StatusCode::SUCCESS;
        }
    }

    return Status;
}


//-----------------------------------------------------------------

void AFP_Geometry::getPixelLocalPosition(const eAFPStation eStation, const int nPixelID, double* pfX1Pos, double* pfX2Pos) const
{
    int i,j;
    AFP_TDCONFIGURATION TofCfg=m_CfgParams.tdcfg.at(eStation);

    i=getPixelRow(nPixelID)-1;
    j=getPixelColumn(nPixelID)-1;

    if(pfX1Pos) *pfX1Pos = -i * m_CfgParams.tdcfg.at(eStation).fPixelX1Dim;
    if(pfX2Pos) *pfX2Pos =  j * m_CfgParams.tdcfg.at(eStation).fPixelX2Dim;
}

void AFP_Geometry::setupLBarsDims(const eAFPStation eStation)
{
    int i,j,nTrainOfBar11,nRefTrainID,nTrainID,nTrainCnt;
    AFPTOF_LBARDIMENSIONS BarDims;
    double fRadLength11, fLGuideLength11, fRadLength, fLGuideLength;

    AFP_CONSTANTS AfpConstants;
    AFP_TDCONFIGURATION tdcfg=m_CfgParams.tdcfg.at(eStation);
    AFPTOF_LBARREFDIMENSIONS RefBarDims=tdcfg.RefBarDims;

    nTrainCnt=nTrainOfBar11=tdcfg.nX1PixCnt;

    if(RefBarDims.fLBarThickness>(tdcfg.fPixelX2Dim-AfpConstants.ToF_MinBarGap)) RefBarDims.fLBarThickness=tdcfg.fPixelX2Dim-AfpConstants.ToF_MinBarGap;

    //calculate dimensions for bar (1,1), move from (refx1,refx2)
    nRefTrainID = nTrainCnt-(RefBarDims.nBarX1ID-1); //reference train
    fRadLength11 = (RefBarDims.fRadLength+tdcfg.mapTrainInfo.at(nRefTrainID).fTaperOffset)+(1-RefBarDims.nBarX1ID)*tdcfg.fPixelX1Dim; // move to (refx1,1)
    fRadLength11 += (tdcfg.mapTrainInfo.at(nRefTrainID).fPerpShiftInPixel-tdcfg.mapTrainInfo.at(nTrainOfBar11).fPerpShiftInPixel)+0.5*(tdcfg.mapTrainInfo.at(nTrainOfBar11).fLGuideWidth-tdcfg.mapTrainInfo.at(nRefTrainID).fLGuideWidth);
    fRadLength11 -= ((1-RefBarDims.nBarX2ID)*tdcfg.fPixelX2Dim)/std::tan(tdcfg.fAlpha); //move to (1,1)
    for(fLGuideLength11=RefBarDims.fLGuideLength,i=RefBarDims.nBarX1ID-1;i>=1;i--){
        nTrainID = nTrainCnt-(i-1);
        fLGuideLength11 -= AfpConstants.ToF_LGuideTrainOffset+(tdcfg.mapTrainInfo.at(nTrainID).fLGuideWidth-tdcfg.mapTrainInfo.at(nTrainID).fTaperOffset);
    }

    //calculate length of light guides
    std::vector<double> vecLGLengths;
    vecLGLengths.resize(tdcfg.nX1PixCnt);
    vecLGLengths[0]=fLGuideLength11;
    for(i=2;i<=tdcfg.nX1PixCnt;i++){
        nTrainID=nTrainCnt-(i-1);
        vecLGLengths[i-1]=vecLGLengths[i-2]+AfpConstants.ToF_LGuideTrainOffset+tdcfg.mapTrainInfo.at(nTrainID+1).fLGuideWidth-tdcfg.mapTrainInfo.at(nTrainID+1).fTaperOffset;
    }

    tdcfg.mapBarDims.clear();
    for(i=0;i<tdcfg.nX1PixCnt;i++){
        nTrainID=tdcfg.nX1PixCnt-i;

        for(j=0;j<tdcfg.nX2PixCnt;j++){
            int k = j;
            if(eStation==EAS_AFP02 || eStation==EAS_AFP03){
                k = tdcfg.nX2PixCnt-1 - j;
            }
            
            fRadLength=fRadLength11+i*tdcfg.fPixelX1Dim-k*tdcfg.fPixelX2Dim/std::tan(tdcfg.fAlpha);
            fRadLength+=(tdcfg.mapTrainInfo.at(nTrainOfBar11).fPerpShiftInPixel-tdcfg.mapTrainInfo.at(nTrainID).fPerpShiftInPixel);
            fRadLength+=0.5*(tdcfg.mapTrainInfo.at(nTrainID).fLGuideWidth-tdcfg.mapTrainInfo.at(nTrainOfBar11).fLGuideWidth);
            fLGuideLength=vecLGLengths[i];

            BarDims.nBarX1ID=i+1;
            BarDims.nBarX2ID=j+1;
            BarDims.nTrainID=nTrainID;
            BarDims.fLGuideLength=fLGuideLength;
            BarDims.fLBarThickness=RefBarDims.fLBarThickness;
            BarDims.fRadLength=fRadLength-tdcfg.mapTrainInfo.at(BarDims.nTrainID).fTaperOffset;
            BarDims.fRadYDim=tdcfg.mapTrainInfo.at(nTrainID).fLGuideWidth-tdcfg.mapTrainInfo.at(BarDims.nTrainID).fTaperOffset;
            BarDims.fLGuideWidth=tdcfg.mapTrainInfo.at(nTrainID).fLGuideWidth;

            if(j==0) tdcfg.mapTrainInfo.at(nTrainCnt-i).fLength=fLGuideLength+0.5*BarDims.fRadYDim;
            tdcfg.mapBarDims[10*(i+1)+(k + 1)]=BarDims;
        }
    }
}
