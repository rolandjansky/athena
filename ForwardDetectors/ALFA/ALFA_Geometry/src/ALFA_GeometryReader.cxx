/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include "ALFA_Geometry/ALFA_GeometryReader.h"
#include "ALFA_Geometry/ALFA_RDBAccess.h"
#include "ALFA_Geometry/ALFA_SvdCalc.h"

void RPPINS::clear()
{
	IdealRPPin1=RealRPPin1=HepGeom::Point3D<double>();
	IdealRPPin2=RealRPPin2=HepGeom::Point3D<double>();
	IdealRPPin3=RealRPPin3=HepGeom::Point3D<double>();
	DTPInAlfaCS=HepGeom::Point3D<double>();
	DTPInRPotCS=HepGeom::Point3D<double>();
	DTPInAtlasCS=HepGeom::Point3D<double>();
}

void ASPOSPARAMS::clear()
{
	memset(szLabel,0,sizeof(szLabel));
	IdealMainPoint=HepGeom::Point3D<double>();
	ShiftE=HepGeom::Vector3D<double>();
	ShiftS=HepGeom::Vector3D<double>();
	ASTransformInMainPoint=HepGeom::Transform3D();
	ASTransformInATLAS=HepGeom::Transform3D();
}

void RPPOSPARAMS::clear()
{
	memset(szLabel,0,sizeof(szLabel));
	IdealRefPoint=HepGeom::Point3D<double>();
	IdealMainPoint=HepGeom::Point3D<double>();
	IdealMainPointInStation=HepGeom::Point3D<double>();

	VecIdealRPRefPoints.clear();
	VecRealRPRefPoints.clear();
	RPIdealTransformInStation=HepGeom::Transform3D();
	RPSWTransformInStation=HepGeom::Transform3D();
	RPTransformInStation=HepGeom::Transform3D();

	VecIdealDetRefPoints.clear();
	VecRealDetRefPoints.clear();
	DetIdealTransformInMainPoint=HepGeom::Transform3D();
	DetSWTransformInMainPoint=HepGeom::Transform3D();
	DetTransformInMainPoint=HepGeom::Transform3D();

	bIsLow=false;
	eASName=EASN_UNDEFINED;
	fCurrentLVDTmm=0.0;
	DetectorNormal=HepGeom::Vector3D<double>();

	RefPins.clear();
}

void GEOMETRYCONFIGURATION::clear()
{
	eRPMetrologyGeoType=EGST_UNDEFINED;
	strRPMetrologyConnString=std::string("");
	bShiftToX97Pos=false;

	fNominalZPosA7L1=237388*CLHEP::mm;
	fNominalZPosB7L1=241528*CLHEP::mm;
	fNominalZPosA7R1=-237408*CLHEP::mm;
	fNominalZPosB7R1=-241548*CLHEP::mm;

	for(int i=0;i<RPOTSCNT;i++){
		CfgRPosParams[i].eRPPosType=ERPPT_UNSET;
		CfgRPosParams[i].eMDGeoType=EGST_UNDEFINED;
		CfgRPosParams[i].eODGeoType=EGST_UNDEFINED;
		CfgRPosParams[i].strMDConnString=std::string("");
		CfgRPosParams[i].strODConnString=std::string("");

		CfgRPosParams[i].fCurrentLVDTmm=0.0*CLHEP::mm;

		CfgRPosParams[i].swcorr.fYOffset=(i%2==0)? 2.0*CLHEP::mm:-2.0*CLHEP::mm;
		CfgRPosParams[i].swcorr.fXOffset=0.0*CLHEP::mm;
		CfgRPosParams[i].swcorr.fTheta=0.0*CLHEP::mm;

		CfgRPosParams[i].usercorr.bIsEnabledUserTranform=false;
		CfgRPosParams[i].usercorr.UserOriginOfDetTransInRPot=HepGeom::Point3D<double>();
		CfgRPosParams[i].usercorr.UserTransformOfDetInRPot=HepGeom::Transform3D();
		CfgRPosParams[i].usercorr.UserTransformOfRPInStation=HepGeom::Transform3D();
	}
}

HepGeom::Point3D<double> ALFA_GeometryReader::ms_NominalRPPin1=HepGeom::Point3D<double>(+77.5*CLHEP::mm,+172.2*CLHEP::mm,-124.0*CLHEP::mm);
HepGeom::Point3D<double> ALFA_GeometryReader::ms_NominalRPMainPoint=HepGeom::Point3D<double>(0.0*CLHEP::mm,31.525*CLHEP::mm,8.0*CLHEP::mm);//31.025
HepGeom::Point3D<double> ALFA_GeometryReader::ms_NominalAlfaRefPoint=HepGeom::Point3D<double>(-77.5*CLHEP::mm,-35.2*CLHEP::mm,114.0*CLHEP::mm);//-35.7
HepGeom::Point3D<double> ALFA_GeometryReader::ms_NominalDetPin1=HepGeom::Point3D<double>(-77.5*CLHEP::mm,-35.0*CLHEP::mm,114.0*CLHEP::mm);

ALFA_GeometryReader::ALFA_GeometryReader()
{
    m_eFCoordSystem=EFCS_UNDEFINED;
	m_eMetrologyType=EMT_UNDEFINED;
    m_MapRPot.clear();
}

ALFA_GeometryReader::~ALFA_GeometryReader()
{
	m_MapRPot.clear();
}

void ALFA_GeometryReader::TransformFiberPositions(PFIBERPARAMS pFiberParams,eRPotName eRPName, const eFiberType eType, const eGeoSourceType eSourceType)
{
	switch(m_eFCoordSystem)
	{
	case EFCS_CLADDING:
		TransformFiberPositionsFCSCladding(pFiberParams, eRPName, eType, eSourceType);
		break;
	case EFCS_ATLAS:
		TransformFiberPositionsFCSAtlas(pFiberParams, eRPName, eType, eSourceType);
		break;
	default:
		break;
	}
}

void ALFA_GeometryReader::TransformFiberPositionsFCSCladding(PFIBERPARAMS pFiberParams,eRPotName eRPName, const eFiberType eType, const eGeoSourceType eSourceType)
{
	const double ALFA_stagger[10] = {0.0, 0.283, -0.141, 0.141, -0.283, 0.354, -0.071, 0.212, -0.212, 0.071};
	const double OD_stagger[3] = {0.0, -0.167, -0.334};
	const double fSCY=-126.3765+0.0045;
	
	const double fSCY_ODFiberU=-106.333;
	const double fSCY_ODFiberV=-113.833;
	
	const double fSCX_ODFiber00=-23.00;
	const double fSCX_ODFiber01=+23.00;

	double fStagger=0.0;

	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::TransformFiberPositions");

	switch(eType)
	{
	case EFT_UFIBER:
	case EFT_VFIBER:
		if(pFiberParams->nPlateID<1 || pFiberParams->nPlateID>10)
		{
			LogStream<<MSG::ERROR<<"Wrong PlateID "<<pFiberParams->nPlateID<<" (RP no."<<eRPName<<")"<<endmsg;
			return;
		}
		else
		{
			fStagger=ALFA_stagger[pFiberParams->nPlateID-1]*CLHEP::mm;
		}
		break;
		case EFT_ODFIBERU0:
		case EFT_ODFIBERU1:
		case EFT_ODFIBERV0:
		case EFT_ODFIBERV1:
		if(pFiberParams->nPlateID<1 || pFiberParams->nPlateID>3)
		{
			LogStream<<MSG::ERROR<<"Wrong ODPlateID "<<pFiberParams->nPlateID<<" (RP no."<<eRPName<<")"<<endmsg;
			return;
		}
		else
		{
			fStagger=OD_stagger[pFiberParams->nPlateID-1]*CLHEP::mm;
		}
		break;
		default:
		break;
	}
	
	switch(eSourceType){
	case EGST_IDEALGEOMETRY:
		if(eType==EFT_VFIBER){
			pFiberParams->fcs_cladding.fAngle=(+90*CLHEP::deg-atan(pFiberParams->fSlope))-(+45*CLHEP::deg);
			pFiberParams->fcs_cladding.fCentreXPos=(+15.75-0.5*(pFiberParams->nFiberID-1))*CLHEP::mm;

			//adjust fiber offset
			pFiberParams->fOffset=-1.414213562*pFiberParams->fcs_cladding.fCentreXPos+fSCY-fStagger;
		}
		if(eType==EFT_UFIBER){
			pFiberParams->fcs_cladding.fAngle=(-90*CLHEP::deg-atan(pFiberParams->fSlope))-(-45*CLHEP::deg);
			pFiberParams->fcs_cladding.fCentreXPos=(+15.75-0.5*(pFiberParams->nFiberID-1))*CLHEP::mm;

			//adjust fiber offset
			pFiberParams->fOffset=+1.414213562*pFiberParams->fcs_cladding.fCentreXPos+fSCY+fStagger;
		}
		if(eType==EFT_ODFIBERV0)
		{
			pFiberParams->fcs_cladding.fAngle=0*CLHEP::deg;
			pFiberParams->fcs_cladding.fCentreYPos=(-29.75+0.5*(pFiberParams->nFiberID-1))*CLHEP::mm;

			//adjust fiber offset
			pFiberParams->fOffset=pFiberParams->fcs_cladding.fCentreYPos+fSCY_ODFiberV+fStagger;
		}
		if(eType==EFT_ODFIBERU0)
		{
			pFiberParams->fcs_cladding.fAngle=0*CLHEP::deg;
			pFiberParams->fcs_cladding.fCentreYPos=(-29.75+0.5*(pFiberParams->nFiberID-16))*CLHEP::mm;

			//adjust fiber offset
			pFiberParams->fOffset=pFiberParams->fcs_cladding.fCentreYPos+fSCY_ODFiberU+fStagger;
		}
		if(eType==EFT_ODFIBERU1)
		{
			pFiberParams->fcs_cladding.fAngle=0*CLHEP::deg;
			//1.10.2010 LN: change of indexation scheme
			//pFiberParams->fcs_cladding.fCentreYPos=(-22.75-0.5*(pFiberParams->nFiberID-1))*CLHEP::mm;
			pFiberParams->fcs_cladding.fCentreYPos=(-29.75+0.5*(pFiberParams->nFiberID-16))*CLHEP::mm;

			//adjust fiber offset
			pFiberParams->fOffset=pFiberParams->fcs_cladding.fCentreYPos+fSCY_ODFiberU+fStagger;
		}
		if (eType==EFT_ODFIBERV1)
		{
			pFiberParams->fcs_cladding.fAngle=0*CLHEP::deg;
			//1.10.2010 LN: change of indexation scheme
			//pFiberParams->fcs_cladding.fCentreYPos=(-22.75-0.5*(pFiberParams->nFiberID-16))*CLHEP::mm;
			pFiberParams->fcs_cladding.fCentreYPos=(-29.75+0.5*(pFiberParams->nFiberID-1))*CLHEP::mm;

			//adjust fiber offset
			pFiberParams->fOffset=pFiberParams->fcs_cladding.fCentreYPos+fSCY_ODFiberV+fStagger;
		}
		break;
		case EGST_FILE:
		case EGST_DATABASE:
		if(eType==EFT_VFIBER){
			//Geant4: pFiberParams->fAngle=(+90*deg-atan(pFiberParams->fSlope))-(+45*deg);
			pFiberParams->fcs_cladding.fAngle=(+90*CLHEP::deg-atan(pFiberParams->fSlope))-(+45*CLHEP::deg);
			pFiberParams->fcs_cladding.fCentreXPos=1.414213562*((fStagger+fSCY-pFiberParams->fOffset)/(pFiberParams->fSlope+1)-fStagger);
		}
		if(eType==EFT_UFIBER){
			//Geant4: pFiberParams->fAngle=(-90*deg-atan(pFiberParams->fSlope))-(-45*deg);
			pFiberParams->fcs_cladding.fAngle=(-90*CLHEP::deg-atan(pFiberParams->fSlope))-(-45*CLHEP::deg);
			pFiberParams->fcs_cladding.fCentreXPos=1.414213562*((-fStagger+fSCY-pFiberParams->fOffset)/(pFiberParams->fSlope-1)-fStagger);
		}
		if(eType==EFT_ODFIBERV0)
		{
			pFiberParams->fcs_cladding.fAngle=atan(pFiberParams->fSlope);
			pFiberParams->fcs_cladding.fCentreYPos=pFiberParams->fSlope*fSCX_ODFiber00+pFiberParams->fOffset-fSCY_ODFiberV-fStagger;
		}
		if(eType==EFT_ODFIBERV1)
		{
			pFiberParams->fcs_cladding.fAngle=(-1)*atan(pFiberParams->fSlope);
			pFiberParams->fcs_cladding.fCentreYPos=pFiberParams->fSlope*fSCX_ODFiber01+pFiberParams->fOffset-fSCY_ODFiberV-fStagger;
		}
		if(eType==EFT_ODFIBERU0)
		{
			pFiberParams->fcs_cladding.fAngle=atan(pFiberParams->fSlope);
			pFiberParams->fcs_cladding.fCentreYPos=pFiberParams->fSlope*fSCX_ODFiber00+pFiberParams->fOffset-fSCY_ODFiberU-fStagger;
		}
		if(eType==EFT_ODFIBERU1)
		{
			pFiberParams->fcs_cladding.fAngle=(-1)*atan(pFiberParams->fSlope);
			pFiberParams->fcs_cladding.fCentreYPos=pFiberParams->fSlope*fSCX_ODFiber01+pFiberParams->fOffset-fSCY_ODFiberU-fStagger;
		}
		break;
		default:
		break;
	}
}

void ALFA_GeometryReader::TransformFiberPositionsFCSAtlas(PFIBERPARAMS pFiberParams,eRPotName eRPName, const eFiberType eType, const eGeoSourceType eSourceType)
{
	HepGeom::Transform3D TotTrans;
	RPPOSPARAMS RPPosParams;
	ASPOSPARAMS ASPosParams;
	GetRPPosParams(&RPPosParams, eRPName);
	GetASPosParams(&ASPosParams, RPPosParams.eASName);

	const double MD_stagger[10] = {0.0, 0.283, -0.141, 0.141, -0.283, 0.354, -0.071, 0.212, -0.212, 0.071};
	const double OD_stagger[3] = {0.0, -0.167, -0.334};
	const double fSCY=-126.3765+0.0045;
	
	const double fSCY_ODFiberU=-106.333;
	const double fSCY_ODFiberV=-113.833;
	
	//const double fSCX_ODFiber00=-23.00;
	//const double fSCX_ODFiber01=+23.00;

	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::TransformFiberPositionsFCSAtlas");

	//HepGeom::Point3D<double> RealDetRefPointInATLAS=RPPosParams.RPTransformInATLAS*RPPosParams.IdealRefPoint;
	double fCentrePos=0.0, fZOffset=0.0;
	double fStagger=0.0;

	switch(eType)
	{
	case EFT_VFIBER:
	case EFT_UFIBER:
		fStagger=MD_stagger[pFiberParams->nPlateID-1]*CLHEP::mm;

		if(eSourceType==EGST_IDEALGEOMETRY){
			if(eType==EFT_VFIBER){
				fCentrePos=(+15.75-0.5*(pFiberParams->nFiberID-1))*CLHEP::mm;
				pFiberParams->fOffset=-1.414213562*fCentrePos+fSCY-fStagger;
			}
			else if(eType==EFT_UFIBER){
				fCentrePos=(+15.75-0.5*(pFiberParams->nFiberID-1))*CLHEP::mm;
				pFiberParams->fOffset=+1.414213562*fCentrePos+fSCY+fStagger;
			}
		}

		if(RPPosParams.bIsLow==false) fZOffset=(pFiberParams->nLayerID-1)*CLHEP::mm;
		else fZOffset=-(pFiberParams->nLayerID-1)*CLHEP::mm;

		break;
		case EFT_ODFIBERV0:
		case EFT_ODFIBERV1:
		fStagger=OD_stagger[pFiberParams->nPlateID-1]*CLHEP::mm;

		if(eSourceType==EGST_IDEALGEOMETRY){
			if(eType==EFT_ODFIBERV0){
				fCentrePos=(-29.75+0.5*(pFiberParams->nFiberID-1))*CLHEP::mm;
				pFiberParams->fOffset=fCentrePos+fSCY_ODFiberV+fStagger;
			}
			else if(eType==EFT_ODFIBERV1){

				//1.10.2010 LN: change of indexation scheme
				//fCentrePos=(-22.75-0.5*(pFiberParams->nFiberID-16))*CLHEP::mm;
				fCentrePos=(-29.75+0.5*(pFiberParams->nFiberID-1))*CLHEP::mm;

				pFiberParams->fOffset=fCentrePos+fSCY_ODFiberV+fStagger;
			}
		}

		if(RPPosParams.bIsLow==false) fZOffset=2*(pFiberParams->nPlateID-1)*CLHEP::mm+33.65*CLHEP::mm;
		else fZOffset=-2*(pFiberParams->nPlateID-1)*CLHEP::mm-33.65*CLHEP::mm;
		break;
		case EFT_ODFIBERU0:
		case EFT_ODFIBERU1:
		fStagger=OD_stagger[pFiberParams->nPlateID-1]*CLHEP::mm;

		if(eSourceType==EGST_IDEALGEOMETRY){
			if(eType==EFT_ODFIBERU0){
				fCentrePos=(-29.75+0.5*(pFiberParams->nFiberID-16))*CLHEP::mm;
				pFiberParams->fOffset=fCentrePos+fSCY_ODFiberU+fStagger;
			}
			else if(eType==EFT_ODFIBERU1){

				//1.10.2010 LN: change of indexation scheme
				//fCentrePos=(-22.75-0.5*(pFiberParams->nFiberID-1))*CLHEP::mm;
				fCentrePos=(-29.75+0.5*(pFiberParams->nFiberID-16))*CLHEP::mm;

				pFiberParams->fOffset=fCentrePos+fSCY_ODFiberU+fStagger;
			}
		}

		if(RPPosParams.bIsLow==false) fZOffset=2*(pFiberParams->nPlateID-1)*CLHEP::mm+32.35*CLHEP::mm;
		else fZOffset=-2*(pFiberParams->nPlateID-1)*CLHEP::mm-32.35*CLHEP::mm;
		break;
	default:
		break;
	}

	HepGeom::Point3D<double> DetFiberPoint1, DetFiberPoint2, DetPin1Point;
	HepGeom::Point3D<double> AtlFiberPoint1, AtlFiberPoint2, AtlDetPin1Point;
	HepGeom::Point3D<double> AtlProjFiberPoint1, AtlProjFiberPoint2;

	DetPin1Point=HepGeom::Point3D<double>(0.0,0.0,0.0);
	DetFiberPoint1=HepGeom::Point3D<double>(0.0,pFiberParams->fOffset,(RPPosParams.bIsLow)? -fZOffset:+fZOffset);
	DetFiberPoint2=HepGeom::Point3D<double>(-pFiberParams->fOffset/pFiberParams->fSlope,0.0,(RPPosParams.bIsLow)? -fZOffset:+fZOffset);

	AtlDetPin1Point=GetDetPointInAtlas(eRPName,AtlDetPin1Point);
	AtlFiberPoint1=GetDetPointInAtlas(eRPName,DetFiberPoint1);
	AtlFiberPoint2=GetDetPointInAtlas(eRPName,DetFiberPoint2);

	AtlProjFiberPoint1=HepGeom::Point3D<double>(AtlFiberPoint1[0],AtlFiberPoint1[1],AtlDetPin1Point[2]+fZOffset);
	AtlProjFiberPoint2=HepGeom::Point3D<double>(AtlFiberPoint2[0],AtlFiberPoint2[1],AtlDetPin1Point[2]+fZOffset);

	double fx2=AtlProjFiberPoint1[0]+AtlProjFiberPoint1[1]*(AtlProjFiberPoint2[0]-AtlProjFiberPoint1[0])/(AtlProjFiberPoint1[1]-AtlProjFiberPoint2[1]);
	double fy1=AtlProjFiberPoint1[1]+AtlProjFiberPoint1[0]*(AtlProjFiberPoint2[1]-AtlProjFiberPoint1[1])/(AtlProjFiberPoint1[0]-AtlProjFiberPoint2[0]);

	pFiberParams->fcs_atlas.fSlope=-fy1/fx2;
	pFiberParams->fcs_atlas.fOffset=fy1;
	pFiberParams->fcs_atlas.fZPos=AtlDetPin1Point[2]+fZOffset;

	//add full space parameters
	HepGeom::Vector3D<double> DirVector=AtlFiberPoint2-AtlFiberPoint1;

	pFiberParams->fcs_atlas_full.fOriginX=AtlFiberPoint1[0];
	pFiberParams->fcs_atlas_full.fOriginY=AtlFiberPoint1[1];
	pFiberParams->fcs_atlas_full.fOriginZ=AtlFiberPoint1[2];
	pFiberParams->fcs_atlas_full.fDirX=DirVector[0]/DirVector.mag();
	pFiberParams->fcs_atlas_full.fDirY=DirVector[1]/DirVector.mag();
	pFiberParams->fcs_atlas_full.fDirZ=DirVector[2]/DirVector.mag();

	/* 2013-01-23: LN - old fashioned calculationfiber slope and offset in atlas cs
	//TotTrans=ASPosParams.ASTransformInATLAS*ASPosParams.ASTransformInMainPoint*RPPosParams.RPTransformInStation*RPPosParams.DetTransformInMainPoint;
	TotTrans=ASPosParams.ASTransformInATLAS*RPPosParams.RPTransformInStation*RPPosParams.DetTransformInMainPoint;

	HepGeom::Point3D<double> IdealDetRefPointInMainPoint;
	if(RPPosParams.bIsLow==false) {
		IdealDetRefPointInMainPoint=HepGeom::Point3D<double>(RPPosParams.IdealRefPoint[0]-RPPosParams.IdealMainPoint[0],RPPosParams.IdealRefPoint[1]-RPPosParams.IdealMainPoint[1],RPPosParams.IdealRefPoint[2]+fZOffset-RPPosParams.IdealMainPoint[2]);
	}
	else{
		IdealDetRefPointInMainPoint=HepGeom::Point3D<double>(RPPosParams.IdealRefPoint[0]-RPPosParams.IdealMainPoint[0],-(RPPosParams.IdealRefPoint[1]-RPPosParams.IdealMainPoint[1]),-(RPPosParams.IdealRefPoint[2]+fZOffset-RPPosParams.IdealMainPoint[2]));
	}
	
	HepGeom::Point3D<double> RealDetRefPointInATLAS=TotTrans*IdealDetRefPointInMainPoint;

	HepGeom::Point3D<double> FiberPointX, FiberPointY;
	switch(eType)
	{
		case EFT_VFIBER:
		case EFT_UFIBER:
			FiberPointX=HepGeom::Point3D<double>(IdealDetRefPointInMainPoint[0]+0.0,IdealDetRefPointInMainPoint[1]+pFiberParams->fOffset,IdealDetRefPointInMainPoint[2]);
			FiberPointY=HepGeom::Point3D<double>(IdealDetRefPointInMainPoint[0]-pFiberParams->fOffset/pFiberParams->fSlope,IdealDetRefPointInMainPoint[1]+0.0,IdealDetRefPointInMainPoint[2]);
			break;
		case EFT_ODFIBERV0:
		case EFT_ODFIBERV1:
		case EFT_ODFIBERU0:
		case EFT_ODFIBERU1:
			FiberPointX=HepGeom::Point3D<double>(IdealDetRefPointInMainPoint[0]+0.0,IdealDetRefPointInMainPoint[1]+pFiberParams->fOffset,IdealDetRefPointInMainPoint[2]);
			FiberPointY=HepGeom::Point3D<double>(IdealDetRefPointInMainPoint[0]+100.0,IdealDetRefPointInMainPoint[1]+100.0*pFiberParams->fSlope+pFiberParams->fOffset,IdealDetRefPointInMainPoint[2]);
			break;
		default:
			break;
	}

	HepGeom::Point3D<double> RealFiberPointXInATLAS=TotTrans*FiberPointX;
	HepGeom::Point3D<double> RealFiberPointYInATLAS=TotTrans*FiberPointY;
	HepGeom::Point3D<double> RealProjectedFiberPointXInATLAS=HepGeom::Point3D<double>(RealFiberPointXInATLAS[0],RealFiberPointXInATLAS[1],RealDetRefPointInATLAS[2]);
	HepGeom::Point3D<double> RealProjectedFiberPointYInATLAS=HepGeom::Point3D<double>(RealFiberPointYInATLAS[0],RealFiberPointYInATLAS[1],RealDetRefPointInATLAS[2]);

	pFiberParams->fcs_atlas.fSlope=(RealProjectedFiberPointYInATLAS[1]-RealProjectedFiberPointXInATLAS[1])/(RealProjectedFiberPointYInATLAS[0]-RealProjectedFiberPointXInATLAS[0]);
	//pFiberParams->fcs_atlas.fOffset=RealProjectedFiberPointXInATLAS[1]-pFiberParams->fcs_atlas.fSlope*RealProjectedFiberPointXInATLAS[0];
	HepGeom::Point3D<double> FiberPointYInAtlas=GetDetPointInAtlas(eRPName,FiberPointY);
	pFiberParams->fcs_atlas.fOffset=FiberPointYInAtlas[1];
	pFiberParams->fcs_atlas.fZPos=RealDetRefPointInATLAS[2];
	*/
}


bool ALFA_GeometryReader::InitializeDefault(const PGEOMETRYCONFIGURATION pConfig)
{
	RPPOSPARAMS RPPosParams;
	ASPOSPARAMS ASPosParams;

	m_MapRPot.clear();
	m_RPPosParams.clear();
	m_ListExistingRPots.clear();

	//double fGlobalYOffset=DETEDGEDISTANCE;
	double fYOffset;

	// ALFA Station positions
	//B7L1
	ASPosParams.clear();
	strcpy(ASPosParams.szLabel,"B7L1");
	ASPosParams.IdealMainPoint=HepGeom::Point3D<double>(0.0*CLHEP::mm,0.0*CLHEP::mm,pConfig->fNominalZPosB7L1+10.0*CLHEP::mm); //241538.0
	ASPosParams.ASTransformInATLAS=HepGeom::Translate3D(0.0*CLHEP::mm,0.0*CLHEP::mm,ASPosParams.IdealMainPoint[2]);
	m_ASPosParams.insert(std::pair<eAStationName,ASPOSPARAMS>(EASN_B7L1,ASPosParams));
	//A7L1
	ASPosParams.clear();
	strcpy(ASPosParams.szLabel,"A7L1");
	ASPosParams.IdealMainPoint=HepGeom::Point3D<double>(0.0*CLHEP::mm,0.0*CLHEP::mm,pConfig->fNominalZPosA7L1+10.0*CLHEP::mm); //237398.0
	ASPosParams.ASTransformInATLAS=HepGeom::Translate3D(0.0*CLHEP::mm,0.0*CLHEP::mm,ASPosParams.IdealMainPoint[2]);
	m_ASPosParams.insert(std::pair<eAStationName,ASPOSPARAMS>(EASN_A7L1,ASPosParams));
	//A7R1
	ASPosParams.clear();
	strcpy(ASPosParams.szLabel,"A7R1");
	ASPosParams.IdealMainPoint=HepGeom::Point3D<double>(0.0*CLHEP::mm,0.0*CLHEP::mm,pConfig->fNominalZPosA7R1+10.0*CLHEP::mm); //-237398 (old:-237418.0)
	ASPosParams.ASTransformInATLAS=HepGeom::Translate3D(0.0*CLHEP::mm,0.0*CLHEP::mm,ASPosParams.IdealMainPoint[2]);
	m_ASPosParams.insert(std::pair<eAStationName,ASPOSPARAMS>(EASN_A7R1,ASPosParams));
	//B7R1
	ASPosParams.clear();
	strcpy(ASPosParams.szLabel,"B7R1");
	ASPosParams.IdealMainPoint=HepGeom::Point3D<double>(0.0*CLHEP::mm,0.0*CLHEP::mm,pConfig->fNominalZPosB7R1+10.0*CLHEP::mm); //-241538 (old:-241558.0)
	ASPosParams.ASTransformInATLAS=HepGeom::Translate3D(0.0*CLHEP::mm,0.0*CLHEP::mm,ASPosParams.IdealMainPoint[2]);
	m_ASPosParams.insert(std::pair<eAStationName,ASPOSPARAMS>(EASN_B7R1,ASPosParams));
	
	// Roma Pot positions
	//B7L1U -1 --------------------------------------------
	RPPosParams.clear();
	strcpy(RPPosParams.szLabel,"B7L1U");
	RPPosParams.eASName=EASN_B7L1;
	RPPosParams.bIsLow=false;
	fYOffset=(pConfig->CfgRPosParams[0].eRPPosType==ERPPT_ACTIVE)? DETEDGEDISTANCE:INACTIVEDETEGDEDISTANCE;
	RPPosParams.IdealRefPoint=HepGeom::Point3D<double>(0.0*CLHEP::mm, 136.5*CLHEP::mm, m_ASPosParams[RPPosParams.eASName].IdealMainPoint[2]-10.0*CLHEP::mm); //241528.0
	RPPosParams.IdealMainPointInStation=HepGeom::Point3D<double>(0.0*CLHEP::mm, (29.525+fYOffset)*CLHEP::mm,18.0*CLHEP::mm);
	RPPosParams.IdealMainPoint=HepGeom::Point3D<double>(0.0*CLHEP::mm, (29.525+fYOffset)*CLHEP::mm, RPPosParams.IdealRefPoint[2]+18.0*CLHEP::mm);
	RPPosParams.RPIdealTransformInStation=HepGeom::Translate3D(RPPosParams.IdealMainPoint[0]-m_ASPosParams[RPPosParams.eASName].IdealMainPoint[0],
															   RPPosParams.IdealMainPoint[1]-m_ASPosParams[RPPosParams.eASName].IdealMainPoint[1],
															   RPPosParams.IdealMainPoint[2]-m_ASPosParams[RPPosParams.eASName].IdealMainPoint[2])*HepGeom::RotateX3D(RPPosParams.bIsLow? 180*CLHEP::deg:0*CLHEP::deg);
	//RPPosParams.RPTransformInStation=RPPosParams.RPIdealTransformInStation;
	RPPosParams.RPTransformInStation=HepGeom::Transform3D();
	RPPosParams.RPSWTransformInStation=HepGeom::Transform3D();
	RPPosParams.DetTransformInMainPoint=HepGeom::Transform3D();
	RPPosParams.DetIdealTransformInMainPoint=RPPosParams.DetTransformInMainPoint;
	RPPosParams.DetSWTransformInMainPoint=HepGeom::Transform3D();
	RPPosParams.RefPins.IdealRPPin1=HepGeom::Point3D<double>(+77.5*CLHEP::mm,+172.2*CLHEP::mm,-124.0*CLHEP::mm);
	RPPosParams.RefPins.IdealRPPin2=HepGeom::Point3D<double>(-77.5*CLHEP::mm,+172.2*CLHEP::mm,+124.0*CLHEP::mm);
	RPPosParams.RefPins.IdealRPPin3=HepGeom::Point3D<double>(-77.5*CLHEP::mm,+172.2*CLHEP::mm,-124.0*CLHEP::mm);
	RPPosParams.RefPins.DTPInAlfaCS=HepGeom::Point3D<double>(0.0*CLHEP::mm,-135.0*CLHEP::mm,0.0*CLHEP::mm);
	RPPosParams.RefPins.DCPInAlfaCS=HepGeom::Point3D<double>(0.0*CLHEP::mm,-135.0*CLHEP::mm,+10.0*CLHEP::mm);
	m_RPPosParams.insert(std::pair<eRPotName,RPPOSPARAMS>(ERPN_B7L1U,RPPosParams));
	
	//B7L1L -2
	RPPosParams.clear();
	strcpy(RPPosParams.szLabel,"B7L1L");
	RPPosParams.eASName=EASN_B7L1;
	RPPosParams.bIsLow=true;
	fYOffset=(pConfig->CfgRPosParams[1].eRPPosType==ERPPT_ACTIVE)? DETEDGEDISTANCE:INACTIVEDETEGDEDISTANCE;
	RPPosParams.IdealRefPoint=HepGeom::Point3D<double>(0.0*CLHEP::mm, -136.5*CLHEP::mm, m_ASPosParams[RPPosParams.eASName].IdealMainPoint[2]+10.0*CLHEP::mm); //241548.0
	RPPosParams.IdealMainPointInStation=HepGeom::Point3D<double>(0.0*CLHEP::mm, -(29.525+fYOffset)*CLHEP::mm,-18.0*CLHEP::mm);
	RPPosParams.IdealMainPoint=HepGeom::Point3D<double>(0.0*CLHEP::mm, -(29.525+fYOffset)*CLHEP::mm, RPPosParams.IdealRefPoint[2]-18.0*CLHEP::mm);
	RPPosParams.RPIdealTransformInStation=HepGeom::Translate3D(RPPosParams.IdealMainPoint[0]-m_ASPosParams[RPPosParams.eASName].IdealMainPoint[0],
															   RPPosParams.IdealMainPoint[1]-m_ASPosParams[RPPosParams.eASName].IdealMainPoint[1],
															   RPPosParams.IdealMainPoint[2]-m_ASPosParams[RPPosParams.eASName].IdealMainPoint[2])*HepGeom::RotateX3D(RPPosParams.bIsLow? 180*CLHEP::deg:0*CLHEP::deg);
	//RPPosParams.RPTransformInStation=RPPosParams.RPIdealTransformInStation;
	RPPosParams.RPTransformInStation=HepGeom::Transform3D();
	RPPosParams.RPSWTransformInStation=HepGeom::Transform3D();
	RPPosParams.DetTransformInMainPoint=HepGeom::Transform3D();
	RPPosParams.DetIdealTransformInMainPoint=RPPosParams.DetTransformInMainPoint;
	RPPosParams.DetSWTransformInMainPoint=HepGeom::Transform3D();
	RPPosParams.RefPins.IdealRPPin1=HepGeom::Point3D<double>(+77.5*CLHEP::mm,-172.2*CLHEP::mm,+124.0*CLHEP::mm);
	RPPosParams.RefPins.IdealRPPin2=HepGeom::Point3D<double>(-77.5*CLHEP::mm,-172.2*CLHEP::mm,-124.0*CLHEP::mm);
	RPPosParams.RefPins.IdealRPPin3=HepGeom::Point3D<double>(-77.5*CLHEP::mm,-172.2*CLHEP::mm,+124.0*CLHEP::mm);
	RPPosParams.RefPins.DTPInAlfaCS=HepGeom::Point3D<double>(0.0*CLHEP::mm,-135.0*CLHEP::mm,0.0*CLHEP::mm);//HepGeom::Point3D<double>(0.0*CLHEP::mm,-135.012*CLHEP::mm,0.0*CLHEP::mm);
	RPPosParams.RefPins.DCPInAlfaCS=HepGeom::Point3D<double>(0.0*CLHEP::mm,-135.0*CLHEP::mm,+10.0*CLHEP::mm);
	m_RPPosParams.insert(std::pair<eRPotName,RPPOSPARAMS>(ERPN_B7L1L,RPPosParams));
	
	//A7L1U -3 --------------------------------------------
	RPPosParams.clear();
	strcpy(RPPosParams.szLabel,"A7L1U");
	RPPosParams.eASName=EASN_A7L1;
	RPPosParams.bIsLow=false;
	fYOffset=(pConfig->CfgRPosParams[2].eRPPosType==ERPPT_ACTIVE)? DETEDGEDISTANCE:INACTIVEDETEGDEDISTANCE;
	RPPosParams.IdealRefPoint=HepGeom::Point3D<double>(0.0*CLHEP::mm, 136.5*CLHEP::mm, m_ASPosParams[RPPosParams.eASName].IdealMainPoint[2]-10.0*CLHEP::mm); //237388.0
	RPPosParams.IdealMainPointInStation=HepGeom::Point3D<double>(0.0*CLHEP::mm, (29.525+fYOffset)*CLHEP::mm,+18.0*CLHEP::mm);
	RPPosParams.IdealMainPoint=HepGeom::Point3D<double>(0.0*CLHEP::mm, (29.525+fYOffset)*CLHEP::mm, RPPosParams.IdealRefPoint[2]+18.0*CLHEP::mm);
	RPPosParams.RPIdealTransformInStation=HepGeom::Translate3D(RPPosParams.IdealMainPoint[0]-m_ASPosParams[RPPosParams.eASName].IdealMainPoint[0],
															   RPPosParams.IdealMainPoint[1]-m_ASPosParams[RPPosParams.eASName].IdealMainPoint[1],
															   RPPosParams.IdealMainPoint[2]-m_ASPosParams[RPPosParams.eASName].IdealMainPoint[2])*HepGeom::RotateX3D(RPPosParams.bIsLow? 180*CLHEP::deg:0*CLHEP::deg);
	//RPPosParams.RPTransformInStation=RPPosParams.RPIdealTransformInStation;
	RPPosParams.RPTransformInStation=HepGeom::Transform3D();
	RPPosParams.RPSWTransformInStation=HepGeom::Transform3D();
	RPPosParams.DetTransformInMainPoint=HepGeom::Transform3D();
	RPPosParams.DetIdealTransformInMainPoint=RPPosParams.DetTransformInMainPoint;
	RPPosParams.DetSWTransformInMainPoint=HepGeom::Transform3D();
	RPPosParams.RefPins.IdealRPPin1=HepGeom::Point3D<double>(+77.5*CLHEP::mm,+172.2*CLHEP::mm,-124.0*CLHEP::mm);
	RPPosParams.RefPins.IdealRPPin2=HepGeom::Point3D<double>(-77.5*CLHEP::mm,+172.2*CLHEP::mm,+124.0*CLHEP::mm);
	RPPosParams.RefPins.IdealRPPin3=HepGeom::Point3D<double>(-77.5*CLHEP::mm,+172.2*CLHEP::mm,-124.0*CLHEP::mm);
	RPPosParams.RefPins.DTPInAlfaCS=HepGeom::Point3D<double>(0.0*CLHEP::mm,-135.0*CLHEP::mm,0.0*CLHEP::mm);//HepGeom::Point3D<double>(0.0*CLHEP::mm,-135.034*CLHEP::mm,0.0*CLHEP::mm);
	RPPosParams.RefPins.DCPInAlfaCS=HepGeom::Point3D<double>(0.0*CLHEP::mm,-135.0*CLHEP::mm,+10.0*CLHEP::mm);
	m_RPPosParams.insert(std::pair<eRPotName,RPPOSPARAMS>(ERPN_A7L1U,RPPosParams));

	//A7L1L -4
	RPPosParams.clear();
	strcpy(RPPosParams.szLabel,"A7L1L");
	RPPosParams.eASName=EASN_A7L1;
	RPPosParams.bIsLow=true;
	fYOffset=(pConfig->CfgRPosParams[3].eRPPosType==ERPPT_ACTIVE)? DETEDGEDISTANCE:INACTIVEDETEGDEDISTANCE;
	RPPosParams.IdealRefPoint=HepGeom::Point3D<double>(0.0*CLHEP::mm, -136.5*CLHEP::mm, m_ASPosParams[RPPosParams.eASName].IdealMainPoint[2]+10.0*CLHEP::mm); //237408.0
	RPPosParams.IdealMainPointInStation=HepGeom::Point3D<double>(0.0*CLHEP::mm, -(29.525+fYOffset)*CLHEP::mm,-18.0*CLHEP::mm);
	RPPosParams.IdealMainPoint=HepGeom::Point3D<double>(0.0*CLHEP::mm, -(29.525+fYOffset)*CLHEP::mm, RPPosParams.IdealRefPoint[2]-18.0*CLHEP::mm);
	RPPosParams.RPIdealTransformInStation=HepGeom::Translate3D(RPPosParams.IdealMainPoint[0]-m_ASPosParams[RPPosParams.eASName].IdealMainPoint[0],
															   RPPosParams.IdealMainPoint[1]-m_ASPosParams[RPPosParams.eASName].IdealMainPoint[1],
															   RPPosParams.IdealMainPoint[2]-m_ASPosParams[RPPosParams.eASName].IdealMainPoint[2])*HepGeom::RotateX3D(RPPosParams.bIsLow? 180*CLHEP::deg:0*CLHEP::deg);
	//RPPosParams.RPTransformInStation=RPPosParams.RPIdealTransformInStation;
	RPPosParams.RPTransformInStation=HepGeom::Transform3D();
	RPPosParams.RPSWTransformInStation=HepGeom::Transform3D();
	RPPosParams.DetTransformInMainPoint=HepGeom::Transform3D();
	RPPosParams.DetIdealTransformInMainPoint=RPPosParams.DetTransformInMainPoint;
	RPPosParams.DetSWTransformInMainPoint=HepGeom::Transform3D();
	RPPosParams.RefPins.IdealRPPin1=HepGeom::Point3D<double>(+77.5*CLHEP::mm,-172.2*CLHEP::mm,+124.0*CLHEP::mm);
	RPPosParams.RefPins.IdealRPPin2=HepGeom::Point3D<double>(-77.5*CLHEP::mm,-172.2*CLHEP::mm,-124.0*CLHEP::mm);
	RPPosParams.RefPins.IdealRPPin3=HepGeom::Point3D<double>(-77.5*CLHEP::mm,-172.2*CLHEP::mm,+124.0*CLHEP::mm);
	RPPosParams.RefPins.DTPInAlfaCS=HepGeom::Point3D<double>(0.0*CLHEP::mm,-135.0*CLHEP::mm,0.0*CLHEP::mm);//HepGeom::Point3D<double>(0.0*CLHEP::mm,-134.948*CLHEP::mm,0.0*CLHEP::mm);
	RPPosParams.RefPins.DCPInAlfaCS=HepGeom::Point3D<double>(0.0*CLHEP::mm,-135.0*CLHEP::mm,+10.0*CLHEP::mm);
	m_RPPosParams.insert(std::pair<eRPotName,RPPOSPARAMS>(ERPN_A7L1L,RPPosParams));
	
	//A7R1U -5 --------------------------------------------
	RPPosParams.clear();
	strcpy(RPPosParams.szLabel,"A7R1U");
	RPPosParams.eASName=EASN_A7R1;
	RPPosParams.bIsLow=false;
	fYOffset=(pConfig->CfgRPosParams[4].eRPPosType==ERPPT_ACTIVE)? DETEDGEDISTANCE:INACTIVEDETEGDEDISTANCE;
	RPPosParams.IdealRefPoint=HepGeom::Point3D<double>(0.0*CLHEP::mm, 136.5*CLHEP::mm, m_ASPosParams[RPPosParams.eASName].IdealMainPoint[2]-10.0*CLHEP::mm); //-237408.0
	RPPosParams.IdealMainPointInStation=HepGeom::Point3D<double>(0.0*CLHEP::mm, (29.525+fYOffset)*CLHEP::mm,+18.0*CLHEP::mm);
	RPPosParams.IdealMainPoint=HepGeom::Point3D<double>(0.0*CLHEP::mm, (29.525+fYOffset)*CLHEP::mm, RPPosParams.IdealRefPoint[2]+18.0*CLHEP::mm);
	RPPosParams.RPIdealTransformInStation=HepGeom::Translate3D(RPPosParams.IdealMainPoint[0]-m_ASPosParams[RPPosParams.eASName].IdealMainPoint[0],
															   RPPosParams.IdealMainPoint[1]-m_ASPosParams[RPPosParams.eASName].IdealMainPoint[1],
															   RPPosParams.IdealMainPoint[2]-m_ASPosParams[RPPosParams.eASName].IdealMainPoint[2])*HepGeom::RotateX3D(RPPosParams.bIsLow? 180*CLHEP::deg:0*CLHEP::deg);
	//RPPosParams.RPTransformInStation=RPPosParams.RPIdealTransformInStation;
	RPPosParams.RPTransformInStation=HepGeom::Transform3D();
	RPPosParams.RPSWTransformInStation=HepGeom::Transform3D();
	RPPosParams.DetTransformInMainPoint=HepGeom::Transform3D();
	RPPosParams.DetIdealTransformInMainPoint=RPPosParams.DetTransformInMainPoint;
	RPPosParams.DetSWTransformInMainPoint=HepGeom::Transform3D();
	RPPosParams.RefPins.IdealRPPin1=HepGeom::Point3D<double>(+77.5*CLHEP::mm,+172.2*CLHEP::mm,-124.0*CLHEP::mm);
	RPPosParams.RefPins.IdealRPPin2=HepGeom::Point3D<double>(-77.5*CLHEP::mm,+172.2*CLHEP::mm,+124.0*CLHEP::mm);
	RPPosParams.RefPins.IdealRPPin3=HepGeom::Point3D<double>(-77.5*CLHEP::mm,+172.2*CLHEP::mm,-124.0*CLHEP::mm);
	RPPosParams.RefPins.DTPInAlfaCS=HepGeom::Point3D<double>(0.0*CLHEP::mm,-135.0*CLHEP::mm,0.0*CLHEP::mm);//HepGeom::Point3D<double>(0.0*CLHEP::mm,-135.08*CLHEP::mm,0.0*CLHEP::mm);
	RPPosParams.RefPins.DCPInAlfaCS=HepGeom::Point3D<double>(0.0*CLHEP::mm,-135.0*CLHEP::mm,+10.0*CLHEP::mm);
	m_RPPosParams.insert(std::pair<eRPotName,RPPOSPARAMS>(ERPN_A7R1U,RPPosParams));
	
	//A7R1L -6
	RPPosParams.clear();
	strcpy(RPPosParams.szLabel,"A7R1L");
	RPPosParams.eASName=EASN_A7R1;
	RPPosParams.bIsLow=true;
	fYOffset=(pConfig->CfgRPosParams[5].eRPPosType==ERPPT_ACTIVE)? DETEDGEDISTANCE:INACTIVEDETEGDEDISTANCE;
	RPPosParams.IdealRefPoint=HepGeom::Point3D<double>(0.0*CLHEP::mm, -136.5*CLHEP::mm, m_ASPosParams[RPPosParams.eASName].IdealMainPoint[2]+10.0*CLHEP::mm); //-237388.0
	RPPosParams.IdealMainPointInStation=HepGeom::Point3D<double>(0.0*CLHEP::mm, -(29.525+fYOffset)*CLHEP::mm,-18.0*CLHEP::mm);
	RPPosParams.IdealMainPoint=HepGeom::Point3D<double>(0.0*CLHEP::mm, -(29.525+fYOffset)*CLHEP::mm, RPPosParams.IdealRefPoint[2]-18.0*CLHEP::mm);
	RPPosParams.RPIdealTransformInStation=HepGeom::Translate3D(RPPosParams.IdealMainPoint[0]-m_ASPosParams[RPPosParams.eASName].IdealMainPoint[0],
															   RPPosParams.IdealMainPoint[1]-m_ASPosParams[RPPosParams.eASName].IdealMainPoint[1],
															   RPPosParams.IdealMainPoint[2]-m_ASPosParams[RPPosParams.eASName].IdealMainPoint[2])*HepGeom::RotateX3D(RPPosParams.bIsLow? 180*CLHEP::deg:0*CLHEP::deg);
	//RPPosParams.RPTransformInStation=RPPosParams.RPIdealTransformInStation;
	RPPosParams.RPTransformInStation=HepGeom::Transform3D();
	RPPosParams.RPSWTransformInStation=HepGeom::Transform3D();
	RPPosParams.DetTransformInMainPoint=HepGeom::Transform3D();
	RPPosParams.DetIdealTransformInMainPoint=RPPosParams.DetTransformInMainPoint;
	RPPosParams.DetSWTransformInMainPoint=HepGeom::Transform3D();
	RPPosParams.RefPins.IdealRPPin1=HepGeom::Point3D<double>(+77.5*CLHEP::mm,-172.2*CLHEP::mm,+124.0*CLHEP::mm);
	RPPosParams.RefPins.IdealRPPin2=HepGeom::Point3D<double>(-77.5*CLHEP::mm,-172.2*CLHEP::mm,-124.0*CLHEP::mm);
	RPPosParams.RefPins.IdealRPPin3=HepGeom::Point3D<double>(-77.5*CLHEP::mm,-172.2*CLHEP::mm,+124.0*CLHEP::mm);
	RPPosParams.RefPins.DTPInAlfaCS=HepGeom::Point3D<double>(0.0*CLHEP::mm,-135.0*CLHEP::mm,0.0*CLHEP::mm);//HepGeom::Point3D<double>(0.0*CLHEP::mm,-134.995*CLHEP::mm,0.0*CLHEP::mm);
	RPPosParams.RefPins.DCPInAlfaCS=HepGeom::Point3D<double>(0.0*CLHEP::mm,-135.0*CLHEP::mm,+10.0*CLHEP::mm);
	m_RPPosParams.insert(std::pair<eRPotName,RPPOSPARAMS>(ERPN_A7R1L,RPPosParams));
	
	//B7R1U -7 --------------------------------------------
	RPPosParams.clear();
	strcpy(RPPosParams.szLabel,"B7R1U");
	RPPosParams.eASName=EASN_B7R1;
	RPPosParams.bIsLow=false;
	fYOffset=(pConfig->CfgRPosParams[6].eRPPosType==ERPPT_ACTIVE)? DETEDGEDISTANCE:INACTIVEDETEGDEDISTANCE;
	RPPosParams.IdealRefPoint=HepGeom::Point3D<double>(0.0*CLHEP::mm, 136.5*CLHEP::mm, m_ASPosParams[RPPosParams.eASName].IdealMainPoint[2]-10.0*CLHEP::mm); //-241548.0
	RPPosParams.IdealMainPointInStation=HepGeom::Point3D<double>(0.0*CLHEP::mm, (29.525+fYOffset)*CLHEP::mm,+18.0*CLHEP::mm);
	RPPosParams.IdealMainPoint=HepGeom::Point3D<double>(0.0*CLHEP::mm, (29.525+fYOffset)*CLHEP::mm, RPPosParams.IdealRefPoint[2]+18.0*CLHEP::mm);
	RPPosParams.RPIdealTransformInStation=HepGeom::Translate3D(RPPosParams.IdealMainPoint[0]-m_ASPosParams[RPPosParams.eASName].IdealMainPoint[0],
															   RPPosParams.IdealMainPoint[1]-m_ASPosParams[RPPosParams.eASName].IdealMainPoint[1],
															   RPPosParams.IdealMainPoint[2]-m_ASPosParams[RPPosParams.eASName].IdealMainPoint[2])*HepGeom::RotateX3D(RPPosParams.bIsLow? 180*CLHEP::deg:0*CLHEP::deg);
	//RPPosParams.RPTransformInStation=RPPosParams.RPIdealTransformInStation;
	RPPosParams.RPTransformInStation=HepGeom::Transform3D();
	RPPosParams.RPSWTransformInStation=HepGeom::Transform3D();
	RPPosParams.DetTransformInMainPoint=HepGeom::Transform3D();
	RPPosParams.DetIdealTransformInMainPoint=RPPosParams.DetTransformInMainPoint;
	RPPosParams.DetSWTransformInMainPoint=HepGeom::Transform3D();
	RPPosParams.RefPins.IdealRPPin1=HepGeom::Point3D<double>(+77.5*CLHEP::mm,+172.2*CLHEP::mm,-124.0*CLHEP::mm);
	RPPosParams.RefPins.IdealRPPin2=HepGeom::Point3D<double>(-77.5*CLHEP::mm,+172.2*CLHEP::mm,+124.0*CLHEP::mm);
	RPPosParams.RefPins.IdealRPPin3=HepGeom::Point3D<double>(-77.5*CLHEP::mm,+172.2*CLHEP::mm,-124.0*CLHEP::mm);
	RPPosParams.RefPins.DTPInAlfaCS=HepGeom::Point3D<double>(0.0*CLHEP::mm,-135.0*CLHEP::mm,0.0*CLHEP::mm);//HepGeom::Point3D<double>(0.0*CLHEP::mm,-135.005*CLHEP::mm,0.0*CLHEP::mm);
	RPPosParams.RefPins.DCPInAlfaCS=HepGeom::Point3D<double>(0.0*CLHEP::mm,-135.0*CLHEP::mm,+10.0*CLHEP::mm);
	m_RPPosParams.insert(std::pair<eRPotName,RPPOSPARAMS>(ERPN_B7R1U,RPPosParams));
	
	//B7R1L -8
	RPPosParams.clear();
	strcpy(RPPosParams.szLabel,"B7R1L");
	RPPosParams.eASName=EASN_B7R1;
	RPPosParams.bIsLow=true;
	fYOffset=(pConfig->CfgRPosParams[7].eRPPosType==ERPPT_ACTIVE)? DETEDGEDISTANCE:INACTIVEDETEGDEDISTANCE;
	RPPosParams.IdealRefPoint=HepGeom::Point3D<double>(0.0*CLHEP::mm, -136.5*CLHEP::mm, m_ASPosParams[RPPosParams.eASName].IdealMainPoint[2]+10.0*CLHEP::mm); //-241528.0
	RPPosParams.IdealMainPointInStation=HepGeom::Point3D<double>(0.0*CLHEP::mm, -(29.525+fYOffset)*CLHEP::mm,-18.0*CLHEP::mm);
	RPPosParams.IdealMainPoint=HepGeom::Point3D<double>(0.0*CLHEP::mm, -(29.525+fYOffset)*CLHEP::mm, RPPosParams.IdealRefPoint[2]-18.0*CLHEP::mm);
	RPPosParams.RPIdealTransformInStation=HepGeom::Translate3D(RPPosParams.IdealMainPoint[0]-m_ASPosParams[RPPosParams.eASName].IdealMainPoint[0],
															   RPPosParams.IdealMainPoint[1]-m_ASPosParams[RPPosParams.eASName].IdealMainPoint[1],
															   RPPosParams.IdealMainPoint[2]-m_ASPosParams[RPPosParams.eASName].IdealMainPoint[2])*HepGeom::RotateX3D(RPPosParams.bIsLow? 180*CLHEP::deg:0*CLHEP::deg);
	//RPPosParams.RPTransformInStation=RPPosParams.RPIdealTransformInStation;
	RPPosParams.RPTransformInStation=HepGeom::Transform3D();
	RPPosParams.RPSWTransformInStation=HepGeom::Transform3D();
	RPPosParams.DetTransformInMainPoint=HepGeom::Transform3D();
	RPPosParams.DetIdealTransformInMainPoint=RPPosParams.DetTransformInMainPoint;
	RPPosParams.DetSWTransformInMainPoint=HepGeom::Transform3D();
	RPPosParams.RefPins.IdealRPPin1=HepGeom::Point3D<double>(+77.5*CLHEP::mm,-172.2*CLHEP::mm,+124.0*CLHEP::mm);
	RPPosParams.RefPins.IdealRPPin2=HepGeom::Point3D<double>(-77.5*CLHEP::mm,-172.2*CLHEP::mm,-124.0*CLHEP::mm);
	RPPosParams.RefPins.IdealRPPin3=HepGeom::Point3D<double>(-77.5*CLHEP::mm,-172.2*CLHEP::mm,+124.0*CLHEP::mm);
	RPPosParams.RefPins.DTPInAlfaCS=HepGeom::Point3D<double>(0.0*CLHEP::mm,-135.0*CLHEP::mm,0.0*CLHEP::mm);//HepGeom::Point3D<double>(0.0*CLHEP::mm,-135.021*CLHEP::mm,0.0*CLHEP::mm);
	RPPosParams.RefPins.DCPInAlfaCS=HepGeom::Point3D<double>(0.0*CLHEP::mm,-135.0*CLHEP::mm,+10.0*CLHEP::mm);
	m_RPPosParams.insert(std::pair<eRPotName,RPPOSPARAMS>(ERPN_B7R1L,RPPosParams));
	
	return true;
}

bool ALFA_GeometryReader::Initialize(const PGEOMETRYCONFIGURATION pConfig, eFiberCoordSystem eFCoordSystem)
{
	int i;
	bool bRes=true;
	std::string FilePath;
	m_eFCoordSystem=eFCoordSystem;
	
	if(pConfig!=NULL)
	{
		if(InitializeDefault(pConfig))
		{
			MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::Initialize");
			LogStream<<MSG::INFO<<"Metrology type:"<<pConfig->eRPMetrologyGeoType<<endmsg;
			LogStream<<MSG::INFO<<"Metrology source:"<<pConfig->strRPMetrologyConnString<<endmsg;

			if(pConfig->bShiftToX97Pos)
			{
				m_ASPosParams[EASN_B7L1].ASTransformInATLAS=HepGeom::TranslateX3D(-97.0*CLHEP::mm)*m_ASPosParams[EASN_B7L1].ASTransformInATLAS;
				m_ASPosParams[EASN_A7L1].ASTransformInATLAS=HepGeom::TranslateX3D(-97.0*CLHEP::mm)*m_ASPosParams[EASN_A7L1].ASTransformInATLAS;
				m_ASPosParams[EASN_A7R1].ASTransformInATLAS=HepGeom::TranslateX3D(-97.0*CLHEP::mm)*m_ASPosParams[EASN_A7R1].ASTransformInATLAS;
				m_ASPosParams[EASN_B7R1].ASTransformInATLAS=HepGeom::TranslateX3D(-97.0*CLHEP::mm)*m_ASPosParams[EASN_B7R1].ASTransformInATLAS;
			}

			m_eMetrologyType=(eMetrologyType)pConfig->eRPMetrologyGeoType;
			if(m_eMetrologyType==EMT_NOMINAL){
				SetupUserCorrections(pConfig);
			}
			else if(m_eMetrologyType==EMT_METROLOGY) {
				SetupCurrentLVDT(pConfig);

				if(pConfig->strRPMetrologyConnString==std::string("")) FilePath = PathResolver::find_file(METROLOGYFILE,"DATAPATH", PathResolver::RecursiveSearch);
				else FilePath=pConfig->strRPMetrologyConnString;
				LogStream<<MSG::INFO<<"Metrology data loaded from file "<<FilePath<<endmsg;
				bRes=ParseRPMetrology(EGST_FILE,FilePath.c_str());
				if(bRes==true) {
					UpdateStationsPosParams();
				}
				else return false;
			}
			else if(m_eMetrologyType==EMT_SWCORRECTIONS){
				SetupSWCorrections(pConfig);
			}
			else if(m_eMetrologyType==EMT_UNDEFINED){
				//do nothing, there will be no shift to regular positions
			}
			else{
				throw GaudiException(" Unknown metrology type ", "ALFA_GeometryReader::Initialize", StatusCode::FAILURE);
			}

			for(i=1;i<=RPOTSCNT;i++)
			{
				eRPotName eName=(eRPotName)i;

				if(pConfig->eRPMetrologyGeoType!=EMT_UNDEFINED) UpdateSimRPPos(eName);
				m_ListExistingRPots.push_back(eName);
			}

		}
	}
	else
	{
		m_ListExistingRPots.push_back(ERPN_B7L1U);
		m_ListExistingRPots.push_back(ERPN_B7L1L);
		m_ListExistingRPots.push_back(ERPN_A7L1U);
		m_ListExistingRPots.push_back(ERPN_A7L1L);
		m_ListExistingRPots.push_back(ERPN_A7R1U);
		m_ListExistingRPots.push_back(ERPN_A7R1L);
		m_ListExistingRPots.push_back(ERPN_B7R1U);
		m_ListExistingRPots.push_back(ERPN_B7R1L);
	}

	//2010-11-04 (LN): ReadGeometry removed and it should be called by user due to SW transformation feature
	//2012-08-03 (LN): back to call ReadGeometry because user corrections implemented to geometry consfiguration structure
	//read fiber geometry
	bRes=ReadFiberGeometry(pConfig);

	return bRes;
}

void ALFA_GeometryReader::UpdateStationsPosParams()
{
	ASPOSPARAMS Params;
	eAStationName eASName;
	HepGeom::Vector3D<double> MeanShift;

	//EASN_B7L1
	eASName=EASN_B7L1;
	GetASPosParams(&Params,eASName);
	MeanShift=0.5*(Params.ShiftE+Params.ShiftS);
	m_ASPosParams[eASName].ASTransformInMainPoint=HepGeom::Translate3D(MeanShift);

	//EASN_A7L1
	eASName=EASN_A7L1;
	GetASPosParams(&Params,eASName);
	MeanShift=0.5*(Params.ShiftE+Params.ShiftS);
	m_ASPosParams[eASName].ASTransformInMainPoint=HepGeom::Translate3D(MeanShift);

	//EASN_A7R1
	eASName=EASN_A7R1;
	GetASPosParams(&Params,eASName);
	MeanShift=0.5*(Params.ShiftE+Params.ShiftS);
	m_ASPosParams[eASName].ASTransformInMainPoint=HepGeom::Translate3D(MeanShift);

	//EASN_B7R1
	eASName=EASN_B7R1;
	GetASPosParams(&Params,eASName);
	MeanShift=0.5*(Params.ShiftE+Params.ShiftS);
	m_ASPosParams[eASName].ASTransformInMainPoint=HepGeom::Translate3D(MeanShift);
}

void ALFA_GeometryReader::UpdateSimRPPos(const eRPotName eRPName)
{
    int i, nCnt;
	HepGeom::Transform3D CorrTransform;
	HepGeom::Scale3D AuxScale;
    RPPOSPARAMS& Params=m_RPPosParams[eRPName];

	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::UpdateSimRPPos");

	//correct position of the RP in the station due to metrology data -------------------------------------------------------------------
    nCnt=Params.VecIdealRPRefPoints.size();
	HepGeom::Transform3D IdealTransform=Params.RPIdealTransformInStation;
	if(m_eMetrologyType==EMT_METROLOGY) CorrTransform=ComputeTransformMatrix(Params.VecIdealRPRefPoints,Params.VecRealRPRefPoints, nCnt, AuxScale,true);
	else CorrTransform=HepGeom::Transform3D();

	//----------------------------------------------
	//double fRotZ=atan2(CorrTransform.yx(),CorrTransform.xx());
	//HepGeom::Transform3D auxTransform=HepGeom::Transform3D(HepRotationZ(fRotZ),CorrTransform.getTranslation());
	//CorrTransform=auxTransform;
	//----------------------------------------------

	//note: SW Corrections are set either via EMT_SWCORRECTIONS flag or by user with EMT_NOMINAL flag
	m_RPPosParams[eRPName].RPTransformInStation=m_ASPosParams[Params.eASName].ASTransformInMainPoint*CorrTransform*Params.RPSWTransformInStation*IdealTransform;
	m_RPPosParams[eRPName].RPScaleInStation=AuxScale;

	m_RPPosParams[eRPName].RefPins.RealRPPin1=CorrTransform*Params.RPSWTransformInStation*m_RPPosParams[eRPName].RefPins.IdealRPPin1;
	m_RPPosParams[eRPName].RefPins.RealRPPin2=CorrTransform*Params.RPSWTransformInStation*m_RPPosParams[eRPName].RefPins.IdealRPPin2;
	m_RPPosParams[eRPName].RefPins.RealRPPin3=CorrTransform*Params.RPSWTransformInStation*m_RPPosParams[eRPName].RefPins.IdealRPPin3;

	HepGeom::Vector3D<double> NominalDetNormal=HepGeom::Vector3D<double>(0.0,0.0,1.0);
	m_RPPosParams[eRPName].DetectorNormal=m_RPPosParams[eRPName].RPTransformInStation*NominalDetNormal;

	//----------------------------------------------
	//important - false for left side, use true for right side (swapped PS points)
	//double alphaid, alphare;
	//double alpha=1000.0*atan((m_RPPosParams[eRPName].RefPins.RealRPPin3.y()-m_RPPosParams[eRPName].RefPins.RealRPPin1.y())/(m_RPPosParams[eRPName].RefPins.RealRPPin3.x()-m_RPPosParams[eRPName].RefPins.RealRPPin1.x()));
	//	bool bCond=(eRPName==ERPN_B7L1U || eRPName==ERPN_B7L1L || eRPName==ERPN_A7L1U || eRPName==ERPN_A7L1L)? false:true;
	//	if(Params.bIsLow==bCond){
	//		alphaid=1000.0*atan((Params.VecIdealRPRefPoints[2].y()-Params.VecIdealRPRefPoints[0].y())/(Params.VecIdealRPRefPoints[2].x()-Params.VecIdealRPRefPoints[0].x()));
	//		alphare=1000.0*atan((Params.VecRealRPRefPoints[2].y()-Params.VecRealRPRefPoints[0].y())/(Params.VecRealRPRefPoints[2].x()-Params.VecRealRPRefPoints[0].x()));
	//	}
	//	else{
	//		alphaid=1000.0*atan((Params.VecIdealRPRefPoints[2].y()-Params.VecIdealRPRefPoints[1].y())/(Params.VecIdealRPRefPoints[2].x()-Params.VecIdealRPRefPoints[1].x()));
	//		alphare=1000.0*atan((Params.VecRealRPRefPoints[2].y()-Params.VecRealRPRefPoints[1].y())/(Params.VecRealRPRefPoints[2].x()-Params.VecRealRPRefPoints[1].x()));
	//	}
	//----------------------------------------------

	//correct position of the RP's detector in the RP due to metrology data --------------------------------------------------------------
	IdealTransform=Params.DetIdealTransformInMainPoint;

	HepGeom::Point3D<double> RPPin1,MainPoint; //point relative to the station
	//MainPoint=HepGeom::RotateX3D(Params.bIsLow? -180.0*CLHEP::deg:0.0*CLHEP::deg)*m_RPPosParams[eRPName].RPTransformInStation*IdealTransform*HepGeom::Point3D<double>(0.0,0.0,0.0);
	//RPPin1=HepGeom::Point3D<double>(77.5,+172.2,-124.0);	//actually origin of Pot CS
	// HepGeom::Vector3D<double> CoordShift=RPPin1-MainPoint;
	RPPin1=ms_NominalRPPin1;
	MainPoint=ms_NominalRPMainPoint;

	if(m_eMetrologyType==EMT_METROLOGY){
		nCnt=Params.VecIdealDetRefPoints.size();
		std::vector<HepGeom::Point3D<double> > VecIdealDetRefPoints(nCnt);
		std::vector<HepGeom::Point3D<double> > VecRealDetRefPoints(nCnt);

		for(i=0;i<(int)Params.VecIdealDetRefPoints.size();i++){
			VecIdealDetRefPoints[i]=m_RPPosParams[eRPName].VecIdealDetRefPoints[i];//+CoordShift;
			VecRealDetRefPoints[i]=m_RPPosParams[eRPName].VecRealDetRefPoints[i];//+CoordShift;
		}

		CorrTransform=ComputeTransformMatrix(VecIdealDetRefPoints,VecRealDetRefPoints, nCnt, AuxScale);
    }
    else{
		CorrTransform=HepGeom::Transform3D();
    }

	//recalculate SW transform in MainPoint
	HepGeom::Vector3D<double> OriginShift=(RPPin1-MainPoint)+Params.OriginOfDetSWTransform;
	CLHEP::Hep3Vector TranslationInMainPoint=Params.DetSWTransform.getTranslation()-Params.DetSWTransform*OriginShift+OriginShift;
	CLHEP::HepRotation RotationInMainPoint=Params.DetSWTransform.getRotation();
	m_RPPosParams[eRPName].DetSWTransformInMainPoint=HepGeom::Transform3D(RotationInMainPoint,TranslationInMainPoint);
	//HepGeom::Transform3D AuxTrans=m_RPPosParams[eRPName].DetSWTransformInMainPoint;

	m_RPPosParams[eRPName].DetTransformInMainPoint=m_RPPosParams[eRPName].DetSWTransformInMainPoint*CorrTransform*IdealTransform;
	m_RPPosParams[eRPName].RPScaleInStation=AuxScale;

	m_RPPosParams[eRPName].RefPins.DTPInAtlasCS=GetDetPointInAtlas(eRPName,m_RPPosParams[eRPName].RefPins.DTPInAlfaCS);
	m_RPPosParams[eRPName].RefPins.DTPInRPotCS=GetDetPointInRPot(eRPName,m_RPPosParams[eRPName].RefPins.DTPInAlfaCS);
	m_RPPosParams[eRPName].RefPins.DCPInAtlasCS=GetDetPointInAtlas(eRPName,m_RPPosParams[eRPName].RefPins.DCPInAlfaCS);
	m_RPPosParams[eRPName].RefPins.DCPInRPotCS=GetDetPointInRPot(eRPName,m_RPPosParams[eRPName].RefPins.DCPInAlfaCS);
}

HepGeom::Transform3D ALFA_GeometryReader::ComputeTransformMatrix(const std::vector<HepGeom::Point3D<double> >& VecIdealRefPoints,const std::vector<HepGeom::Point3D<double> >& VecRealRefPoints, const int nPointCnt,
																 HepGeom::Scale3D& Scale, bool bForceUseSVD)
{
	int i,j;
	const int N=nPointCnt;
	CLHEP::HepVector vecAux1,vecAux2;
	CLHEP::HepVector xmean,ymean,t;
	CLHEP::HepMatrix C,U,V,R,W;
	HepGeom::Transform3D TransTot;
	HepGeom::Rotate3D AuxRot;
	HepGeom::Translate3D AuxTranslation;

	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_DetectorFactory::ReadGeometry");

	if(nPointCnt==3 && !bForceUseSVD)
	{
		HepGeom::Point3D<double> aux0=VecIdealRefPoints[0]; HepGeom::Point3D<double> aux1=VecIdealRefPoints[1]; HepGeom::Point3D<double> aux2=VecIdealRefPoints[2];
		HepGeom::Point3D<double> aux3=VecRealRefPoints[0]; HepGeom::Point3D<double> aux4=VecRealRefPoints[1]; HepGeom::Point3D<double> aux5=VecRealRefPoints[2];
		HepGeom::Transform3D AuxTrans=HepGeom::Transform3D(VecIdealRefPoints[0],VecIdealRefPoints[1],VecIdealRefPoints[2],VecRealRefPoints[0],VecRealRefPoints[1],VecRealRefPoints[2]);
		AuxTrans.getDecomposition(Scale,AuxRot,AuxTranslation);

		TransTot=HepGeom::Transform3D(AuxTrans.getRotation(),AuxTrans.getTranslation());

	}
	else
	{
		CLHEP::HepVector* pX=new CLHEP::HepVector[N];
		CLHEP::HepVector* pY=new CLHEP::HepVector[N];
		CLHEP::HepVector* pXs=new CLHEP::HepVector[N];
		CLHEP::HepVector* pYs=new CLHEP::HepVector[N];

		for(i=0;i<N;i++){
			pX[i]=CLHEP::Hep3Vector(VecIdealRefPoints[i].x(),VecIdealRefPoints[i].y(),VecIdealRefPoints[i].z());
			pY[i]=CLHEP::Hep3Vector(VecRealRefPoints[i].x(),VecRealRefPoints[i].y(),VecRealRefPoints[i].z());
		}

		/*
		pX[0]=CLHEP::Hep3Vector(1.0, 0.0, 0.0);
		pX[1]=CLHEP::Hep3Vector(0.0, 1.0, 0.0);
		pX[2]=CLHEP::Hep3Vector(0.0, 0.0, 1.0);
		
		pY[0]=CLHEP::Hep3Vector(1.7071, -9.7071, 1.0000);
		pY[1]=CLHEP::Hep3Vector(1.7071, -8.2929, 1.0000);
		pY[2]=CLHEP::Hep3Vector(1.0, -9.0, 2.0);*/
		
		vecAux1=CLHEP::Hep3Vector();
		vecAux2=CLHEP::Hep3Vector();
		for(i=0;i<N;i++){
			vecAux1+=pX[i];
			vecAux2+=pY[i];
		}
		
		xmean=vecAux1/(1.0*N);
		ymean=vecAux2/(1.0*N);
		
		C=CLHEP::HepMatrix(3,3);
		for(i=0;i<N;i++){
			pXs[i]=pX[i]-xmean;
			pYs[i]=pY[i]-ymean;
			C+=pYs[i]*pXs[i].T();
		}
		
		//LogStream<<MSG::INFO<<"C="<<C<<endmsg;
		
		double** ppfA=(double**)new char[3*sizeof(double*)];
		double** ppfV=(double**)new char[3*sizeof(double*)];
		for(i=0;i<3;i++){
			ppfA[i]=(double*)new char[3*sizeof(double)];
			memset(ppfA[i],0,3*sizeof(double));
			
			ppfV[i]=(double*)new char[3*sizeof(double)];
			memset(ppfV[i],0,3*sizeof(double));
		}
		
		double* pfW=(double*)new char[3*sizeof(double)];
		memset(pfW,0,3*sizeof(double));
		

		for(i=0;i<3;i++){
			for(j=0;j<3;j++) ppfA[i][j]=(double)C[i][j];
		}

		dsvd(ppfA,3,3,pfW,ppfV);

		U=CLHEP::HepMatrix(3,3); V=CLHEP::HepMatrix(3,3);
		for(i=0;i<3;i++){
			for(j=0;j<3;j++) U[i][j]=(double)ppfA[i][j];
			for(j=0;j<3;j++) V[i][j]=(double)ppfV[i][j];
		}
		
		W=CLHEP::HepMatrix(3,3);
		W[0][0]=1; W[1][1]=1;
		W[2][2]=(U*V).determinant();
		
		R=U*W*V.T();
		t=ymean-R*xmean;
		//LogStream<<MSG::INFO<<"U="<<U<<endmsg;
		//LogStream<<MSG::INFO<<"V="<<V<<endmsg;
		//LogStream<<MSG::INFO<<"W="<<W<<endmsg;
		//LogStream<<MSG::INFO<<"R="<<R<<endmsg;
		//LogStream<<MSG::INFO<<"t="<<t<<endmsg;

		CLHEP::HepRep3x3 matAux;
		matAux.xx_=R[0][0]; matAux.xy_=R[0][1]; matAux.xz_=R[0][2];
		matAux.yx_=R[1][0]; matAux.yy_=R[1][1]; matAux.yz_=R[1][2];
		matAux.zx_=R[2][0]; matAux.zy_=R[2][1]; matAux.zz_=R[2][2];
		CLHEP::HepRotation TransM=CLHEP::HepRotation(matAux);
		CLHEP::Hep3Vector TransT=CLHEP::Hep3Vector(t[0],t[1],t[2]);
		TransTot=HepGeom::Transform3D(TransM,TransT);

		for(i=0;i<3;i++){
			if(ppfA[i]) delete [] ppfA[i];
			if(ppfV[i]) delete [] ppfV[i];
		}
		if(ppfA) delete [] ppfA;
		if(ppfV) delete [] ppfV;
		if(pfW) delete [] pfW;

		if(pX) delete [] pX;
		if(pY) delete [] pY;
		if(pXs) delete [] pXs;
		if(pYs) delete [] pYs;
	}

	return TransTot;
}

bool ALFA_GeometryReader::ReadFiberGeometry(const PGEOMETRYCONFIGURATION pConfig)
{
	bool bFailRes=false;
	int nRPCfgIndex;
	std::list<eRPotName>::const_iterator iterRPName;
	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_DetectorReader::ReadGeometry");

	if(pConfig!=NULL)
	{
		LogStream<<MSG::INFO<<"Number of active or inactive Romain Pots: "<<m_ListExistingRPots.size()<<endmsg;
		
		for(iterRPName=m_ListExistingRPots.begin();iterRPName!=m_ListExistingRPots.end();iterRPName++){
			nRPCfgIndex=((int)(*iterRPName))-1;
			bFailRes|=!ReadSource((eGeoSourceType)pConfig->CfgRPosParams[nRPCfgIndex].eMDGeoType, *iterRPName, EFT_FIBERMD, pConfig->CfgRPosParams[nRPCfgIndex].strMDConnString.c_str());
			bFailRes|=!ReadSource((eGeoSourceType)pConfig->CfgRPosParams[nRPCfgIndex].eODGeoType, *iterRPName, EFT_FIBEROD, pConfig->CfgRPosParams[nRPCfgIndex].strODConnString.c_str());
		}
	}
	else //set ideal geometry if pConfig is not provided
	{
		for(iterRPName=m_ListExistingRPots.begin();iterRPName!=m_ListExistingRPots.end();iterRPName++){
			bFailRes|=!ReadSource(EGST_IDEALGEOMETRY, *iterRPName, EFT_FIBERMD, NULL);
			bFailRes|=!ReadSource(EGST_IDEALGEOMETRY, *iterRPName, EFT_FIBEROD, NULL);
		}
	}

	return !bFailRes;
}

bool ALFA_GeometryReader::ReadSource(const eGeoSourceType eSourceType, const eRPotName eRPName, const eFiberType eFType, const char* szDataSource)
{
	bool bRes=false;
	int i;
	std::string strDetType, FilePath, GeomFile;
	PLATEPARAMS PlateParams;
	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_DetectorReader::ReadGeometry");

	if(eFType!=EFT_FIBERMD && eFType!=EFT_FIBEROD) return false;

	strDetType=(eFType==EFT_FIBERMD)? "MD":"OD";
	
	switch(eSourceType){
	case EGST_IDEALGEOMETRY:
		LogStream<<MSG::INFO<<"The IDEAL "<<strDetType<<" fiber geometry will be loaded for RP "<<GetRPotLabel(eRPName)<<endmsg;
		bRes=SetIdealGeometry(eRPName, eFType);
		break;
	case EGST_FILE:

		GeomFile=std::string("geom_")+strDetType+std::string("_")+std::string(GetRPotLabel(eRPName))+std::string(".dat");
		if(szDataSource==NULL || !strcmp(szDataSource,"")) FilePath = PathResolver::find_file(GeomFile,"DATAPATH", PathResolver::RecursiveSearch);
		else FilePath=std::string(szDataSource);

		LogStream<<MSG::INFO<<"The "<<strDetType<<" fiber geometry will be loaded from FILE "<<FilePath.c_str()<<" for RP "<<GetRPotLabel(eRPName)<<endmsg;
		bRes=ReadFile(eRPName, eFType, FilePath.c_str());
		break;
	case EGST_DATABASE:
		LogStream<<MSG::INFO<<"The "<<strDetType<<" fiber geometry will be loaded from DATABASE for RP "<<GetRPotLabel(eRPName)<<endmsg;
		bRes=ReadDatabase(eRPName, eFType, szDataSource);
		break;
	default:
		bRes=false;
		break;
	}
	
	if(bRes==true){
		memset(&PlateParams,0,sizeof(PLATEPARAMS));
		for(i=1;i<=ALFAFIBERSCNT;i++){
			m_MapRPot[eRPName].MapPlates.insert(std::pair<int,PLATEPARAMS>(i,PlateParams));
		}

		for(i=1;i<=ODFIBERSCNT;i++){
			m_MapRPot[eRPName].MapODPlates.insert(std::pair<int,PLATEPARAMS>(i,PlateParams));
		}
		
		UpdateGeometry();
	}
	else
	{
		MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::ReadSource");
		LogStream << MSG::FATAL << " Could not load geometry! " << endmsg;
		
		throw GaudiException(" Could not load geometry ", "ALFA_GeometryReader::ReadSource", StatusCode::FAILURE);
	}

	return bRes;
}

bool ALFA_GeometryReader::SetIdealGeometry(const eRPotName eRPName, const eFiberType eFType)
{
	if(eFType!=EFT_FIBERMD && eFType!=EFT_FIBEROD) return false;
	
	int i,j;
	bool bRes=true;
	eFiberType eFiberAlfaType;
	ROMAPOT RomaPot;

	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::SetIdealGeometry");
	
	if(m_MapRPot.find(eRPName)==m_MapRPot.end())
	{
		//RP not yet defined
		RomaPot.ListUFibers.clear();
		RomaPot.ListVFibers.clear();
		RomaPot.ListODFibersU0.clear();
		RomaPot.ListODFibersU1.clear();
		RomaPot.ListODFibersV0.clear();
		RomaPot.ListODFibersV1.clear();

		m_MapRPot.insert(std::pair<eRPotName,ROMAPOT>(eRPName,RomaPot));
	}

	if(eFType==EFT_FIBERMD)	m_MapRPot[eRPName].eMDGeometryType=EGST_IDEALGEOMETRY;
	else if(eFType==EFT_FIBEROD) m_MapRPot[eRPName].eODGeometryType=EGST_IDEALGEOMETRY;

	if (eFType==EFT_FIBERMD)
	{
		for(i=1;i<=ALFALAYERSCNT*ALFAPLATESCNT;i++){
			for(j=1;j<=ALFAFIBERSCNT;j++){
                                FIBERPARAMS FiberParams;

				eFiberAlfaType=(i%2==0)? EFT_VFIBER:EFT_UFIBER;
				FiberParams.nLayerID=i;
				FiberParams.nFiberID=j;
				FiberParams.fSlope=(eFiberAlfaType==EFT_VFIBER)? 1.0:-1.0;
				FiberParams.nPlateID=FiberParams.nLayerID/2+FiberParams.nLayerID%2;
				TransformFiberPositions(&FiberParams, eRPName, eFiberAlfaType, EGST_IDEALGEOMETRY);

				if(eFiberAlfaType==EFT_UFIBER) m_MapRPot[eRPName].ListUFibers.push_back(FiberParams);
				if(eFiberAlfaType==EFT_VFIBER) m_MapRPot[eRPName].ListVFibers.push_back(FiberParams);
			}
		}
	}
	else if (eFType==EFT_FIBEROD)
	{
		for(i=1;i<=ODLAYERSCNT*ODPLATESCNT;i++)
		{
			for(j=1;j<=ODFIBERSCNT;j++)
			{
                                FIBERPARAMS FiberParams;

				FiberParams.nLayerID=i;
				FiberParams.nPlateID=FiberParams.nLayerID/2+FiberParams.nLayerID%2;
				FiberParams.fSlope = 0;

				if (i%2==0)
				{	//for V0 and V1 ODFibers
					FiberParams.nFiberID=j;
					TransformFiberPositions(&FiberParams, eRPName, EFT_ODFIBERV0, EGST_IDEALGEOMETRY);
					m_MapRPot[eRPName].ListODFibersV0.push_back(FiberParams);

					//1.10.2010 LN: change of indexation scheme
					FiberParams.nFiberID=j;//j+15;
					TransformFiberPositions(&FiberParams, eRPName, EFT_ODFIBERV1, EGST_IDEALGEOMETRY);
					m_MapRPot[eRPName].ListODFibersV1.push_back(FiberParams);
				}
				else if (i%2==1)
				{	//for U0 and U1 ODFibers
					FiberParams.nFiberID=j+15;
					TransformFiberPositions(&FiberParams, eRPName, EFT_ODFIBERU0, EGST_IDEALGEOMETRY);
					m_MapRPot[eRPName].ListODFibersU0.push_back(FiberParams);

					//1.10.2010 LN: change of indexation scheme
					FiberParams.nFiberID=j+15;//j;
					TransformFiberPositions(&FiberParams, eRPName, EFT_ODFIBERU1, EGST_IDEALGEOMETRY);
					m_MapRPot[eRPName].ListODFibersU1.push_back(FiberParams);
				}
			}
		}
	}

	return bRes;
}

bool ALFA_GeometryReader::ReadFile(const eRPotName eRPName, const eFiberType eFType, const char* szFilename)
{
	if(eFType!=EFT_FIBERMD && eFType!=EFT_FIBEROD) return false;
	
	bool bRes=true;
	int i,nLine,nLength;
	char szLine[64];
	char* pch1, *pch2;
	FILE* pFile;
	eFiberType eType=EFT_UNDEFINED;
	ROMAPOT RomaPot;

	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::ReadFile");

	if(m_MapRPot.find(eRPName)==m_MapRPot.end()){
		//RP not yet defined
		RomaPot.ListUFibers.clear();
		RomaPot.ListVFibers.clear();
		RomaPot.ListODFibersU0.clear();
		RomaPot.ListODFibersU1.clear();
		RomaPot.ListODFibersV0.clear();
		RomaPot.ListODFibersV1.clear();

		m_MapRPot.insert(std::pair<eRPotName,ROMAPOT>(eRPName,RomaPot));
	}

	if(eFType==EFT_FIBERMD)	m_MapRPot[eRPName].eMDGeometryType=EGST_FILE;
	else if(eFType==EFT_FIBEROD) m_MapRPot[eRPName].eODGeometryType=EGST_FILE;

	if((pFile=fopen(szFilename,"r"))==NULL){
		LogStream<<MSG::ERROR<< "Could not open the file "<<szFilename<<endmsg;
		return false;
	}
	
	nLine=0;
	while(!feof(pFile)){
		if(fgets(szLine,sizeof(szLine),pFile)!=NULL){

                        FIBERPARAMS FiberParams;
			
			nLine++;
			if(nLine<6) continue;
			
			nLength=strlen(szLine);
			
			//1. column - Layer ID
			for(i=0;i<nLength;i++) { if(*(szLine+i)==' ') continue; else break; }
			pch1=szLine+i; 
			pch2=strchr(pch1,' ');
			if(pch2!=NULL){
				*pch2='\0';
				FiberParams.nLayerID=atoi(pch1);
				FiberParams.nPlateID=FiberParams.nLayerID/2+FiberParams.nLayerID%2;
			}
			else{
				LogStream<<MSG::ERROR<< "Error at line "<<nLine<<" while reading the data file "<<szFilename<<endmsg;
				bRes=false;
				break;
			}
			
			//2. column - Fiber ID
			pch2++;
			for(i=0;i<nLength;i++) { if(*(pch2+i)==' ') continue; else break;}
			pch1=pch2+i; 
			pch2=strchr(pch1,' ');
			if(pch2!=NULL){
				*pch2='\0';
				FiberParams.nFiberID=atoi(pch1);
			}
			else{
				LogStream<<MSG::ERROR<< "Error at line "<<nLine<<" while reading the data file "<<szFilename<<endmsg;
				bRes=false;
				break;
			}
			
			//3. column - Slope
			pch2++;
			for(i=0;i<nLength;i++) { if(*(pch2+i)==' ') continue; else break; }
			pch1=pch2+i; 
			pch2=strchr(pch1,' ');
			if(pch2!=NULL){
				*pch2='\0';
				FiberParams.fSlope=atof(pch1);
			}
			else{
				LogStream<<MSG::ERROR<< "Error at line "<<nLine<<" while reading the data file "<<szFilename<<endmsg;
				bRes=false;
				break;
			}
			
			//4. column - Offset
			pch2++;
			for(i=0;i<nLength;i++) { if(*(pch2+i)==' ') continue; else break; }
			pch1=pch2+i; 
			pch2=strchr(pch1,' ');
			if(pch2!=NULL){
				*pch2='\0';
				FiberParams.fOffset=atof(pch1);
			}
			else{
				LogStream<<MSG::ERROR<< "Error at line "<<nLine<<" while reading the data file "<<szFilename<<endmsg;
				bRes=false;
				break;
			}
			
			//5. column - Z position
			pch2++;
			for(i=0;i<nLength;i++) { if(*(pch2+i)==' ') continue; else break; }
			pch1=pch2+i; 
			FiberParams.fZPos=atof(pch1);

			//Resolve fiber type
			if(eFType==EFT_FIBERMD)
			{
				eType=(FiberParams.fSlope<0)? EFT_UFIBER:EFT_VFIBER;
			}
			else if(eFType==EFT_FIBEROD)
			{
				if(FiberParams.nLayerID%2==1 && FiberParams.nFiberID<=15)								eType=EFT_ODFIBERV0;
				if(FiberParams.nLayerID%2==1 && FiberParams.nFiberID>=16 && FiberParams.nFiberID<=30)	eType=EFT_ODFIBERU0;

				//1.10.2010 LN: change of indexation scheme
				//if(FiberParams.nLayerID%2==0 && FiberParams.nFiberID<=15)								eType=EFT_ODFIBERU1;
				//if(FiberParams.nLayerID%2==0 && FiberParams.nFiberID>=16 && FiberParams.nFiberID<=30)	eType=EFT_ODFIBERV1;
				if(FiberParams.nLayerID%2==0 && FiberParams.nFiberID<=15)								eType=EFT_ODFIBERV1;
				if(FiberParams.nLayerID%2==0 && FiberParams.nFiberID>=16 && FiberParams.nFiberID<=30)	eType=EFT_ODFIBERU1;
			}
			
			
			//Transform & save values for RP
			TransformFiberPositions(&FiberParams, eRPName, eType, EGST_FILE);
			
			if(eType==EFT_UFIBER) m_MapRPot[eRPName].ListUFibers.push_back(FiberParams);
			if(eType==EFT_VFIBER) m_MapRPot[eRPName].ListVFibers.push_back(FiberParams);
			if(eType==EFT_ODFIBERU0) m_MapRPot[eRPName].ListODFibersU0.push_back(FiberParams);
			if(eType==EFT_ODFIBERV0) m_MapRPot[eRPName].ListODFibersV0.push_back(FiberParams);
			if(eType==EFT_ODFIBERV1) m_MapRPot[eRPName].ListODFibersV1.push_back(FiberParams);
			if(eType==EFT_ODFIBERU1) m_MapRPot[eRPName].ListODFibersU1.push_back(FiberParams);
			
		}
	}

	if(pFile) fclose(pFile);
	pFile=NULL;

	return bRes;
}

bool ALFA_GeometryReader::ReadDatabase(const eRPotName eRPName, const eFiberType eFType, const char* szDataSource)
{
	bool bRes=false;
	
	//MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::ReadDatabase");

	eFiberType eType=EFT_UNDEFINED;
	ROMAPOT RomaPot;
	
	std::vector<std::string> strDBElements;
	char szSource[64];
	char *pch;
	
	memset(szSource,0,sizeof(szSource));
	if(szDataSource) strncpy(szSource, szDataSource, sizeof(szSource)-1);
	pch = strtok(szSource,":");
	while (pch != NULL)
	{
		strDBElements.push_back(pch);
		pch = strtok(NULL, ":");
	}
	
	//	LogStream << MSG::INFO << "MARK - elements: " << strDBElements[0] << "  " << strDBElements[1] << "  " << strDBElements[2] << endmsg;
	
	ALFA_RDBAccess* p_DBAccess = new ALFA_RDBAccess();
	
	bRes = p_DBAccess->ReadGeometry(eRPName, eFType, strDBElements[2], strDBElements[1], strDBElements[0]);
	
	if(m_MapRPot.find(eRPName)==m_MapRPot.end()){
		//RP not yet defined
		RomaPot.ListUFibers.clear();
		RomaPot.ListVFibers.clear();
		RomaPot.ListODFibersU0.clear();
		RomaPot.ListODFibersU1.clear();
		RomaPot.ListODFibersV0.clear();
		RomaPot.ListODFibersV1.clear();

		m_MapRPot.insert(std::pair<eRPotName,ROMAPOT>(eRPName,RomaPot));
	}

	if(eFType==EFT_FIBERMD)	m_MapRPot[eRPName].eMDGeometryType=EGST_DATABASE;
	else if(eFType==EFT_FIBEROD) m_MapRPot[eRPName].eODGeometryType=EGST_DATABASE;

	std::list<FIBERDATA>::const_iterator iter;
	for(iter = p_DBAccess->m_ListFiberData.begin(); iter != p_DBAccess->m_ListFiberData.end(); iter++)
	{
		if (eRPName == (*iter).nPotID)
		{
                        FIBERPARAMS FiberParams;

			FiberParams.nLayerID = (*iter).nLayerID;
			FiberParams.nFiberID = (*iter).nFiberID;
			FiberParams.fSlope   = (*iter).fSlope;
			FiberParams.fOffset  = (*iter).fOffset;
			FiberParams.fZPos    = (*iter).fZPos;
			
			FiberParams.nPlateID=FiberParams.nLayerID/2+FiberParams.nLayerID%2;

			//Resolve fiber type
			if(eFType==EFT_FIBERMD)
			{
				eType=(FiberParams.fSlope<0)? EFT_UFIBER:EFT_VFIBER;
			}
			else if(eFType==EFT_FIBEROD)
			{
				if(FiberParams.nLayerID%2==1 && FiberParams.nFiberID<=15)								eType=EFT_ODFIBERV0;
				if(FiberParams.nLayerID%2==1 && FiberParams.nFiberID>=16 && FiberParams.nFiberID<=30)	eType=EFT_ODFIBERU0;
				if(FiberParams.nLayerID%2==0 && FiberParams.nFiberID<=15)								eType=EFT_ODFIBERU1;
				if(FiberParams.nLayerID%2==0 && FiberParams.nFiberID>=16 && FiberParams.nFiberID<=30)	eType=EFT_ODFIBERV1;
				
				//				LogStream << MSG::INFO << "eType: " << eType << "   fSlope: " << FiberParams.fSlope << endmsg;
			}

			//Transform & save values for RP
			TransformFiberPositions(&FiberParams, eRPName, eType, EGST_DATABASE);
			
			if(eType==EFT_UFIBER) m_MapRPot[eRPName].ListUFibers.push_back(FiberParams);
			if(eType==EFT_VFIBER) m_MapRPot[eRPName].ListVFibers.push_back(FiberParams);
			if(eType==EFT_ODFIBERU0) m_MapRPot[eRPName].ListODFibersU0.push_back(FiberParams);
			if(eType==EFT_ODFIBERV0) m_MapRPot[eRPName].ListODFibersV0.push_back(FiberParams);
			if(eType==EFT_ODFIBERV1) m_MapRPot[eRPName].ListODFibersV1.push_back(FiberParams);
			if(eType==EFT_ODFIBERU1) m_MapRPot[eRPName].ListODFibersU1.push_back(FiberParams);
		}
	}
	
	delete p_DBAccess;
	
	return bRes;
}

bool ALFA_GeometryReader::GetUFiberParams(PFIBERPARAMS pFiberParams, const eRPotName eRPName, const int nPlateID, const int nFiberID)
{
	bool bRes=false;
	std::list<FIBERPARAMS>::const_iterator iter;
	std::map<eRPotName, ROMAPOT>::const_iterator rpiter;

	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::GetUFiberParams");
	
	if((rpiter=m_MapRPot.find(eRPName))!=m_MapRPot.end()){
		for(iter=(*rpiter).second.ListUFibers.begin();iter!=(*rpiter).second.ListUFibers.end();iter++){
			if(((*iter).nPlateID==nPlateID) && ((*iter).nFiberID==nFiberID)) break;
		}
		
		if(iter==(*rpiter).second.ListUFibers.end()){
			LogStream<<MSG::ERROR<<"Cannot find fiber PotID="<<eRPName<<", PlateID="<<nPlateID<<" and FiberID="<<nFiberID<<endmsg;
			//throw new Exception("Wrong U-fiber");
		}
		else{
                        *pFiberParams = *iter;
			bRes=true;
		}
	}
	else{
		LogStream<<MSG::ERROR<<"Unknown Roman pot PotID="<<eRPName<<endmsg;
	}
	
	return bRes;
}

bool ALFA_GeometryReader::GetVFiberParams(PFIBERPARAMS pFiberParams, const eRPotName eRPName, const int nPlateID, const int nFiberID)
{	
	bool bRes=false;
	std::list<FIBERPARAMS>::const_iterator iter;
	std::map<eRPotName, ROMAPOT>::const_iterator rpiter;
	
	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::GetVFiberParams");

	if((rpiter=m_MapRPot.find(eRPName))!=m_MapRPot.end()){
		for(iter=(*rpiter).second.ListVFibers.begin();iter!=(*rpiter).second.ListVFibers.end();iter++){
			if(((*iter).nPlateID==nPlateID) && ((*iter).nFiberID==nFiberID)) break;
		}
		
		if(iter==(*rpiter).second.ListVFibers.end()) {
			LogStream<<MSG::ERROR<<"Cannot find fiber PotID="<<eRPName<<", PlateID="<<nPlateID<<" and FiberID="<<nFiberID<<endmsg;
			//throw new Exception("Wrong U-fiber");
		}
		else{
                        *pFiberParams = *iter;
			bRes=true;
		}
	}
	else{
		LogStream<<MSG::ERROR<<"Unknown Roman pot PotID="<<eRPName<<endmsg;
	}

	
	return bRes;
}

double ALFA_GeometryReader::GetUFiberCentreXPos(const eRPotName eRPName, const int nPlateID, const int nFiberID)
{
	double fXPos=0.0;
	FIBERPARAMS FiberParams;

	if(m_eFCoordSystem!=EFCS_CLADDING){
		MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::GetUFiberCentreXPos");
		LogStream<<MSG::ERROR<<"Invalid coordinate system"<<endmsg;
		return 0.0;
	}

	if(GetUFiberParams(&FiberParams, eRPName, nPlateID, nFiberID)){
		fXPos=FiberParams.fcs_cladding.fCentreXPos;
	}
	
	return fXPos;
}

double ALFA_GeometryReader::GetVFiberCentreXPos(const eRPotName eRPName, const int nPlateID, const int nFiberID)
{
	double fXPos=0.0;
	FIBERPARAMS FiberParams;

	if(m_eFCoordSystem!=EFCS_CLADDING){
		MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::GetVFiberCentreXPos");
		LogStream<<MSG::ERROR<<"Invalid coordinate system"<<endmsg;
		return 0.0;
	}

	if(GetVFiberParams(&FiberParams, eRPName, nPlateID, nFiberID)){
		fXPos=FiberParams.fcs_cladding.fCentreXPos;
	}
	
	return fXPos;
}

double ALFA_GeometryReader::GetUFiberAngle(const eRPotName eRPName, const int nPlateID, const int nFiberID)
{
	double fAngle=0.0;
	FIBERPARAMS FiberParams;

	if(m_eFCoordSystem!=EFCS_CLADDING){
		MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::GetUFiberAngle");
		LogStream<<MSG::ERROR<<"Invalid coordinate system"<<endmsg;
		return 0.0;
	}

	if(GetUFiberParams(&FiberParams, eRPName, nPlateID, nFiberID)){
		fAngle=FiberParams.fcs_cladding.fAngle;
	}
	
	return fAngle;
}

double ALFA_GeometryReader::GetVFiberAngle(const eRPotName eRPName, const int nPlateID, const int nFiberID)
{
	double fAngle=0.0;
	FIBERPARAMS FiberParams;

	if(m_eFCoordSystem!=EFCS_CLADDING){
		MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::GetVFiberAngle");
		LogStream<<MSG::ERROR<<"Invalid coordinate system"<<endmsg;
		return 0.0;
	}
	
	if(GetVFiberParams(&FiberParams, eRPName, nPlateID, nFiberID)){
		fAngle=FiberParams.fcs_cladding.fAngle;
	}
	
	return fAngle;
}

void ALFA_GeometryReader::SetUFiberPositionToMainReference(const eRPotName eRPName, const int nPlateID, const int nFiberID, const HepGeom::Point3D<float>& TransPoint,const double fTransSlope)
{
	std::list<FIBERPARAMS>::iterator iter;
	std::map<eRPotName, ROMAPOT>::iterator rpiter;
	FIBERPARAMS FiberParams;

	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::SetUFiberPositionToMainReference");

	if(m_eFCoordSystem!=EFCS_CLADDING){
		LogStream<<MSG::ERROR<<"Invalid coordinate system"<<endmsg;
		return;
	}
	
	if((rpiter=m_MapRPot.find(eRPName))!=m_MapRPot.end()){
		for(iter=(*rpiter).second.ListUFibers.begin();iter!=(*rpiter).second.ListUFibers.end();iter++){
			if(((*iter).nPlateID==nPlateID) && ((*iter).nFiberID==nFiberID)) break;
		}
		
		if(iter==(*rpiter).second.ListUFibers.end()) {
			LogStream<<MSG::ERROR<<"Cannot find fiber PotID="<<eRPName<<", PlateID="<<nPlateID<<" and FiberID="<<nFiberID<<endmsg;
			//throw new Exception("Wrong U-fiber");
		}
		else{
			(*iter).MainRefPointPos=TransPoint;
			(*iter).fMainRefPointSlope=fTransSlope;
		}
	}
	else{
		LogStream<<MSG::ERROR<<"Unknown Roman pot PotID="<<eRPName<<endmsg;
	}
}

void ALFA_GeometryReader::SetVFiberPositionToMainReference(const eRPotName eRPName, const int nPlateID, const int nFiberID, const HepGeom::Point3D<float>& TransPoint,const double fTransSlope)
{
	std::list<FIBERPARAMS>::iterator iter;
	std::map<eRPotName, ROMAPOT>::iterator rpiter;
	FIBERPARAMS FiberParams;

	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::SetVFiberPositionToMainReference");

	if(m_eFCoordSystem!=EFCS_CLADDING){
		LogStream<<MSG::ERROR<<"Invalid coordinate system"<<endmsg;
		return;
	}
	
	if((rpiter=m_MapRPot.find(eRPName))!=m_MapRPot.end()){
		for(iter=(*rpiter).second.ListVFibers.begin();iter!=(*rpiter).second.ListVFibers.end();iter++){
			if(((*iter).nPlateID==nPlateID) && ((*iter).nFiberID==nFiberID)) break;
		}
		
		if(iter==(*rpiter).second.ListVFibers.end()){
			LogStream<<MSG::ERROR<<"Cannot find fiber PotID="<<eRPName<<", PlateID="<<nPlateID<<" and FiberID="<<nFiberID<<endmsg;
			//throw new Exception("Wrong U-fiber");
		}
		else{
			(*iter).MainRefPointPos=TransPoint;
			(*iter).fMainRefPointSlope=fTransSlope;
		}
	}
	else{
		LogStream<<MSG::ERROR<<"Unknown Roman pot PotID="<<eRPName<<endmsg;
	}
}

bool ALFA_GeometryReader::GetPlateParams(PPLATEPARAMS pPlateParams, const eRPotName eRPName, const int nPlateID)
{
	bool bRes=false;
	std::map<eRPotName, ROMAPOT>::iterator rpiter;
	std::map<int, PLATEPARAMS>::iterator pliter;
	
	MsgStream LogStream(Athena::getMessageSvc(),"ALFA_GeometryReader::GetPlateParams");

	if((rpiter=m_MapRPot.find(eRPName))!=m_MapRPot.end()){
		if((pliter=(*rpiter).second.MapPlates.find(nPlateID))!=(*rpiter).second.MapPlates.end()){
			*pPlateParams=(*pliter).second;
			bRes=true;
		}
		else{
			LogStream<<MSG::ERROR<<"Unknown Ti plate ID "<<nPlateID<<endmsg;
		}
	}
	else{
		LogStream<<MSG::ERROR<<"Unknown Roman pot PotID="<<eRPName<<endmsg;
	}
	
	return bRes;
}


void ALFA_GeometryReader::PrintFiberGeometry(const char* szOutFilename)
{
	std::ofstream OutStream(szOutFilename);
	PrintFiberGeometry(OutStream);
	OutStream.close();
}

void ALFA_GeometryReader::PrintFiberGeometry(std::ostream &OutStream)
{

	std::list<FIBERPARAMS>::const_iterator iter;
	std::map<eRPotName, ROMAPOT>::const_iterator rpiter;
	FIBERPARAMS FiberParams;

	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::PrintGeometry");

	if(m_eFCoordSystem!=EFCS_CLADDING && m_eFCoordSystem!=EFCS_ATLAS){
		LogStream<<MSG::ERROR<<"Invalid coordinate system"<<endmsg;
		return;
	}
	
	for(rpiter=m_MapRPot.begin();rpiter!=m_MapRPot.end();rpiter++){
		OutStream<<std::endl<<"Geometry of U-fibers in Roma Pot "<<(*rpiter).first<<std::endl;
		if(m_eFCoordSystem==EFCS_CLADDING) OutStream<<"nPotID\t\t\t\tnPlateID\t\t\t\tnFiberID\t\t\t\tnLayerID\t\t\t\tfCentreXPos\t\t\t\tfAngle"<<std::endl;
		else if(m_eFCoordSystem==EFCS_ATLAS) OutStream<<"nPotID\t\t\t\tnPlateID\t\t\t\tnFiberID\t\t\t\tnLayerID\t\t\t\tfSlope\t\t\t\tfOffset\t\t\t\tfZPos"<<std::endl;
		
		for(iter=(*rpiter).second.ListUFibers.begin();iter!=(*rpiter).second.ListUFibers.end();iter++){
			OutStream<<(*rpiter).first<<"\t\t\t\t"<<(*iter).nPlateID<<"\t\t\t\t"<<(*iter).nFiberID<<"\t\t\t\t"<<(*iter).nLayerID<<"\t\t\t\t";
			if(m_eFCoordSystem==EFCS_CLADDING) OutStream<<(*iter).fcs_cladding.fCentreXPos<<"\t\t\t\t"<<(*iter).fcs_cladding.fAngle<<std::endl;
			else if(m_eFCoordSystem==EFCS_ATLAS) OutStream<<(*iter).fcs_atlas.fSlope<<"\t\t\t\t"<<(*iter).fcs_atlas.fOffset<<(*iter).fcs_atlas.fZPos<<std::endl;
		}
		
		OutStream<<std::endl<<"Geometry of V-fibers in Roma Pot "<<(*rpiter).first<<std::endl;
		if(m_eFCoordSystem==EFCS_CLADDING) OutStream<<"nPotID\t\t\tnPlateID\t\t\tnFiberID\t\t\tnLayerID\t\t\tfCentreXPos\t\t\tfAngle\t\t\tfSlope\t\t\tfOffset\t\t\tfZPos"<<std::endl;
		else if(m_eFCoordSystem==EFCS_ATLAS) OutStream<<"nPotID\t\t\t\tnPlateID\t\t\t\tnFiberID\t\t\t\tnLayerID\t\t\t\tfSlope\t\t\t\tfOffset\t\t\t\tfZPos"<<std::endl;
		
		for(iter=(*rpiter).second.ListVFibers.begin();iter!=(*rpiter).second.ListVFibers.end();iter++){
			OutStream<<(*rpiter).first<<"\t\t\t\t"<<(*iter).nPlateID<<"\t\t\t\t"<<(*iter).nFiberID<<"\t\t\t\t"<<(*iter).nLayerID<<"\t\t\t\t";
			if(m_eFCoordSystem==EFCS_CLADDING) OutStream<<(*iter).fcs_cladding.fCentreXPos<<"\t\t\t\t"<<(*iter).fcs_cladding.fAngle<<std::endl;
			else if(m_eFCoordSystem==EFCS_ATLAS) OutStream<<(*iter).fcs_atlas.fSlope<<"\t\t\t\t"<<(*iter).fcs_atlas.fOffset<<(*iter).fcs_atlas.fZPos<<std::endl;
		}

		OutStream<<std::endl<<"Geometry of V0-ODFibers in Roma Pot "<<(*rpiter).first<<std::endl;
		if(m_eFCoordSystem==EFCS_CLADDING) OutStream<<"nPotID\t\t\tnPlateID\t\t\tnFiberID\t\t\tnLayerID\t\t\tfCentreXPos\t\t\tfAngle\t\t\tfSlope\t\t\tfOffset\t\t\tfZPos"<<std::endl;
		else if(m_eFCoordSystem==EFCS_ATLAS) OutStream<<"nPotID\t\t\t\tnPlateID\t\t\t\tnFiberID\t\t\t\tnLayerID\t\t\t\tfSlope\t\t\t\tfOffset\t\t\t\tfZPos"<<std::endl;

		for(iter=(*rpiter).second.ListODFibersV0.begin();iter!=(*rpiter).second.ListODFibersV0.end();iter++){
			OutStream<<(*rpiter).first<<"\t\t\t\t"<<(*iter).nPlateID<<"\t\t\t\t"<<(*iter).nFiberID<<"\t\t\t\t"<<(*iter).nLayerID<<"\t\t\t\t";
			if(m_eFCoordSystem==EFCS_CLADDING) OutStream<<(*iter).fcs_cladding.fCentreYPos<<"\t\t\t\t"<<(*iter).fcs_cladding.fAngle<<std::endl;
			else if(m_eFCoordSystem==EFCS_ATLAS) OutStream<<(*iter).fcs_atlas.fSlope<<"\t\t\t\t"<<(*iter).fcs_atlas.fOffset<<(*iter).fcs_atlas.fZPos<<std::endl;
		}

		OutStream<<std::endl<<"Geometry of U0-ODFibers in Roma Pot "<<(*rpiter).first<<std::endl;
		if(m_eFCoordSystem==EFCS_CLADDING) OutStream<<"nPotID\t\t\tnPlateID\t\t\tnFiberID\t\t\tnLayerID\t\t\tfCentreXPos\t\t\tfAngle\t\t\tfSlope\t\t\tfOffset\t\t\tfZPos"<<std::endl;
		else if(m_eFCoordSystem==EFCS_ATLAS) OutStream<<"nPotID\t\t\t\tnPlateID\t\t\t\tnFiberID\t\t\t\tnLayerID\t\t\t\tfSlope\t\t\t\tfOffset\t\t\t\tfZPos"<<std::endl;

		for(iter=(*rpiter).second.ListODFibersU0.begin();iter!=(*rpiter).second.ListODFibersU0.end();iter++){
			OutStream<<(*rpiter).first<<"\t\t\t\t"<<(*iter).nPlateID<<"\t\t\t\t"<<(*iter).nFiberID<<"\t\t\t\t"<<(*iter).nLayerID<<"\t\t\t\t";
			if(m_eFCoordSystem==EFCS_CLADDING) OutStream<<(*iter).fcs_cladding.fCentreYPos<<"\t\t\t\t"<<(*iter).fcs_cladding.fAngle<<std::endl;
			else if(m_eFCoordSystem==EFCS_ATLAS) OutStream<<(*iter).fcs_atlas.fSlope<<"\t\t\t\t"<<(*iter).fcs_atlas.fOffset<<(*iter).fcs_atlas.fZPos<<std::endl;
		}

		OutStream<<std::endl<<"Geometry of U1-ODFibers in Roma Pot "<<(*rpiter).first<<std::endl;
		if(m_eFCoordSystem==EFCS_CLADDING) OutStream<<"nPotID\t\t\tnPlateID\t\t\tnFiberID\t\t\tnLayerID\t\t\tfCentreXPos\t\t\tfAngle\t\t\tfSlope\t\t\tfOffset\t\t\tfZPos"<<std::endl;
		else if(m_eFCoordSystem==EFCS_ATLAS) OutStream<<"nPotID\t\t\t\tnPlateID\t\t\t\tnFiberID\t\t\t\tnLayerID\t\t\t\tfSlope\t\t\t\tfOffset\t\t\t\tfZPos"<<std::endl;

		for(iter=(*rpiter).second.ListODFibersU1.begin();iter!=(*rpiter).second.ListODFibersU1.end();iter++){
			OutStream<<(*rpiter).first<<"\t\t\t\t"<<(*iter).nPlateID<<"\t\t\t\t"<<(*iter).nFiberID<<"\t\t\t\t"<<(*iter).nLayerID<<"\t\t\t\t";
			if(m_eFCoordSystem==EFCS_CLADDING) OutStream<<(*iter).fcs_cladding.fCentreYPos<<"\t\t\t\t"<<(*iter).fcs_cladding.fAngle<<std::endl;
			else if(m_eFCoordSystem==EFCS_ATLAS) OutStream<<(*iter).fcs_atlas.fSlope<<"\t\t\t\t"<<(*iter).fcs_atlas.fOffset<<(*iter).fcs_atlas.fZPos<<std::endl;
		}

		OutStream<<std::endl<<"Geometry of V1-ODFibers in Roma Pot "<<(*rpiter).first<<std::endl;
		if(m_eFCoordSystem==EFCS_CLADDING) OutStream<<"nPotID\t\t\tnPlateID\t\t\tnFiberID\t\t\tnLayerID\t\t\tfCentreXPos\t\t\tfAngle\t\t\tfSlope\t\t\tfOffset\t\t\tfZPos"<<std::endl;
		else if(m_eFCoordSystem==EFCS_ATLAS) OutStream<<"nPotID\t\t\t\tnPlateID\t\t\t\tnFiberID\t\t\t\tnLayerID\t\t\t\tfSlope\t\t\t\tfOffset\t\t\t\tfZPos"<<std::endl;

		for(iter=(*rpiter).second.ListODFibersV1.begin();iter!=(*rpiter).second.ListODFibersV1.end();iter++){
			OutStream<<(*rpiter).first<<"\t\t\t\t"<<(*iter).nPlateID<<"\t\t\t\t"<<(*iter).nFiberID<<"\t\t\t\t"<<(*iter).nLayerID<<"\t\t\t\t";
			if(m_eFCoordSystem==EFCS_CLADDING) OutStream<<(*iter).fcs_cladding.fCentreYPos<<"\t\t\t\t"<<(*iter).fcs_cladding.fAngle<<std::endl;
			else if(m_eFCoordSystem==EFCS_ATLAS) OutStream<<(*iter).fcs_atlas.fSlope<<"\t\t\t\t"<<(*iter).fcs_atlas.fOffset<<(*iter).fcs_atlas.fZPos<<std::endl;
		}
	}
}

bool ALFA_GeometryReader::StoreReconstructionGeometry(const eRPotName eRPName, const eFiberType eFType, const char* szDataDestination)
{
	int i,j;
	double fParamB, fY, fX, fZ, fSlope;
	FILE * pFile;
	FIBERPARAMS FiberParams;

	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::StoreReconstructionGeometry");

	if(!(m_eFCoordSystem==EFCS_CLADDING || m_eFCoordSystem==EFCS_ATLAS)){
		LogStream<<MSG::ERROR<<"Invalid coordinate system"<<endmsg;
		return false;
	}

	pFile = fopen(szDataDestination, "w");
	if(pFile==NULL) return false;

	fprintf(pFile, "xxxxxxxxxxxxxxxxxxx\n");
	
	
	if (eFType==EFT_FIBERMD)
	{
		fprintf(pFile, "20\n");

		for(i=1;i<=ALFAPLATESCNT;i++){
			//U-fiber
			for(j=1;j<=ALFAFIBERSCNT;j++){
				if(GetUFiberParams(&FiberParams, eRPName, i, j)==true){
					if(m_eFCoordSystem==EFCS_CLADDING){
						fX=FiberParams.MainRefPointPos.x();
						fY=FiberParams.MainRefPointPos.y();
						fZ=FiberParams.MainRefPointPos.z();
						fSlope=FiberParams.fMainRefPointSlope;
						//fSlope=FiberParams.fSlope;
						fParamB=fY-fSlope*fX;
					}
					else {
						fSlope=FiberParams.fcs_atlas.fSlope;
						fParamB=FiberParams.fcs_atlas.fOffset;
						fZ=FiberParams.fcs_atlas.fZPos;
					}

					fprintf(pFile, " %2d  %2d  %7.5f  %7.4f  %7.3f\n", (i*2-1), j, fSlope, fParamB, fZ);
				}
			}
			
			//V-fiber
			for(j=1;j<=ALFAFIBERSCNT;j++){
				if(GetVFiberParams(&FiberParams, eRPName, i, j)==true){
					if(m_eFCoordSystem==EFCS_CLADDING){
						fX=FiberParams.MainRefPointPos.x();
						fY=FiberParams.MainRefPointPos.y();
						fZ=FiberParams.MainRefPointPos.z();
						fSlope=FiberParams.fMainRefPointSlope;
						//fSlope=FiberParams.fSlope;
						fParamB=fY-fSlope*fX;
					}
					else {
						fSlope=FiberParams.fcs_atlas.fSlope;
						fParamB=FiberParams.fcs_atlas.fOffset;
						fZ=FiberParams.fcs_atlas.fZPos;
					}

					fprintf(pFile, " %2d  %2d  %7.5f  %7.4f  %7.3f\n", (i*2), j, fSlope, fParamB, fZ);
				}
			}
		}
	}
	else if (eFType==EFT_FIBEROD)
	{
		fprintf(pFile, "6\n");

		for(int i=1; i<=ODPLATESCNT; i++)
		{
			//V0-ODFiber
			for(j=1;j<=ODFIBERSCNT;j++)
			{
				if(GetODFiberParams(&FiberParams, EFT_ODFIBERV0, eRPName, i, j)==true)
				{
					if(m_eFCoordSystem==EFCS_CLADDING){
						fX=FiberParams.MainRefPointPos.x();
						fY=FiberParams.MainRefPointPos.y();
						fZ=FiberParams.MainRefPointPos.z();
						fSlope=FiberParams.fMainRefPointSlope;
						fParamB=fY-fSlope*fX;
					}
					else {
						fSlope=FiberParams.fcs_atlas.fSlope;
						fParamB=FiberParams.fcs_atlas.fOffset;
						fZ=FiberParams.fcs_atlas.fZPos;
					}

					fprintf(pFile, " %2d  %2d  %7.5f  %7.4f  %7.3f\n", (i*2-1), j, fSlope, fParamB, fZ);
				}
			}

			//U0-ODFiber (note: U0-nFiberID is indexed from 16 to 30)
			for(j=ODFIBERSCNT+1;j<=ODFIBERSCNT+15;j++)
			{
				if(GetODFiberParams(&FiberParams, EFT_ODFIBERU0, eRPName, i, j)==true)
				{
					if(m_eFCoordSystem==EFCS_CLADDING){
						fX=FiberParams.MainRefPointPos.x();
						fY=FiberParams.MainRefPointPos.y();
						fZ=FiberParams.MainRefPointPos.z();
						fSlope=FiberParams.fMainRefPointSlope;
						fParamB=fY-fSlope*fX;
					}
					else {
						fSlope=FiberParams.fcs_atlas.fSlope;
						fParamB=FiberParams.fcs_atlas.fOffset;
						fZ=FiberParams.fcs_atlas.fZPos;
					}

					fprintf(pFile, " %2d  %2d  %7.5f  %7.4f  %7.3f\n", (i*2-1), j, fSlope, fParamB, fZ);
				}
			}

			//1.10.2010 LN: change of indexation scheme
			//V1-ODFiber (note: V1-nFiberID is indexed from 16 to 30)
			//for(j=ODFIBERSCNT+1;j<=ODFIBERSCNT+15;j++)
			for(j=1;j<=ODFIBERSCNT;j++)
			{
				if(GetODFiberParams(&FiberParams, EFT_ODFIBERV1, eRPName, i, j)==true)
				{
					if(m_eFCoordSystem==EFCS_CLADDING){
						fX=FiberParams.MainRefPointPos.x();
						fY=FiberParams.MainRefPointPos.y();
						fZ=FiberParams.MainRefPointPos.z();
						fSlope=FiberParams.fMainRefPointSlope;
						fParamB=fY-fSlope*fX;
					}
					else {
						fSlope=FiberParams.fcs_atlas.fSlope;
						fParamB=FiberParams.fcs_atlas.fOffset;
						fZ=FiberParams.fcs_atlas.fZPos;
					}
					fprintf(pFile, " %2d  %2d  %7.5f  %7.4f  %7.3f\n", (i*2), j, fSlope, fParamB, fZ);
				}
			}

			//U1-ODFiber
			//1.10.2010 LN: change of indexation scheme
			//for(j=1;j<=ODFIBERSCNT;j++)
			for(j=ODFIBERSCNT+1;j<=ODFIBERSCNT+15;j++)
			{
				if(GetODFiberParams(&FiberParams, EFT_ODFIBERU1, eRPName, i, j)==true)
				{
					if(m_eFCoordSystem==EFCS_CLADDING){
						fX=FiberParams.MainRefPointPos.x();
						fY=FiberParams.MainRefPointPos.y();
						fZ=FiberParams.MainRefPointPos.z();
						fSlope=FiberParams.fMainRefPointSlope;
						fParamB=fY-fSlope*fX;
					}
					else {
						fSlope=FiberParams.fcs_atlas.fSlope;
						fParamB=FiberParams.fcs_atlas.fOffset;
						fZ=FiberParams.fcs_atlas.fZPos;
					}
					fprintf(pFile, " %2d  %2d  %7.5f  %7.4f  %7.3f\n", (i*2), j, fSlope, fParamB, fZ);
				}
			}

		}
	}

	fclose(pFile);
	
	return true;
}

void ALFA_GeometryReader::UpdateGeometry()
{
	int i;
	std::map<eRPotName,ROMAPOT>::iterator rpiter;
	std::list<FIBERPARAMS>::const_iterator iter;
	FIBERPARAMS Fiber01Params,Fiber64Params;
	
	for(rpiter=m_MapRPot.begin();rpiter!=m_MapRPot.end();rpiter++){
		for(i=1;i<=10;i++){
			GetUFiberParams(&Fiber01Params,(*rpiter).first,i,1);
			GetUFiberParams(&Fiber64Params,(*rpiter).first,i,64);
			//(*rpiter).second.MapPlates[i].fUCladdingSizeX=fabs(Fiber01Params.fCentreXPos-Fiber64Params.fCentreXPos)+0.48*CLHEP::mm;
			(*rpiter).second.MapPlates[i].fUCladdingSizeX=2.0*(fmax(fabs(Fiber01Params.fcs_cladding.fCentreXPos),fabs(Fiber64Params.fcs_cladding.fCentreXPos))+0.24);
			//cout<<"U clad half-size="<<(*rpiter).second.MapPlates[i].fUCladdingSizeX/2.0<<"("<<Fiber01Params.fCentreXPos<<", "<<Fiber64Params.fCentreXPos<<")"<<std::endl;
			
			GetVFiberParams(&Fiber01Params,(*rpiter).first,i,1);
			GetVFiberParams(&Fiber64Params,(*rpiter).first,i,64);
			//(*rpiter).second.MapPlates[i].fVCladdingSizeX=fabs(Fiber01Params.fCentreXPos-Fiber64Params.fCentreXPos)+0.48*CLHEP::mm;
			(*rpiter).second.MapPlates[i].fVCladdingSizeX=2.0*(fmax(fabs(Fiber01Params.fcs_cladding.fCentreXPos),fabs(Fiber64Params.fcs_cladding.fCentreXPos))+0.24);
			//cout<<"V clad half-size="<<(*rpiter).second.MapPlates[i].fVCladdingSizeX/2.0<<"("<<Fiber01Params.fCentreXPos<<", "<<Fiber64Params.fCentreXPos<<")"<<std::endl;
		}
	}
}

void ALFA_GeometryReader::GetListOfRPotIDs(std::map<eRPotName,std::string>* pMapRPotName)
{
	std::string strLabel;
	std::map<eRPotName,ROMAPOT>::const_iterator rpiter;
	
	if(pMapRPotName!=NULL){
		pMapRPotName->clear();
		
		for(rpiter=m_MapRPot.begin();rpiter!=m_MapRPot.end();rpiter++){
			strLabel=GetRPotLabel((*rpiter).first);
			pMapRPotName->insert(std::pair<eRPotName,std::string>((*rpiter).first,strLabel));
		}
	}
}

void ALFA_GeometryReader::GetListOfExistingRPotIDs(std::list<eRPotName>* pListRPotName)
{
	*pListRPotName=m_ListExistingRPots;
}

bool ALFA_GeometryReader::GetRPPosParams(PRPPOSPARAMS pRPPosParams, const eRPotName eRPName)
{
	bool bRes=false;
	std::map<eRPotName,RPPOSPARAMS>::iterator iter;

	MsgStream LogStream(Athena::getMessageSvc(),"ALFA_GeometryReader::GetRPPosParams");
	
	pRPPosParams->clear();
	if((iter=m_RPPosParams.find(eRPName))!=m_RPPosParams.end()){
		*pRPPosParams=(*iter).second;
		bRes=true;
	}
	else{
		LogStream<<MSG::ERROR<<"Unknown Roma pot ID="<<eRPName<<endmsg;
	}
	
	return bRes;
}

bool ALFA_GeometryReader::GetASPosParams(PASPOSPARAMS pASPosParams, const eAStationName eASName)
{
	bool bRes=false;
	std::map<eAStationName,ASPOSPARAMS>::iterator iter;

	MsgStream LogStream(Athena::getMessageSvc(),"ALFA_GeometryReader::GetASPosParams");
	
	pASPosParams->clear();
	if((iter=m_ASPosParams.find(eASName))!=m_ASPosParams.end()){
		*pASPosParams=(*iter).second;
		bRes=true;
	}
	else{
		LogStream<<MSG::ERROR<<"Unknown ALFA Station ID="<<eASName<<endmsg;
	}
	
	return bRes;
}

eGeoSourceType ALFA_GeometryReader::GetRPGeometryType(const eRPotName eRPName, eFiberType eFType)
{
	eGeoSourceType eGeoType=EGST_UNDEFINED;

	if(m_MapRPot.find(eRPName)!=m_MapRPot.end()){
		if(eFType==EFT_FIBERMD) eGeoType=m_MapRPot[eRPName].eMDGeometryType;
		else if(eFType==EFT_FIBEROD) eGeoType=m_MapRPot[eRPName].eODGeometryType;
	}
	else{
		MsgStream LogStream(Athena::getMessageSvc(),"ALFA_GeometryReader::GetRPGeometryType");
		LogStream<<MSG::ERROR<<"Unknown Roman pot ID="<<eRPName<<endmsg;
	}
	
	
	return eGeoType;
}

bool ALFA_GeometryReader::GetMDFiberParams(PFIBERPARAMS pFiberParams, const eFiberType eFType, const eRPotName eRPName, const int nPlateID, const int nFiberID)
{
	bool bRes=false;

	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::GetMDFiberParams");

	if(pFiberParams==NULL)
	{
		LogStream<<MSG::ERROR<<"pFiberParams points to NULL"<<endmsg;
	}
	else if(m_eFCoordSystem!=EFCS_ATLAS)
	{
		LogStream<<MSG::ERROR<<"Invalid coordinate system"<<endmsg;
	}
	else{
		switch(eFType){
		case EFT_UFIBER:
			bRes=GetUFiberParams(pFiberParams, eRPName, nPlateID, nFiberID);
			break;
		case EFT_VFIBER:
			bRes=GetVFiberParams(pFiberParams, eRPName, nPlateID, nFiberID);
			break;
		default:
			LogStream<<MSG::ERROR<<"Invalid fiber type"<<endmsg;
			break;
		}
	}

	return bRes;
}

// OD functions
bool ALFA_GeometryReader::GetODFiberParams(PFIBERPARAMS pFiberParams, const eFiberType eFType, const eRPotName eRPName, const int nPlateID, const int nFiberID)
{
	bool bRes=false;
	std::list<FIBERPARAMS>::const_iterator iter;
	std::map<eRPotName, ROMAPOT>::const_iterator rpiter;
	
	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::GetODFiberParams");
	
	if ((rpiter=m_MapRPot.find(eRPName))!=m_MapRPot.end())
	{
		switch (eFType)
		{
		case EFT_ODFIBERU0:
			{
				for(iter=(*rpiter).second.ListODFibersU0.begin();iter!=(*rpiter).second.ListODFibersU0.end();iter++)
				{
					if (((*iter).nPlateID==nPlateID) && ((*iter).nFiberID==nFiberID)) break;
				}

				if (iter==(*rpiter).second.ListODFibersU0.end())
				{
					LogStream<<MSG::ERROR<<"Cannot find ODFiberU0 PotID="<<eRPName<<", PlateID="<<nPlateID<<" and FiberID="<<nFiberID<<endmsg;
					//throw new G4Exception("Wrong ODFiberU0");
				}
				else
				{
                                        *pFiberParams = *iter;
					bRes=true;
				}

				break;
			}
		case EFT_ODFIBERV0:
			{
				for(iter=(*rpiter).second.ListODFibersV0.begin();iter!=(*rpiter).second.ListODFibersV0.end();iter++)
				{
					if (((*iter).nPlateID==nPlateID) && ((*iter).nFiberID==nFiberID)) break;
				}

				if (iter==(*rpiter).second.ListODFibersV0.end())
				{
					LogStream<<MSG::ERROR<<"Cannot find ODFiberV0 PotID="<<eRPName<<", PlateID="<<nPlateID<<" and FiberID="<<nFiberID<<endmsg;
					//throw new G4Exception("Wrong ODFiberV0");
				}
				else
				{
                                        *pFiberParams = *iter;
					bRes=true;
				}

				break;
			}
		case EFT_ODFIBERU1:
			{
				for(iter=(*rpiter).second.ListODFibersU1.begin();iter!=(*rpiter).second.ListODFibersU1.end();iter++)
				{
					if (((*iter).nPlateID==nPlateID) && ((*iter).nFiberID==nFiberID)) break;
				}

				if (iter==(*rpiter).second.ListODFibersU1.end())
				{
					LogStream<<MSG::ERROR<<"Cannot find ODFiberU1 PotID="<<eRPName<<", PlateID="<<nPlateID<<" and FiberID="<<nFiberID<<endmsg;
					//throw new G4Exception("Wrong ODFiberU1");
				}
				else
				{
                                        *pFiberParams = *iter;
					bRes=true;
				}

				break;
			}
		case EFT_ODFIBERV1:
			{
				for(iter=(*rpiter).second.ListODFibersV1.begin();iter!=(*rpiter).second.ListODFibersV1.end();iter++)
				{
					if (((*iter).nPlateID==nPlateID) && ((*iter).nFiberID==nFiberID)) break;
				}

				if (iter==(*rpiter).second.ListODFibersV1.end())
				{
					LogStream<<MSG::ERROR<<"Cannot find ODFiberV1 PotID="<<eRPName<<", PlateID="<<nPlateID<<" and FiberID="<<nFiberID<<endmsg;
					//throw new G4Exception("Wrong ODFiberV1");
				}
				else
				{
                                        *pFiberParams = *iter;
					bRes=true;
				}

				break;
			}
		default:
			{
				LogStream<<MSG::ERROR<<"Unknown ODFiber eFType="<<eFType<<endmsg;
				break;
			}
		}
	}
	else
	{
		LogStream<<MSG::ERROR<<"Unknown Roma pot PotID="<<eRPName<<endmsg;
	}
	
	return bRes;
}

void ALFA_GeometryReader::SetODFiberPositionToMainReference(const eRPotName eRPName, const eFiberType eFType, const int nPlateID, const int nFiberID, const HepGeom::Point3D<float>& TransPoint, const double fTransSlope)
{
	std::list<FIBERPARAMS>::iterator iter;
	std::map<eRPotName, ROMAPOT>::iterator rpiter;
	
	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::SetODFiberPositionToMainReference");

	if(m_eFCoordSystem!=EFCS_CLADDING){
		LogStream<<MSG::ERROR<<"Invalid coordinate system"<<endmsg;
		return;
	}

	if ((rpiter=m_MapRPot.find(eRPName))!=m_MapRPot.end())
	{
		switch (eFType)
		{
		case EFT_ODFIBERU0:
			{
				for(iter=(*rpiter).second.ListODFibersU0.begin();iter!=(*rpiter).second.ListODFibersU0.end();iter++)
				{
					if (((*iter).nPlateID==nPlateID) && ((*iter).nFiberID==nFiberID)) break;
				}

				if (iter==(*rpiter).second.ListODFibersU0.end())
				{
					LogStream<<MSG::ERROR<<"Cannot find ODFiberU0 PotID="<<eRPName<<", PlateID="<<nPlateID<<" and FiberID="<<nFiberID<<endmsg;
					//throw new G4Exception("Wrong ODFiberU0");
				}
				else
				{
					(*iter).MainRefPointPos=TransPoint;
					(*iter).fMainRefPointSlope=fTransSlope;
				}

				break;
			}
		case EFT_ODFIBERV0:
			{
				for(iter=(*rpiter).second.ListODFibersV0.begin();iter!=(*rpiter).second.ListODFibersV0.end();iter++)
				{
					if (((*iter).nPlateID==nPlateID) && ((*iter).nFiberID==nFiberID)) break;
				}

				if (iter==(*rpiter).second.ListODFibersV0.end())
				{
					LogStream<<MSG::ERROR<<"Cannot find ODFiberV0 PotID="<<eRPName<<", PlateID="<<nPlateID<<" and FiberID="<<nFiberID<<endmsg;
					//throw new G4Exception("Wrong ODFiberV0");
				}
				else
				{
					(*iter).MainRefPointPos=TransPoint;
					(*iter).fMainRefPointSlope=fTransSlope;
				}

				break;
			}
		case EFT_ODFIBERU1:
			{
				for(iter=(*rpiter).second.ListODFibersU1.begin();iter!=(*rpiter).second.ListODFibersU1.end();iter++)
				{
					if (((*iter).nPlateID==nPlateID) && ((*iter).nFiberID==nFiberID)) break;
				}

				if (iter==(*rpiter).second.ListODFibersU1.end())
				{
					LogStream<<MSG::ERROR<<"Cannot find ODFiberU1 PotID="<<eRPName<<", PlateID="<<nPlateID<<" and FiberID="<<nFiberID<<endmsg;
					//throw new G4Exception("Wrong ODFiberU1");
				}
				else
				{
					(*iter).MainRefPointPos=TransPoint;
					(*iter).fMainRefPointSlope=fTransSlope;
				}

				break;
			}
		case EFT_ODFIBERV1:
			{
				for(iter=(*rpiter).second.ListODFibersV1.begin();iter!=(*rpiter).second.ListODFibersV1.end();iter++)
				{
					if (((*iter).nPlateID==nPlateID) && ((*iter).nFiberID==nFiberID)) break;
				}

				if (iter==(*rpiter).second.ListODFibersV1.end())
				{
					LogStream<<MSG::ERROR<<"Cannot find ODFiberV1 PotID="<<eRPName<<", PlateID="<<nPlateID<<" and FiberID="<<nFiberID<<endmsg;
					//throw new G4Exception("Wrong ODFiberV1");
				}
				else
				{
					(*iter).MainRefPointPos=TransPoint;
					(*iter).fMainRefPointSlope=fTransSlope;
				}

				break;
			}
		default:
			{
				LogStream<<MSG::ERROR<<"Unknown ODFiber eFType="<<eFType<<endmsg;
				break;
			}
		}
	}
	else
	{
		LogStream<<MSG::ERROR<<"Unknown Roman pot PotID="<<eRPName<<endmsg;
	}

}

double ALFA_GeometryReader::GetODFiberCentreYPos(const eRPotName eRPName, const eFiberType eFType, const int nPlateID, const int nFiberID)
{
	double fYPos=0.0;
	FIBERPARAMS FiberParams;

	if(m_eFCoordSystem!=EFCS_CLADDING){
		MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::GetODFiberCentreYPos");
		LogStream<<MSG::ERROR<<"Invalid coordinate system"<<endmsg;
		return 0.0;
	}

	if(GetODFiberParams(&FiberParams, eFType, eRPName, nPlateID, nFiberID))
	{
		fYPos=FiberParams.fcs_cladding.fCentreYPos;
	}
	
	return fYPos;
}

double ALFA_GeometryReader::GetODFiberAngle(const eRPotName eRPName, const eFiberType eFType, const int nPlateID, const int nFiberID)
{
	double fAngle=0.0;
	FIBERPARAMS FiberParams;

	if(m_eFCoordSystem!=EFCS_CLADDING){
		MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::GetODFiberAngle");
		LogStream<<MSG::ERROR<<"Invalid coordinate system"<<endmsg;
		return 0.0;
	}
	
	if(GetODFiberParams(&FiberParams, eFType, eRPName, nPlateID, nFiberID))
	{
		fAngle=FiberParams.fcs_cladding.fAngle;
	}
	
	return fAngle;
}

double ALFA_GeometryReader::GetMDFiberSlope(const eRPotName eRPName, const eFiberType eFType, const int nPlateID, const int nFiberID)
{
	double fSlope=0.0;
	FIBERPARAMS FiberParams;

	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::GetMDFiberSlope");
	
	if(m_eFCoordSystem!=EFCS_ATLAS){
		LogStream<<MSG::ERROR<<"Invalid coordinate system"<<endmsg;
		return 0.0;
	}

	switch(eFType){
	case EFT_UFIBER:
		if(GetUFiberParams(&FiberParams, eRPName, nPlateID, nFiberID)){
			fSlope=FiberParams.fcs_atlas.fSlope;
		}
		break;
		case EFT_VFIBER:
		if(GetVFiberParams(&FiberParams, eRPName, nPlateID, nFiberID)){
			fSlope=FiberParams.fcs_atlas.fSlope;
		}
		break;
		default:
		LogStream<<MSG::ERROR<<"Invalid fiber type"<<endmsg;
		break;
	}
	
	return fSlope;
}

double ALFA_GeometryReader::GetMDFiberOffset(const eRPotName eRPName, const eFiberType eFType, const int nPlateID, const int nFiberID)
{
	double fOffset=0.0;
	FIBERPARAMS FiberParams;

	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::GetMDFiberOffset");
	
	if(m_eFCoordSystem!=EFCS_ATLAS){
		LogStream<<MSG::ERROR<<"Invalid coordinate system"<<endmsg;
		return 0.0;
	}

	switch(eFType){
	case EFT_UFIBER:
		if(GetUFiberParams(&FiberParams, eRPName, nPlateID, nFiberID)){
			fOffset=FiberParams.fcs_atlas.fOffset;
		}
		break;
		case EFT_VFIBER:
		if(GetVFiberParams(&FiberParams, eRPName, nPlateID, nFiberID)){
			fOffset=FiberParams.fcs_atlas.fOffset;
		}
		break;
		default:
		LogStream<<MSG::ERROR<<"Invalid fiber type"<<endmsg;
		break;
	}
	
	return fOffset;
}

double ALFA_GeometryReader::GetMDFiberZPos(const eRPotName eRPName, const eFiberType eFType, const int nPlateID, const int nFiberID)
{
	double fZPos=0.0;
	FIBERPARAMS FiberParams;

	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::GetMDFiberZPos");
	
	if(m_eFCoordSystem!=EFCS_ATLAS){
		LogStream<<MSG::ERROR<<"Invalid coordinate system"<<endmsg;
		return 0.0;
	}

	switch(eFType){
	case EFT_UFIBER:
		if(GetUFiberParams(&FiberParams, eRPName, nPlateID, nFiberID)){
			fZPos=FiberParams.fcs_atlas.fZPos;
		}
		break;
		case EFT_VFIBER:
		if(GetVFiberParams(&FiberParams, eRPName, nPlateID, nFiberID)){
			fZPos=FiberParams.fcs_atlas.fZPos;
		}
		break;
		default:
		LogStream<<MSG::ERROR<<"Invalid fiber type"<<endmsg;
		break;
	}
	
	return fZPos;
}

double ALFA_GeometryReader::GetODFiberSlope(const eRPotName eRPName, const eFiberType eFType, const int nPlateID, const int nFiberID)
{
	double fSlope=0.0;
	FIBERPARAMS FiberParams;

	if(m_eFCoordSystem!=EFCS_ATLAS){
		MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::GetODFiberSlope");
		LogStream<<MSG::ERROR<<"Invalid coordinate system"<<endmsg;
		return 0.0;
	}
	
	if(GetODFiberParams(&FiberParams, eFType, eRPName, nPlateID, nFiberID))
	{
		fSlope=FiberParams.fcs_atlas.fSlope;
	}
	
	return fSlope;
}

double ALFA_GeometryReader::GetODFiberOffset(const eRPotName eRPName, const eFiberType eFType, const int nPlateID, const int nFiberID)
{
	double fOffset=0.0;
	FIBERPARAMS FiberParams;

	if(m_eFCoordSystem!=EFCS_ATLAS){
		MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::GetODFiberOffset");
		LogStream<<MSG::ERROR<<"Invalid coordinate system"<<endmsg;
		return 0.0;
	}
	
	if(GetODFiberParams(&FiberParams, eFType, eRPName, nPlateID, nFiberID))
	{
		fOffset=FiberParams.fcs_atlas.fOffset;
	}
	
	return fOffset;
}

double ALFA_GeometryReader::GetODFiberZPos(const eRPotName eRPName, const eFiberType eFType, const int nPlateID, const int nFiberID)
{
	double fZPos=0.0;
	FIBERPARAMS FiberParams;

	if(m_eFCoordSystem!=EFCS_ATLAS){
		MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::GetODFiberZPos");
		LogStream<<MSG::ERROR<<"Invalid coordinate system"<<endmsg;
		return 0.0;
	}
	
	if(GetODFiberParams(&FiberParams, eFType, eRPName, nPlateID, nFiberID))
	{
		fZPos=FiberParams.fcs_atlas.fZPos;
	}
	
	return fZPos;
}

bool ALFA_GeometryReader::ParseRPMetrology(eGeoSourceType eSourceType, const char* szDataSource)
{
	bool bRes=false;

	eRPotName eRPName=ERPN_UNDEFINED;
	ALFA_ConfigParams CfgParams;

	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::ParseRPMetrology");

	if(eSourceType==EGST_FILE){
		// resolve ALFA Stations parameters
		if(CfgParams.Init(szDataSource,"[B7L1]")==0) return false;
		if(!SetupStationMetrologyPoints(CfgParams,EASN_B7L1)) return false;

		if(CfgParams.Init(szDataSource,"[A7L1]")==0) return false;
		if(!SetupStationMetrologyPoints(CfgParams,EASN_A7L1)) return false;

		if(CfgParams.Init(szDataSource,"[A7R1]")==0) return false;
		if(!SetupStationMetrologyPoints(CfgParams,EASN_A7R1)) return false;

		if(CfgParams.Init(szDataSource,"[B7R1]")==0) return false;
		if(!SetupStationMetrologyPoints(CfgParams,EASN_B7R1)) return false;

		// resolve Romain Pots parameters
		eRPName=ERPN_B7L1U;
		if(CfgParams.Init(szDataSource,"[B7L1U]")==0) return false;
		if(!SetupRPMetrologyPoints(CfgParams,eRPName)) return false;
		if(!SetupDetMetrologyPoints(CfgParams,eRPName)) return false;

		eRPName=ERPN_B7L1L;
		if(CfgParams.Init(szDataSource,"[B7L1L]")==0) return false;
		if(!SetupRPMetrologyPoints(CfgParams,eRPName)) return false;
		if(!SetupDetMetrologyPoints(CfgParams,eRPName)) return false;

		eRPName=ERPN_A7L1U;
		if(CfgParams.Init(szDataSource,"[A7L1U]")==0) return false;
		if(!SetupRPMetrologyPoints(CfgParams,eRPName)) return false;
		if(!SetupDetMetrologyPoints(CfgParams,eRPName)) return false;

		eRPName=ERPN_A7L1L;
		if(CfgParams.Init(szDataSource,"[A7L1L]")==0) return false;
		if(!SetupRPMetrologyPoints(CfgParams,eRPName)) return false;
		if(!SetupDetMetrologyPoints(CfgParams,eRPName)) return false;

		eRPName=ERPN_A7R1U;
		if(CfgParams.Init(szDataSource,"[A7R1U]")==0) return false;
		if(!SetupRPMetrologyPoints(CfgParams,eRPName)) return false;
		if(!SetupDetMetrologyPoints(CfgParams,eRPName)) return false;

		eRPName=ERPN_A7R1L;
		if(CfgParams.Init(szDataSource,"[A7R1L]")==0) return false;
		if(!SetupRPMetrologyPoints(CfgParams,eRPName)) return false;
		if(!SetupDetMetrologyPoints(CfgParams,eRPName)) return false;

		eRPName=ERPN_B7R1U;
		if(CfgParams.Init(szDataSource,"[B7R1U]")==0) return false;
		if(!SetupRPMetrologyPoints(CfgParams,eRPName)) return false;
		if(!SetupDetMetrologyPoints(CfgParams,eRPName)) return false;

		eRPName=ERPN_B7R1L;
		if(CfgParams.Init(szDataSource,"[B7R1L]")==0) return false;
		if(!SetupRPMetrologyPoints(CfgParams,eRPName)) return false;
		if(!SetupDetMetrologyPoints(CfgParams,eRPName)) return false;

		bRes=true;
	}
	else
	{
		LogStream<<MSG::ERROR<<"The file source is supported only."<<endmsg;
		bRes=false;
	}

	return bRes;
}

bool ALFA_GeometryReader::ResolveRPotRefPoints(const char* szvalue, eRPotName eRPName, eRefPointType eRPointType)
{
	bool bRes=false;
	std::vector<HepGeom::Point3D<double> > vecRefPoints;

	if(eRPointType==ERPT_IDEAL || eRPointType==ERPT_REAL) bRes=ParseRefPoints(szvalue,vecRefPoints,EMCS_STATION);
	else if(eRPointType==ERPT_DETIDEAL || eRPointType==ERPT_DETREAL) bRes=ParseRefPoints(szvalue,vecRefPoints,EMCS_ROMANPOT);

	if(bRes==true){
		if(eRPointType==ERPT_IDEAL) m_RPPosParams[eRPName].VecIdealRPRefPoints=vecRefPoints;
		else if(eRPointType==ERPT_REAL) m_RPPosParams[eRPName].VecRealRPRefPoints=vecRefPoints;
		else if(eRPointType==ERPT_DETIDEAL) m_RPPosParams[eRPName].VecIdealDetRefPoints=vecRefPoints;
		else if(eRPointType==ERPT_DETREAL) m_RPPosParams[eRPName].VecRealDetRefPoints=vecRefPoints;
	}

	return bRes;
}

bool ALFA_GeometryReader::ParseRefPoints(const char* szvalue, std::vector<HepGeom::Point3D<double> >& vecRefPoints, eMetrologyCoordSystem eCSystem)
{
	double fx,fy,fz,faux;
	char *ppos1,*ppos2,*ppos3,*ppos4,*pstop;
	char szbuff[512];
	HepGeom::Point3D<double> RefPoint;

	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeometryReader::ParseRefPoints");

	memset(szbuff,0,sizeof(szbuff));
	memcpy(szbuff,szvalue,strlen(szvalue));

	//get point count
	int i,nCnt=0;
	ppos2=szbuff;
	while((ppos1=strchr(ppos2,'['))!=NULL){
		if(!(ppos2=strchr(ppos1+1,']'))) return false;
		ppos2++;
		nCnt++;
	}

	if(nCnt==0) return false;
	vecRefPoints.resize(nCnt);

	i=0;
	ppos2=szbuff;
	while((ppos1=strchr(ppos2,'['))!=NULL){
		if(!(ppos2=strchr(ppos1,']'))) return false;

		//x-coordinate
		ppos3=ppos1+1;
		if(!(ppos4=strchr(ppos3+1,','))) return false;
		*ppos4=0;
		faux=strtod(ppos3,&pstop);
		if(pstop!=ppos4 && *pstop!=' ') return false;
		else fx=faux;

		//y-coordinate
		ppos3=ppos4+1;
		if(!(ppos4=strchr(ppos3+1,','))) return false;
		*ppos4=0;
		faux=strtod(ppos3,&pstop);
		if(pstop!=ppos4 && *pstop!=' ') return false;
		else fy=faux;

		//z-coordinate
		ppos3=ppos4+1;
		if(!(ppos4=strchr(ppos3+1,']'))) return false;
		*ppos4=0;
		faux=strtod(ppos3,&pstop);
		if(pstop!=ppos4 && *pstop!=' ') return false;
		else fz=faux;

		//rearanging coordinates to ATLAS scheme
		if(eCSystem==EMCS_ATLAS){
			RefPoint[0]=fx; RefPoint[1]=fy; RefPoint[2]=fz; // no swap
		}
		else if(eCSystem==EMCS_STATION){
			RefPoint[0]=fx; RefPoint[1]=fz; RefPoint[2]=-fy;
		}
		else if(eCSystem==EMCS_ROMANPOT){
			RefPoint[0]=fx; RefPoint[1]=-fz; RefPoint[2]=fy;
		}
		else if(eCSystem==EMCS_DETPIN1){
			RefPoint[0]=fx; RefPoint[1]=fy; RefPoint[2]=fz; // no swap
		}
		else throw new GaudiException("Invalid type of metrology coordinate system", "ALFA_GeometryReader::ParseRefPoints", StatusCode::FAILURE);

		vecRefPoints[i++]=RefPoint;

		ppos2++;
	}

	return true;
}

bool ALFA_GeometryReader::ParseArrayOfValues(const char* szvalue, std::vector<double>& vecValues)
{
	bool bRes=true;
	double faux;
	char *ppos1,*ppos2,*pstop;
	char szbuff[512];

	vecValues.clear();
	memset(szbuff,0,sizeof(szbuff));
	if(szvalue) strncpy(szbuff,szvalue,sizeof(szbuff)-1);

	ppos1=szbuff;
	ppos2=strchr(ppos1,',');
	if(ppos2!=NULL){
		while(ppos2!=NULL){
			*ppos2=0;

			faux=strtod(ppos1,&pstop);
			if(pstop==ppos2) vecValues.push_back(faux);
			else{
				bRes=false;
				break;
			}

			ppos1=ppos2+1;
			ppos2=strchr(ppos1,',');
		}

		//load last value
		faux=strtod(ppos1,&pstop);
		if(*pstop==0) vecValues.push_back(faux);
		else bRes=false;
	}

	if(bRes==false) vecValues.clear();

	return bRes;
}

bool ALFA_GeometryReader::SetupDetMetrologyPoints(ALFA_ConfigParams& CfgParams, eRPotName eRPName)
{
	//bool bIsLightMetrology=true;

	double fZc;
	//double a1,a2;
	double a3,a4;
	double xp1,yp1;
	//double xp2,yp2,xp3,yp3,xr3;
	double xr1,yr1,xr2,yr2;//,yr3;
	HepGeom::Point3D<double> PointIdealD3, PointRealD3;
	HepGeom::Point3D<double> PointIdealD4, PointRealD4;
	HepGeom::Point3D<double> PointIdealD5, PointRealD5;

	std::vector<double> vecDetEdges;
	std::vector<HepGeom::Point3D<double> > vecNominalDetPoints;
	std::vector<HepGeom::Point3D<double> > vecRealDetPoints;

	// load nominal & real position of PSx in RP CS
	if(!CfgParams.IsKey("idealrefdetpoints")) return false;
	if(!ParseRefPoints(CfgParams.GetParameter("idealrefdetpoints"),vecNominalDetPoints,EMCS_ROMANPOT)) return false;
	if(!CfgParams.IsKey("realrefdetpoints")) return false;
	if(!ParseRefPoints(CfgParams.GetParameter("realrefdetpoints"),vecRealDetPoints,EMCS_ROMANPOT)) return false;

	if(!CfgParams.IsKey("realdetedgeypos")) return false;
	if(!ParseArrayOfValues(CfgParams.GetParameter("realdetedgeypos"),vecDetEdges)) return false;

	fZc=0.5*(vecDetEdges[0]+vecDetEdges[1]);
	PointIdealD3=HepGeom::Point3D<double>(10.0,0.0,0.0);
	PointIdealD4=HepGeom::Point3D<double>(0.0,10.0,0.0);
	PointIdealD5=HepGeom::Point3D<double>(0.0,0.0,10.0);

	//compute in-plane transformation matrix (deformation not included)
	xp1=vecNominalDetPoints[0].x();
	yp1=vecNominalDetPoints[0].y();
	//xp2=vecNominalDetPoints[1].x();
	//yp2=vecNominalDetPoints[1].y();
	//xp3=vecNominalDetPoints[2].x();
	//yp3=vecNominalDetPoints[2].y();

	xr1=vecRealDetPoints[0].x();
	yr1=vecRealDetPoints[0].y();
	xr2=vecRealDetPoints[1].x();
	yr2=vecRealDetPoints[1].y();
	//xr3=vecRealDetPoints[2].x();
	//yr3=vecRealDetPoints[2].y();

	double alpha=-atan((xr2-xr1)/(yr2-yr1));
	//a1=cos(alpha); a2=-sin(alpha);
	a3=xr1-xp1;//a3=0.5*(xr1+xr2);
	a4=yr1-yp1;//a4=0.5*(yr1+yr2);

	//double beta=-asin((vecDetEdges[1]-vecDetEdges[0])/90.0);
	//double gamma=asin((yr3-yr1)/8.0);
	//double dz=135.0*sin(gamma);

	//-----------------------------------------
	HepGeom::Transform3D CorrectionInMainPoint, CorrectionInDetPin1;

	//HepGeom::Transform3D CorrectionInDetPin1=HepTransform3D();
	//use light metrology
	CorrectionInDetPin1=HepGeom::Translate3D(a3,a4,fZc-113.0*CLHEP::mm)*HepGeom::RotateZ3D(alpha);
	//CorrectionInDetPin1=HepGeom::RotateX3D(gamma)*HepGeom::RotateY3D(beta)*HepGeom::Translate3D(a3,a4,dz+fZc-113.0*CLHEP::mm)*HepGeom::RotateZ3D(alpha);

	//HepGeom::Point3D<double> MainPoint=m_RPPosParams[eRPName].RPIdealTransformInStation*HepGeom::Point3D<double>(0.0,0.0,0.0);
	HepGeom::Point3D<double> MainPoint=ms_NominalRPMainPoint;
	HepGeom::Point3D<double> RPPin1=ms_NominalRPPin1;
	HepGeom::Point3D<double> DetPin1=ms_NominalDetPin1;

	//recalculate SW transform in MainPoint
	HepGeom::Vector3D<double> OriginShift=RPPin1+DetPin1-MainPoint;

	CLHEP::Hep3Vector TranslationInMainPoint=CorrectionInDetPin1.getTranslation()-CorrectionInDetPin1*OriginShift+OriginShift;

	CLHEP::HepRotation RotationInMainPoint=CorrectionInDetPin1.getRotation();
	CorrectionInMainPoint=HepGeom::Transform3D(RotationInMainPoint,TranslationInMainPoint);

	PointRealD3=CorrectionInMainPoint*PointIdealD3;
	PointRealD4=CorrectionInMainPoint*PointIdealD4;
	PointRealD5=CorrectionInMainPoint*PointIdealD5;

	m_RPPosParams[eRPName].VecIdealDetRefPoints.resize(3);
	m_RPPosParams[eRPName].VecRealDetRefPoints.resize(3);

	m_RPPosParams[eRPName].VecIdealDetRefPoints[0]=PointIdealD5;
	m_RPPosParams[eRPName].VecRealDetRefPoints[0]=PointRealD5;

	m_RPPosParams[eRPName].VecIdealDetRefPoints[1]=PointIdealD4;
	m_RPPosParams[eRPName].VecRealDetRefPoints[1]=PointRealD4;

	m_RPPosParams[eRPName].VecIdealDetRefPoints[2]=PointIdealD3;
	m_RPPosParams[eRPName].VecRealDetRefPoints[2]=PointRealD3;

	return true;
}

bool ALFA_GeometryReader::SetupRPMetrologyPoints(ALFA_ConfigParams& CfgParams, eRPotName eRPName)
{
	bool bRes=true;
	const int nCnt=3;
	int i;
	double fx,fy,fz;
	double fCurrentLVDTmm;
	HepGeom::Point3D<double> PSinStation, RPPinNominal;
	double fCurrentLVDT=0.0;
	std::vector<double> vecPolyFitParams;
	std::vector<HepGeom::Point3D<double> > vecPSinRPCS;
	std::vector<HepGeom::Point3D<double> > vecNominalPSinStationCS;
	std::vector<HepGeom::Point3D<double> > vecRealPSinStationCS;
	RPPOSPARAMS RPPosParams;

	GetRPPosParams(&RPPosParams,eRPName);
	if(RPPosParams.bIsLow==false){
		RPPinNominal=HepGeom::Point3D<double>(77.5*CLHEP::mm,172.2*CLHEP::mm,-124.0*CLHEP::mm);
	}
	else{
		RPPinNominal=HepGeom::Point3D<double>(77.5*CLHEP::mm,-172.2*CLHEP::mm,+124.0*CLHEP::mm);
	}

	vecNominalPSinStationCS.resize(nCnt);
	vecRealPSinStationCS.resize(nCnt);

	// load nominal position of PSx in RP CS
	if(!CfgParams.IsKey("psinrpss")) return false;
	if(!ParseRefPoints(CfgParams.GetParameter("psinrpss"),vecPSinRPCS,EMCS_ROMANPOT)) return false;

	//compute nominal position of PSx in Station CS
	for(i=0;i<nCnt;i++){
		vecNominalPSinStationCS[i]=RPPinNominal+HepGeom::RotateX3D(RPPosParams.bIsLow? 180*CLHEP::deg:0.0)*vecPSinRPCS[i];
		//HepGeom::Point3D<double> auxPoint=vecNominalPSinStationCS[i];
		//int a=0;
	}

	//----- test
	//	if(RPPosParams.bIsLow){
	//		vecNominalPSinStationCS[0]=HepGeom::Point3D<double>(+132.0*CLHEP::mm,-170.0*CLHEP::mm,-132.0*CLHEP::mm);
	//		vecNominalPSinStationCS[1]=HepGeom::Point3D<double>(+132.0*CLHEP::mm,-170.0*CLHEP::mm,+132.0*CLHEP::mm);
	//		vecNominalPSinStationCS[2]=HepGeom::Point3D<double>(-132.0*CLHEP::mm,-170.0*CLHEP::mm,+132.0*CLHEP::mm);
	//	}
	//	else{
	//		vecNominalPSinStationCS[0]=HepGeom::Point3D<double>(+132.0*CLHEP::mm,-170.0*CLHEP::mm,+132.0*CLHEP::mm);
	//		vecNominalPSinStationCS[1]=HepGeom::Point3D<double>(+132.0*CLHEP::mm,-170.0*CLHEP::mm,-132.0*CLHEP::mm);
	//		vecNominalPSinStationCS[2]=HepGeom::Point3D<double>(-132.0*CLHEP::mm,-170.0*CLHEP::mm,+132.0*CLHEP::mm);
	//	}
	//----- test

	//calculate LVDT position from position in [mm]
	fCurrentLVDTmm=RPPosParams.fCurrentLVDTmm;
	//if(m_bIsUserLVDTEnabled) fCurrentLVDTmm=RPPosParams.fCurrentLVDTmm;
	//else fCurrentLVDTmm=atof(CfgParams.GetParameter("currentlvdt")); //value in [mm] !

	ParseArrayOfValues(CfgParams.GetParameter("mm2lvdtfit"),vecPolyFitParams);
	fCurrentLVDT=GetPolyFitValue(fCurrentLVDTmm,vecPolyFitParams); // true LVDT value

	//compute real position of PSx in Station CS from LVDT parametrization
	ParseArrayOfValues(CfgParams.GetParameter("ps1xlvdtpolyfit"),vecPolyFitParams);
	fx=GetPolyFitValue(fCurrentLVDT,vecPolyFitParams);
	ParseArrayOfValues(CfgParams.GetParameter("ps1ylvdtpolyfit"),vecPolyFitParams);
	fy=GetPolyFitValue(fCurrentLVDT,vecPolyFitParams);
	ParseArrayOfValues(CfgParams.GetParameter("ps1zlvdtpolyfit"),vecPolyFitParams);
	fz=GetPolyFitValue(fCurrentLVDT,vecPolyFitParams);
	PSinStation.set(fx,fy,fz);
	vecRealPSinStationCS[0]=PSinStation;

	ParseArrayOfValues(CfgParams.GetParameter("ps2xlvdtpolyfit"),vecPolyFitParams);
	fx=GetPolyFitValue(fCurrentLVDT,vecPolyFitParams);
	ParseArrayOfValues(CfgParams.GetParameter("ps2ylvdtpolyfit"),vecPolyFitParams);
	fy=GetPolyFitValue(fCurrentLVDT,vecPolyFitParams);
	ParseArrayOfValues(CfgParams.GetParameter("ps2zlvdtpolyfit"),vecPolyFitParams);
	fz=GetPolyFitValue(fCurrentLVDT,vecPolyFitParams);
	PSinStation.set(fx,fy,fz);
	vecRealPSinStationCS[1]=PSinStation;

	ParseArrayOfValues(CfgParams.GetParameter("ps3xlvdtpolyfit"),vecPolyFitParams);
	fx=GetPolyFitValue(fCurrentLVDT,vecPolyFitParams);
	ParseArrayOfValues(CfgParams.GetParameter("ps3ylvdtpolyfit"),vecPolyFitParams);
	fy=GetPolyFitValue(fCurrentLVDT,vecPolyFitParams);
	ParseArrayOfValues(CfgParams.GetParameter("ps3zlvdtpolyfit"),vecPolyFitParams);
	fz=GetPolyFitValue(fCurrentLVDT,vecPolyFitParams);
	PSinStation.set(fx,fy,fz);
	vecRealPSinStationCS[2]=PSinStation;

	//----- test
	//vecRealPSinStationCS[0]=vecNominalPSinStationCS[0]+HepGeom::Point3D<double>(0.0*CLHEP::mm,0.0*CLHEP::mm,0.0*CLHEP::mm);
	//vecRealPSinStationCS[1]=vecNominalPSinStationCS[1]+HepGeom::Point3D<double>(0.0*CLHEP::mm,0.0*CLHEP::mm,0.0*CLHEP::mm);
	//vecRealPSinStationCS[2]=vecNominalPSinStationCS[2]+HepGeom::Point3D<double>(0.0*CLHEP::mm,0.1*CLHEP::mm,0.0*CLHEP::mm);
	//----- test

	//m_RPPosParams[eRPName].fCurrentLVDTmm=fCurrentLVDTmm;
	m_RPPosParams[eRPName].VecIdealRPRefPoints=vecNominalPSinStationCS;
	m_RPPosParams[eRPName].VecRealRPRefPoints=vecRealPSinStationCS;

	bRes=true;

	return bRes;
}

bool ALFA_GeometryReader::SetupStationMetrologyPoints(ALFA_ConfigParams &CfgParams, eAStationName eASName)
{
	std::vector<HepGeom::Point3D<double> > vecShiftPoints;

	// load shift E
	if(!CfgParams.IsKey("shifte")) return false;
	if(!ParseRefPoints(CfgParams.GetParameter("shifte"),vecShiftPoints,EMCS_ATLAS)) return false;
	m_ASPosParams[eASName].ShiftE=vecShiftPoints[0];

	// load shift S
	if(!CfgParams.IsKey("shifts")) return false;
	if(!ParseRefPoints(CfgParams.GetParameter("shifts"),vecShiftPoints,EMCS_ATLAS)) return false;
	m_ASPosParams[eASName].ShiftS=vecShiftPoints[0];

	return true;
}

double ALFA_GeometryReader::GetPolyFitValue(const double fInputValue, const std::vector<double>& vecPolyFitParams)
{
	int i,n;
	double fOutputValue=0.0;

	n=vecPolyFitParams.size();

	for(i=0;i<n;i++)
	{
		fOutputValue+=vecPolyFitParams[i]*pow(fInputValue,n-1-i);
	}

	return fOutputValue;
}

HepGeom::Point3D<double> ALFA_GeometryReader::GetDetPointInAtlas(eRPotName eRPName, const HepGeom::Point3D<double>& PointInDetCS)
{
	RPPOSPARAMS RPPosParams;
	ASPOSPARAMS ASPosParams;

	HepGeom::Point3D<double> RPPin1=ms_NominalRPPin1;//HepGeom::Point3D<double>(+77.5*mm,+172.2*mm,-114.0*mm);
	HepGeom::Point3D<double> MainPoint=ms_NominalRPMainPoint;//HepGeom::Point3D<double>(0.0*mm,31.025*mm,18.0*mm);
	HepGeom::Point3D<double> AlfaRefPoint=ms_NominalAlfaRefPoint;//HepGeom::Point3D<double>(-77.5*mm,-35.7*mm,114.0*mm);
	HepGeom::Point3D<double> PointInAtlasCS, PointInRPotCS;

	if(m_eFCoordSystem==EFCS_ATLAS || m_eFCoordSystem==EFCS_CLADDING){
		GetRPPosParams(&RPPosParams, eRPName);
		GetASPosParams(&ASPosParams, RPPosParams.eASName);
		HepGeom::Vector3D<double> Shift=RPPin1-MainPoint;
		HepGeom::Point3D<double> PointInMainPoint=Shift+AlfaRefPoint+PointInDetCS;
		HepGeom::Point3D<double> PointInRPotCS=RPPosParams.DetTransformInMainPoint*PointInMainPoint; //wrt. MainPoint
		HepGeom::Transform3D TotTransform=ASPosParams.ASTransformInATLAS*RPPosParams.RPTransformInStation;
		PointInAtlasCS=TotTransform*PointInRPotCS;
	}
	else{
		throw new GaudiException(" The GetDetPointInAtlas() can be used only with EFCS_ATLAS or EFCS_CLADDING flag ", "ALFA_GeometryReader::GetDetPointInAtlas", StatusCode::FAILURE);
	}
	/*
    RPPOSPARAMS RPPosParams;
    ASPOSPARAMS ASPosParams;
	HepGeom::Point3D<double> AlfaRefPoint=HepGeom::Point3D<double>(0.0*CLHEP::mm,+105.475*CLHEP::mm,-18.0*CLHEP::mm);
	HepGeom::Point3D<double> PointInAtlasCS;

	if(m_eFCoordSystem==EFCS_ATLAS || m_eFCoordSystem==EFCS_CLADDING){
		GetRPPosParams(&RPPosParams, eRPName);
		GetASPosParams(&ASPosParams, RPPosParams.eASName);

		CLHEP::Point3D<double> PointInDetMainPoint=AlfaRefPoint+PointInDetCS;
		HepGeom::Transform3D TotTransform=ASPosParams.ASTransformInATLAS*RPPosParams.RPTransformInStation*RPPosParams.DetTransformInMainPoint;
		PointInAtlasCS=TotTransform*PointInDetMainPoint;
		HepGeom::Point3D<double> auxPoint=PointInAtlasCS;
		int a=0;
    }
    else{
		throw new GaudiException(" The GetDetPointInAtlas() can be used only with EFCS_ATLAS or EFCS_CLADDING flag ", "ALFA_GeometryReader::GetDetPointInAtlas", StatusCode::FAILURE);
	}
*/
    return PointInAtlasCS;
}

HepGeom::Point3D<double> ALFA_GeometryReader::GetDetPointInRPot(eRPotName eRPName, const HepGeom::Point3D<double>& PointInDetCS)
{
	RPPOSPARAMS RPPosParams;
	HepGeom::Point3D<double> RPPin1=ms_NominalRPPin1;
	HepGeom::Point3D<double> MainPoint=ms_NominalRPMainPoint;
	HepGeom::Point3D<double> AlfaRefPoint=ms_NominalAlfaRefPoint;
	HepGeom::Point3D<double> PointInRPotCS;

	if(m_eFCoordSystem==EFCS_ATLAS || m_eFCoordSystem==EFCS_CLADDING){
		GetRPPosParams(&RPPosParams, eRPName);

		HepGeom::Vector3D<double> Shift=RPPin1-MainPoint;
		HepGeom::Point3D<double> PointInMainPoint=Shift+AlfaRefPoint+PointInDetCS;
		PointInRPotCS=RPPosParams.DetTransformInMainPoint*PointInMainPoint-Shift; // wrt. RPPin1
	}
	else{
		throw new GaudiException(" The GetDetPointInAtlas() can be used only with EFCS_ATLAS or EFCS_CLADDING flag ", "ALFA_GeometryReader::GetDetPointInAtlas", StatusCode::FAILURE);
	}

	return PointInRPotCS;
}


bool ALFA_GeometryReader::SaveRPGeometryParams(const eRPotName eRPName, const char* szDataDestination)
{
	bool bRes= false;
	double fRotX,fRotY,fRotZ;

	FILE *pfile=fopen(szDataDestination,"w");
	if(pfile!=NULL)
	{
		fprintf(pfile,"Romain pot geometry info: ----------------------------------------\r\n");

		fprintf(pfile,"LVDT: %.3f mm\r\n\r\n",m_RPPosParams[eRPName].fCurrentLVDTmm);

		fprintf(pfile,"Transformation matrix of RP in station MainPoint:\r\n");
		fprintf(pfile,"%+.5f %+.5f %+.5f %+.5f\r\n",m_RPPosParams[eRPName].RPTransformInStation.xx(),m_RPPosParams[eRPName].RPTransformInStation.xy(),m_RPPosParams[eRPName].RPTransformInStation.xz(),m_RPPosParams[eRPName].RPTransformInStation.dx());
		fprintf(pfile,"%+.5f %+.5f %+.5f %+.5f\r\n",m_RPPosParams[eRPName].RPTransformInStation.yx(),m_RPPosParams[eRPName].RPTransformInStation.yy(),m_RPPosParams[eRPName].RPTransformInStation.yz(),m_RPPosParams[eRPName].RPTransformInStation.dy());
		fprintf(pfile,"%+.5f %+.5f %+.5f %+.5f\r\n",m_RPPosParams[eRPName].RPTransformInStation.zx(),m_RPPosParams[eRPName].RPTransformInStation.zy(),m_RPPosParams[eRPName].RPTransformInStation.zz(),m_RPPosParams[eRPName].RPTransformInStation.dz());

		fRotZ=atan2(m_RPPosParams[eRPName].RPTransformInStation.yx(),m_RPPosParams[eRPName].RPTransformInStation.xx());
		fRotY=atan2(-m_RPPosParams[eRPName].RPTransformInStation.zx(),sqrt(pow(m_RPPosParams[eRPName].RPTransformInStation.zy(),2)+pow(m_RPPosParams[eRPName].RPTransformInStation.zz(),2)));//asin(m_RPPosParams[eRPName].RPTransformInStation.zx());
		fRotX=atan2(m_RPPosParams[eRPName].RPTransformInStation.zy(),m_RPPosParams[eRPName].RPTransformInStation.zz());
		fprintf(pfile,"RotX=%.5f rad, RotY=%.5f rad, RotZ=%.5f rad\r\n",fRotX,fRotY,fRotZ);

		/*
		fprintf(pfile,"\r\nMetrology scale matrix of RP in station MainPoint:\r\n");
		fprintf(pfile,"%+.5f %+.5f %+.5f %+.5f\r\n",m_RPPosParams[eRPName].RPScaleInStation.xx(),m_RPPosParams[eRPName].RPScaleInStation.xy(),m_RPPosParams[eRPName].RPScaleInStation.xz(),m_RPPosParams[eRPName].RPScaleInStation.dx());
		fprintf(pfile,"%+.5f %+.5f %+.5f %+.5f\r\n",m_RPPosParams[eRPName].RPScaleInStation.yx(),m_RPPosParams[eRPName].RPScaleInStation.yy(),m_RPPosParams[eRPName].RPScaleInStation.yz(),m_RPPosParams[eRPName].RPScaleInStation.dy());
		fprintf(pfile,"%+.5f %+.5f %+.5f %+.5f\r\n",m_RPPosParams[eRPName].RPScaleInStation.zx(),m_RPPosParams[eRPName].RPScaleInStation.zy(),m_RPPosParams[eRPName].RPScaleInStation.zz(),m_RPPosParams[eRPName].RPScaleInStation.dz());
		*/

		fprintf(pfile,"\r\nDetector geometry info: ------------------------------------------\r\n");
		fprintf(pfile,"Transformation matrix of detector in RP MainPoint:\r\n");
		fprintf(pfile,"%+.5f %+.5f %+.5f %+.5f\r\n",m_RPPosParams[eRPName].DetTransformInMainPoint.xx(),m_RPPosParams[eRPName].DetTransformInMainPoint.xy(),m_RPPosParams[eRPName].DetTransformInMainPoint.xz(),m_RPPosParams[eRPName].DetTransformInMainPoint.dx());
		fprintf(pfile,"%+.5f %+.5f %+.5f %+.5f\r\n",m_RPPosParams[eRPName].DetTransformInMainPoint.yx(),m_RPPosParams[eRPName].DetTransformInMainPoint.yy(),m_RPPosParams[eRPName].DetTransformInMainPoint.yz(),m_RPPosParams[eRPName].DetTransformInMainPoint.dy());
		fprintf(pfile,"%+.5f %+.5f %+.5f %+.5f\r\n",m_RPPosParams[eRPName].DetTransformInMainPoint.zx(),m_RPPosParams[eRPName].DetTransformInMainPoint.zy(),m_RPPosParams[eRPName].DetTransformInMainPoint.zz(),m_RPPosParams[eRPName].DetTransformInMainPoint.dz());

		fRotZ=atan2(m_RPPosParams[eRPName].DetTransformInMainPoint.yx(),m_RPPosParams[eRPName].DetTransformInMainPoint.xx());
		fRotY=atan2(-m_RPPosParams[eRPName].DetTransformInMainPoint.zx(),sqrt(pow(m_RPPosParams[eRPName].DetTransformInMainPoint.zy(),2)+pow(m_RPPosParams[eRPName].DetTransformInMainPoint.zz(),2)));//asin(m_RPPosParams[eRPName].RPTransformInStation.zx());
		fRotX=atan2(m_RPPosParams[eRPName].DetTransformInMainPoint.zy(),m_RPPosParams[eRPName].DetTransformInMainPoint.zz());
		fprintf(pfile,"RotX=%.5f rad, RotY=%.5f rad, RotZ=%.5f rad\r\n",fRotX,fRotY,fRotZ);

		/*
		fprintf(pfile,"\r\nMetrology scale matrix of detector in RP MainPoint:\r\n");
		fprintf(pfile,"%+.5f %+.5f %+.5f %+.5f\r\n",m_RPPosParams[eRPName].DetScaleInRP.xx(),m_RPPosParams[eRPName].DetScaleInRP.xy(),m_RPPosParams[eRPName].DetScaleInRP.xz(),m_RPPosParams[eRPName].DetScaleInRP.dx());
		fprintf(pfile,"%+.5f %+.5f %+.5f %+.5f\r\n",m_RPPosParams[eRPName].DetScaleInRP.yx(),m_RPPosParams[eRPName].DetScaleInRP.yy(),m_RPPosParams[eRPName].DetScaleInRP.yz(),m_RPPosParams[eRPName].DetScaleInRP.dy());
		fprintf(pfile,"%+.5f %+.5f %+.5f %+.5f\r\n",m_RPPosParams[eRPName].DetScaleInRP.zx(),m_RPPosParams[eRPName].DetScaleInRP.zy(),m_RPPosParams[eRPName].DetScaleInRP.zz(),m_RPPosParams[eRPName].DetScaleInRP.dz());
		*/
		fprintf(pfile,"\r\nReference pins and Track point info: ------------------------------------------\r\n");
		fprintf(pfile,"RPPin1: nominal=[%.3f,%.3f,%.3f], real=[%.3f,%.3f,%.3f] (Station CS)\r\n",m_RPPosParams[eRPName].RefPins.IdealRPPin1.x(),m_RPPosParams[eRPName].RefPins.IdealRPPin1.y(),m_RPPosParams[eRPName].RefPins.IdealRPPin1.z(),
				m_RPPosParams[eRPName].RefPins.RealRPPin1.x(),m_RPPosParams[eRPName].RefPins.RealRPPin1.y(),m_RPPosParams[eRPName].RefPins.RealRPPin1.z());
		fprintf(pfile,"RPPin2: nominal=[%.3f,%.3f,%.3f], real=[%.3f,%.3f,%.3f] (Station CS)\r\n",m_RPPosParams[eRPName].RefPins.IdealRPPin2.x(),m_RPPosParams[eRPName].RefPins.IdealRPPin2.y(),m_RPPosParams[eRPName].RefPins.IdealRPPin2.z(),
				m_RPPosParams[eRPName].RefPins.RealRPPin2.x(),m_RPPosParams[eRPName].RefPins.RealRPPin2.y(),m_RPPosParams[eRPName].RefPins.RealRPPin2.z());
		fprintf(pfile,"RPPin3: nominal=[%.3f,%.3f,%.3f], real=[%.3f,%.3f,%.3f] (Station CS)\r\n",m_RPPosParams[eRPName].RefPins.IdealRPPin3.x(),m_RPPosParams[eRPName].RefPins.IdealRPPin3.y(),m_RPPosParams[eRPName].RefPins.IdealRPPin3.z(),
				m_RPPosParams[eRPName].RefPins.RealRPPin3.x(),m_RPPosParams[eRPName].RefPins.RealRPPin3.y(),m_RPPosParams[eRPName].RefPins.RealRPPin3.z());
		fprintf(pfile,"Detector TrackPoint: [%.3f,%.3f,%.3f] (ALFA CS), [%.3f,%.3f,%.3f] (RPot CS), [%.3f,%.3f,%.3f] (ATLAS CS)\r\n",m_RPPosParams[eRPName].RefPins.DTPInAlfaCS.x(),m_RPPosParams[eRPName].RefPins.DTPInAlfaCS.y(),m_RPPosParams[eRPName].RefPins.DTPInAlfaCS.z(),
				m_RPPosParams[eRPName].RefPins.DTPInRPotCS.x(),m_RPPosParams[eRPName].RefPins.DTPInRPotCS.y(),m_RPPosParams[eRPName].RefPins.DTPInRPotCS.z(),
				m_RPPosParams[eRPName].RefPins.DTPInAtlasCS.x(),m_RPPosParams[eRPName].RefPins.DTPInAtlasCS.y(),m_RPPosParams[eRPName].RefPins.DTPInAtlasCS.z());
		fprintf(pfile,"Detector CentrePoint: [%.3f,%.3f,%.3f] (ALFA CS), [%.3f,%.3f,%.3f] (RPot CS), [%.3f,%.3f,%.3f] (ATLAS CS)\r\n",m_RPPosParams[eRPName].RefPins.DCPInAlfaCS.x(),m_RPPosParams[eRPName].RefPins.DCPInAlfaCS.y(),m_RPPosParams[eRPName].RefPins.DCPInAlfaCS.z(),
				m_RPPosParams[eRPName].RefPins.DCPInRPotCS.x(),m_RPPosParams[eRPName].RefPins.DCPInRPotCS.y(),m_RPPosParams[eRPName].RefPins.DCPInRPotCS.z(),
				m_RPPosParams[eRPName].RefPins.DCPInAtlasCS.x(),m_RPPosParams[eRPName].RefPins.DCPInAtlasCS.y(),m_RPPosParams[eRPName].RefPins.DCPInAtlasCS.z());


		fclose(pfile);

		bRes=true;
	}

	return bRes;
}

HepGeom::Transform3D ALFA_GeometryReader::GetTransformMatrix(const eRPotName eRPName, const eTransformElement eMatrixType)
{
	HepGeom::Transform3D TransMatrix;
	ASPOSPARAMS AParams;
	RPPOSPARAMS RPParams;

	HepGeom::Point3D<double> RPPin1=ms_NominalRPPin1;
	HepGeom::Point3D<double> MainPoint=ms_NominalRPMainPoint;
	HepGeom::Point3D<double> AlfaRefPoint=ms_NominalAlfaRefPoint;

	GetRPPosParams(&RPParams, eRPName);
	GetASPosParams(&AParams, RPParams.eASName);

	HepGeom::Vector3D<double> Shift1=RPPin1-MainPoint;
	HepGeom::Vector3D<double> Shift2=RPPin1-MainPoint+AlfaRefPoint;
	HepGeom::Vector3D<double> Shift3=0.5*(AParams.ShiftE+AParams.ShiftS);

	switch(eMatrixType)
	{
	case ETE_A1:
		TransMatrix=RPParams.RPTransformInStation;
		break;
	case ETE_A2:
		TransMatrix=RPParams.DetTransformInMainPoint;
		break;
	case ETE_A3:
		TransMatrix=AParams.ASTransformInATLAS;
		break;
	case ETE_T1:
		TransMatrix=HepGeom::Translate3D(-Shift1);
		break;
	case ETE_T2:
		TransMatrix=HepGeom::Translate3D(Shift2);
		break;
	case ETE_T3:
		TransMatrix=HepGeom::Translate3D(-Shift3);
		break;
	default:
		throw GaudiException(" Invalid matrix identificator ", "ALFA_GeometryReader::GetTransformMatrix", StatusCode::FAILURE);
		break;
	}

	return TransMatrix;
}

double ALFA_GeometryReader::GetRPotZPosInAtlas(const eRPotName eRPName)
{
	RPPOSPARAMS RPParams;
	GetRPPosParams(&RPParams, eRPName);

	return RPParams.RefPins.DCPInAtlasCS.z();
}

void ALFA_GeometryReader::SetupCurrentLVDT(const PGEOMETRYCONFIGURATION pConfig)
{
	m_RPPosParams[ERPN_B7L1U].fCurrentLVDTmm=pConfig->CfgRPosParams[0].fCurrentLVDTmm;
	m_RPPosParams[ERPN_B7L1L].fCurrentLVDTmm=pConfig->CfgRPosParams[1].fCurrentLVDTmm;
	m_RPPosParams[ERPN_A7L1U].fCurrentLVDTmm=pConfig->CfgRPosParams[2].fCurrentLVDTmm;
	m_RPPosParams[ERPN_A7L1L].fCurrentLVDTmm=pConfig->CfgRPosParams[3].fCurrentLVDTmm;
	m_RPPosParams[ERPN_A7R1U].fCurrentLVDTmm=pConfig->CfgRPosParams[4].fCurrentLVDTmm;
	m_RPPosParams[ERPN_A7R1L].fCurrentLVDTmm=pConfig->CfgRPosParams[5].fCurrentLVDTmm;
	m_RPPosParams[ERPN_B7R1U].fCurrentLVDTmm=pConfig->CfgRPosParams[6].fCurrentLVDTmm;
	m_RPPosParams[ERPN_B7R1L].fCurrentLVDTmm=pConfig->CfgRPosParams[7].fCurrentLVDTmm;
}

void ALFA_GeometryReader::SetupSWCorrections(const PGEOMETRYCONFIGURATION pConfig)
{
	int i, nSign;
	double fTotYOffset;
	//double arrFWDistance[8]={0.000,	0.020, -0.050, 0.003, -0.054, 0.012, -0.046, 0.045};

	for(i=0;i<RPOTSCNT;i++){
		nSign=(i%2==0)? +1:-1;
		fTotYOffset=pConfig->CfgRPosParams[i].swcorr.fYOffset-nSign*(2.0*CLHEP::mm);//+nSign*arrFWDistance[i];
		//fTotYOffset=pConfig->CfgRPosParams[i].swcorr.fYOffset;

		m_RPPosParams[(eRPotName)(i+1)].OriginOfDetSWTransform=HepGeom::Point3D<double>(-77.5*CLHEP::mm+pConfig->CfgRPosParams[i].swcorr.fXOffset,-170.2*CLHEP::mm,114.0*CLHEP::mm);
		m_RPPosParams[(eRPotName)(i+1)].DetSWTransform=HepGeom::RotateZ3D(nSign*pConfig->CfgRPosParams[i].swcorr.fTheta);
		m_RPPosParams[(eRPotName)(i+1)].RPSWTransformInStation=HepGeom::Translate3D(-pConfig->CfgRPosParams[i].swcorr.fXOffset,fTotYOffset,0.0*CLHEP::mm);
	}
}

void ALFA_GeometryReader::SetupUserCorrections(const PGEOMETRYCONFIGURATION pConfig)
{
	if(m_eMetrologyType==EMT_NOMINAL){
		for(int i=0;i<RPOTSCNT;i++){
			if(pConfig->CfgRPosParams[i].usercorr.bIsEnabledUserTranform){
				m_RPPosParams[(eRPotName)(i+1)].OriginOfDetSWTransform=pConfig->CfgRPosParams[i].usercorr.UserOriginOfDetTransInRPot;
				m_RPPosParams[(eRPotName)(i+1)].DetSWTransform=pConfig->CfgRPosParams[i].usercorr.UserTransformOfDetInRPot;
				m_RPPosParams[(eRPotName)(i+1)].RPSWTransformInStation=pConfig->CfgRPosParams[i].usercorr.UserTransformOfRPInStation;
			}
		}
	}
}
