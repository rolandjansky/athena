/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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



AFP_Geometry::AFP_Geometry(const PAFP_CONFIGURATION pCfgParams)
{
    m_CfgParams=*pCfgParams;
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

HepGeom::Transform3D AFP_Geometry::GetStationElementTransform(const char* pszStationName, eStationElement eElement)
{
    HepGeom::Transform3D ReqTransform;
    eAFPStation eStation=ParseStationName(pszStationName);

    AFP_TDCONFIGURATION tdcfg=m_CfgParams.tdcfg;
    AFP_SIDCONFIGURATION sidcfg=m_CfgParams.sidcfg;

    switch(eStation)
    {
    case EAS_AFP00:
        if(eElement==ESE_RPOT) ReqTransform=HepGeom::Translate3D(-m_CfgParams.vecRPotFloorDistance[0],m_CfgParams.vecRPotYPos[0],0.0);
        else if(eElement==ESE_TOF) ReqTransform=HepGeom::Translate3D(-m_CfgParams.vecRPotFloorDistance[0],m_CfgParams.vecRPotYPos[0],0.0)*HepGeom::Translate3D(-tdcfg.fXFloorDistance,tdcfg.fYPosInRPot,tdcfg.fZPosInRPot);
        else if(eElement==ESE_SID) ReqTransform=HepGeom::Translate3D(-m_CfgParams.vecRPotFloorDistance[0],m_CfgParams.vecRPotYPos[0],0.0)*HepGeom::Translate3D(-sidcfg.fXFloorDistance,0.0,-sidcfg.fZDistanceInRPot);
        // if(eElement==ESE_HBP) ReqTransform=HepGeom::Translate3D(m_CfgParams.vecRPFloorXPos[0],m_CfgParams.vecStatYPos[0],0.0*CLHEP::mm);
        // else if(eElement==ESE_TOF) ReqTransform=HepGeom::Translate3D(m_CfgParams.vecRPFloorXPos[0]-m_CfgParams.tdcfg.fDistanceToFloor, m_CfgParams.vecStatYPos[0]+m_CfgParams.fQuarticYPosInTube[0], m_CfgParams.fQuarticZPosInTube[0]);
        // else if(eElement==ESE_SID) ReqTransform=HepGeom::Translate3D(m_CfgParams.fLongHBFloorXPos,m_CfgParams.fLongHBYPos,-200.0*CLHEP::mm);
        else {};
        break;
    case EAS_AFP01:
        if(eElement==ESE_RPOT) ReqTransform=HepGeom::Translate3D(-m_CfgParams.vecRPotFloorDistance[1],m_CfgParams.vecRPotYPos[1],0.0);
        else if(eElement==ESE_SID) ReqTransform=HepGeom::Translate3D(-m_CfgParams.vecRPotFloorDistance[1],m_CfgParams.vecRPotYPos[1],0.0)*HepGeom::TranslateX3D(-sidcfg.fXFloorDistance);
        else {};
        break;
    case EAS_AFP02:
        if(eElement==ESE_RPOT) ReqTransform=HepGeom::Translate3D(-m_CfgParams.vecRPotFloorDistance[2],m_CfgParams.vecRPotYPos[2],0.0)*HepGeom::Reflect3D(0.0,0.0,1.0,0.0);
        else if(eElement==ESE_SID) ReqTransform=HepGeom::Translate3D(-m_CfgParams.vecRPotFloorDistance[2],m_CfgParams.vecRPotYPos[2],0.0)*HepGeom::TranslateX3D(-sidcfg.fXFloorDistance)*HepGeom::Reflect3D(0.0,0.0,1.0,0.0);
        else {};
        break;
    case EAS_AFP03:
        if(eElement==ESE_RPOT) ReqTransform=HepGeom::Translate3D(-m_CfgParams.vecRPotFloorDistance[3],m_CfgParams.vecRPotYPos[3],0.0)*HepGeom::Reflect3D(0.0,0.0,1.0,0.0);
        else if(eElement==ESE_TOF) ReqTransform=HepGeom::Translate3D(-m_CfgParams.vecRPotFloorDistance[3],m_CfgParams.vecRPotYPos[3],0.0)*HepGeom::Translate3D(-tdcfg.fXFloorDistance,tdcfg.fYPosInRPot,-tdcfg.fZPosInRPot)*HepGeom::Reflect3D(0.0,0.0,1.0,0.0);
        else if(eElement==ESE_SID) ReqTransform=HepGeom::Translate3D(-m_CfgParams.vecRPotFloorDistance[3],m_CfgParams.vecRPotYPos[3],0.0)*HepGeom::Translate3D(-sidcfg.fXFloorDistance,0.0,sidcfg.fZDistanceInRPot)*HepGeom::Reflect3D(0.0,0.0,1.0,0.0);
        //if(eElement==ESE_HBP) ReqTransform=HepGeom::Translate3D(m_CfgParams.vecRPFloorXPos[3],m_CfgParams.vecStatYPos[3],0.0*CLHEP::mm)*HepGeom::Reflect3D(0.0,0.0,1.0,0.0);
        //else if(eElement==ESE_TD1) ReqTransform=HepGeom::Translate3D(m_CfgParams.fLongHBFloorXPos, m_CfgParams.fLongHBYPos+m_CfgParams.fQuarticYPosInTube[0], -m_CfgParams.fQuarticZPosInTube[0])*HepGeom::Reflect3D(0.0,0.0,1.0,0.0);
        //else if(eElement==ESE_TD2) ReqTransform=HepGeom::Translate3D(m_CfgParams.fLongHBFloorXPos, m_CfgParams.fLongHBYPos+m_CfgParams.fQuarticYPosInTube[1], -m_CfgParams.fQuarticZPosInTube[1])*HepGeom::Reflect3D(0.0,0.0,1.0,0.0);
        //else if(eElement==ESE_SID) ReqTransform=HepGeom::Translate3D(m_CfgParams.fLongHBFloorXPos,m_CfgParams.fLongHBYPos,+200.0*CLHEP::mm)*HepGeom::Reflect3D(0.0,0.0,1.0,0.0);
        break;
    default:
        break;
    }

    return ReqTransform;
}

HepGeom::Transform3D AFP_Geometry::GetSIDTransform(const eSIDTransformType eType, const char* pszStationName, const int nPlateIndex)
{
    HepGeom::Transform3D ReqTransform=HepGeom::Transform3D();

    std::string Station=std::string(pszStationName);
    eAFPStation eStation=ParseStationName(pszStationName);
    double fAux=0.0;
    double falpha=m_CfgParams.sidcfg.fSlope;
    HepGeom::Transform3D TotTransform;

    HepGeom::Transform3D TransInMotherVolume=GetStationElementTransform(pszStationName,ESE_SID);
    HepGeom::Transform3D TransMotherInWorld=GetStationTransform(pszStationName);

    bool bIsXStaggeringDefined=(m_CfgParams.sidcfg.mapXStaggering.find(Station)!=m_CfgParams.sidcfg.mapXStaggering.end());
    bool bIsYStaggeringDefined=(m_CfgParams.sidcfg.mapYStaggering.find(Station)!=m_CfgParams.sidcfg.mapYStaggering.end());

    //double fxp=2*pMainPlate->getXHalfLength()+2*pEdgePlate->getXHalfLength(); // x' - width of detector
    //double fzp=2*pMainPlate->getZHalfLength(); // z'
    //double fz1=fzp*(cos(falpha)-1/(2*cos(falpha)));
    //double fz2=fxp*sin(falpha)+fz1;
    //double fz=fz1+fz2+(nSidCnt+1)*fSpacing/cos(falpha); //width of full SID in z-axis

    double fxm=DETXSIDE*(0.5*SID_MAINPLATEXDIM*cos(falpha)+SID_PLATETHICKNESS*sin(falpha));
    //double fzm=0.5*fz-(fz2-fxm*tan(falpha)+fSpacing/cos(falpha));
    double fzm=0.0;

    double fZCorrOffset=(DETXSIDE==+1 || falpha==0)? -0:4*SID_CORRZOFFSET;
    //HepGeom::Transform3D NominalPosInPocket=HepGeom::Translate3D(DETXSIDE*m_CfgParams.sidcfg.fXFloorDistance,0.0*CLHEP::mm,+fzm-fZCorrOffset);
    HepGeom::Transform3D NominalPosInPocket=HepGeom::Translate3D(0.0*CLHEP::mm,0.0*CLHEP::mm,+fzm-fZCorrOffset);

    if(eType==ESTT_SUPPORT){
        if(nPlateIndex==1 || nPlateIndex==2){
            fAux=(nPlateIndex==1)? -m_CfgParams.sidcfg.fLayerSpacing:m_CfgParams.sidcfg.fLayerCount*m_CfgParams.sidcfg.fLayerSpacing;
            TotTransform=TransInMotherVolume*NominalPosInPocket;
            ReqTransform=TotTransform*HepGeom::Translate3D(fxm,0.0*CLHEP::mm,fAux/cos(falpha))*HepGeom::RotateY3D(falpha);
        }
    }
    else{ //plate items
        //staggering of sensor shift in its plane - correction to cosinus needed fo x-staggering to transform to staggering in LHC CS
        double fXStagger= bIsXStaggeringDefined? m_CfgParams.sidcfg.mapXStaggering[Station][nPlateIndex]*cos(falpha):0.0*CLHEP::mm;
        double fYStagger= bIsYStaggeringDefined? m_CfgParams.sidcfg.mapYStaggering[Station][nPlateIndex]:0.0*CLHEP::mm;
        HepGeom::Transform3D TransStaggering=HepGeom::Translate3D(fXStagger, fYStagger, 0.0*CLHEP::mm);
        TotTransform=TransInMotherVolume*TransStaggering*NominalPosInPocket;

        HepGeom::Transform3D PlateTotTrans=TotTransform*HepGeom::Translate3D(fxm,0.0*CLHEP::mm,nPlateIndex*m_CfgParams.sidcfg.fLayerSpacing/cos(falpha))*HepGeom::RotateY3D(falpha);
        HepGeom::Transform3D TransSID=PlateTotTrans*HepGeom::Translate3D(DETXSIDE*(-0.5*SID_MAINPLATEXDIM+0.5*SID_SENSORXDIM),0.0*CLHEP::mm,0.5*SID_PLATETHICKNESS+0.5*SID_SENSORTHICKNESS);
        HepGeom::Transform3D TotTransSIDInWorld=TransMotherInWorld*TransSID;

        switch(eType)
        {
        case ESTT_PLATE:
            ReqTransform=PlateTotTrans;
            break;
        case ESTT_SENSOR:
            ReqTransform=TransSID;
            break;
        case ESTT_SENSORLOCAL:
            ReqTransform=TotTransSIDInWorld.inverse();
            break;
        case ESTT_SENSORGLOBAL:
            ReqTransform=TotTransSIDInWorld;
            break;
        case ESTT_VACUUMSENSOR:
            if(nPlateIndex<=10){
                ReqTransform=HepGeom::Translate3D(DETXSIDE*(-0.5*SID_MAINPLATEXDIM+0.5*SID_SENSORXDIM), 0.0*CLHEP::mm, -0.5*SID_PLATETHICKNESS-0.5*SID_VACUUMSENSORTHICKNESS);
                ReqTransform=TransInMotherVolume*NominalPosInPocket*HepGeom::Translate3D(fxm,0.0*CLHEP::mm,nPlateIndex*m_CfgParams.sidcfg.fLayerSpacing/cos(falpha))*HepGeom::RotateY3D(falpha)*ReqTransform;
            }
            else
            {
                if(nPlateIndex==SID_GLOBALVACUUMSENSORID)
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
    eAFPStation eStation=(eAFPStation)nStationID;

    if(nStationID>=0 && nStationID<=3){
        if(nPlateID>=0 && nPlateID<GetSIDPlatesCnt()){
            if(!(m_MapSIDTransToLocal.find(eStation)!=m_MapSIDTransToLocal.end() && m_MapSIDTransToLocal[eStation].find(nPlateID)!=m_MapSIDTransToLocal[eStation].end()))
            {
                char szStationName[8];
                sprintf(szStationName,"AFP%02i",(int)eStation);
                HepGeom::Transform3D Aux=GetSIDTransform(ESTT_SENSORLOCAL,szStationName,nPlateID);
                m_MapSIDTransToLocal[eStation][nPlateID]=Aux;
            }

            LocalPoint=m_MapSIDTransToLocal[eStation][nPlateID]*GlobalPoint-HepGeom::Point3D<double>(0.5*SID_SENSORXDIM,0.5*SID_SENSORYDIM,0.5*SID_SENSORTHICKNESS);
            Status=StatusCode::SUCCESS;
        }
    }

    return Status;
}

StatusCode AFP_Geometry::GetPointInSIDSensorGlobalCS(const int nStationID, const int nPlateID, const HepGeom::Point3D<double>& LocalPoint, HepGeom::Point3D<double>& GlobalPoint)
{
    StatusCode Status=StatusCode::FAILURE;
    GlobalPoint=HepGeom::Point3D<double>();
    eAFPStation eStation=(eAFPStation)nStationID;

    if(nStationID>=0 && nStationID<=3){
        if(nPlateID>=0 && nPlateID<GetSIDPlatesCnt()){
            if(!(m_MapSIDTransToGlobal.find(eStation)!=m_MapSIDTransToGlobal.end() && m_MapSIDTransToGlobal[eStation].find(nPlateID)!=m_MapSIDTransToGlobal[eStation].end()))
            {
                char szStationName[8];
                sprintf(szStationName,"AFP%02i",(int)eStation);
                HepGeom::Transform3D Aux=GetSIDTransform(ESTT_SENSORGLOBAL,szStationName,nPlateID);
                m_MapSIDTransToGlobal[eStation][nPlateID]=Aux;
            }

            GlobalPoint=m_MapSIDTransToGlobal[eStation][nPlateID]*static_cast<HepGeom::Point3D<double> >(LocalPoint+HepGeom::Point3D<double>(0.5*SID_SENSORXDIM,0.5*SID_SENSORYDIM,0.5*SID_SENSORTHICKNESS));
            Status=StatusCode::SUCCESS;
        }
    }

    return Status;
}
