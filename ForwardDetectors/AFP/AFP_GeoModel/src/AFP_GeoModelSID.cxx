/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_GeoModelFactory.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "CLHEP/GenericFunctions/AbsFunction.hh"
#include "CLHEP/GenericFunctions/Variable.hh"
#include "CLHEP/GenericFunctions/Sin.hh"
#include "CLHEP/GenericFunctions/Cos.hh"

#include "GeoModelInterfaces/StoredMaterialManager.h"

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <algorithm>
#include <list>
#include <map>
#include <stdlib.h>

void AFP_GeoModelFactory::AddSiDetector(GeoPhysVol* pPhysMotherVol, const char* pszStationName)
{
	char szLabel[32];

	//add both supports
//	sprintf(szLabel,"%s_LogSIDSupport",pszStationName);
//	GeoShape* pSolidSidSupport=CreateSolidSIDSupport();
//	GeoLogVol* pLogSIDSupport=new GeoLogVol(szLabel,pSolidSidSupport,m_MapMaterials[std::string("Steel")]);
//	GeoFullPhysVol* pPhysSIDSupport1=new GeoFullPhysVol(pLogSIDSupport);
//	GeoFullPhysVol* pPhysSIDSupport2=new GeoFullPhysVol(pLogSIDSupport);

//	sprintf(szLabel,"%s_SIDSupport[01]",pszStationName);
//	pPhysMotherVol->add(new GeoNameTag(szLabel));
//	pPhysMotherVol->add(new GeoTransform(m_pGeometry->GetSIDTransform(ESTT_SUPPORT,pszStationName,1)));
//	pPhysMotherVol->add(pPhysSIDSupport1);

//	sprintf(szLabel,"%s_SIDSupport[02]",pszStationName);
//	pPhysMotherVol->add(new GeoNameTag(szLabel));
//	pPhysMotherVol->add(new GeoTransform(m_pGeometry->GetSIDTransform(ESTT_SUPPORT,pszStationName,2)));
//	pPhysMotherVol->add(pPhysSIDSupport2);
	
	//create cooling channel
	//GeoShape* pSolidSIDCooling=CreateSolidSIDCooling();

	//create thin vacuum layer close to the SID + global
	GeoShape* pSolidSIDPlate=CreateSolidSIDPlate();
	GeoShape* pSolidSIDSensor=new GeoBox(0.5*SID_SENSORXDIM,0.5*SID_SENSORYDIM,0.5*SID_SENSORTHICKNESS);
	GeoShape* pSolidSIDVacuumSensor=CreateSolidSIDVacuumLayer();

	//add global vacuum sensitive volume (ID=11)
	int nSpecVacSensorID=SID_GLOBALVACUUMSENSORID;
	sprintf(szLabel,"%s_LogSIDVacuumSensor[%i]",pszStationName,nSpecVacSensorID);
	GeoLogVol* pLogSIDVacuumSensor=new GeoLogVol(szLabel,pSolidSIDVacuumSensor,m_MapMaterials[std::string("std::Vacuum")]);
	GeoFullPhysVol* pPhysSIDVacuumSensor=new GeoFullPhysVol(pLogSIDVacuumSensor);
	sprintf(szLabel,"%s_SIDVacuumSensor[%i]",pszStationName,nSpecVacSensorID);
	pPhysMotherVol->add(new GeoNameTag(szLabel));
	pPhysMotherVol->add(new GeoTransform(m_pGeometry->GetSIDTransform(ESTT_VACUUMSENSOR,pszStationName,nSpecVacSensorID)));
	pPhysMotherVol->add(pPhysSIDVacuumSensor);

	//add global vacuum sensitive volume (ID=11)
//	nSpecVacSensorID=SID_GLOBALVACUUMSENSORID;
//	sprintf(szLabel,"%s_LogSIDVacuumSensor[%i]",pszStationName,nSpecVacSensorID);
//	pLogSIDVacuumSensor=new GeoLogVol(szLabel,pSolidSIDVacuumSensor,m_MapMaterials[std::string("std::Vacuum")]);
//	pPhysSIDVacuumSensor=new GeoFullPhysVol(pLogSIDVacuumSensor);
//	sprintf(szLabel,"%s_SIDVacuumSensor[%i]",pszStationName,nSpecVacSensorID);
//	pPhysMotherVol->add(new GeoNameTag(szLabel));
//	pPhysMotherVol->add(new GeoTransform(m_pGeometry->GetSIDTransform(ESTT_VACUUMSENSOR,pszStationName,nSpecVacSensorID)));
//	pPhysMotherVol->add(pPhysSIDVacuumSensor);

	for(int i=0;i<m_pGeometry->GetSIDPlatesCnt();i++)
	{
		// create logic SID plate
		sprintf(szLabel,"%s_LogSIDPlate[%i]",pszStationName,i);
		GeoLogVol* pLogSIDPlate=new GeoLogVol(szLabel,pSolidSIDPlate,m_MapMaterials[std::string("CE7")]);
		GeoFullPhysVol* pPhysSIDPlate=new GeoFullPhysVol(pLogSIDPlate);

		//HepGeom::Transform3D PlateTotTrans=TotTransform*HepGeom::Translate3D(fxm,0.0*CLHEP::mm,i*fSpacing/cos(falpha))*HepGeom::RotateY3D(falpha);
		sprintf(szLabel,"%s_SIDPlate[%i]",pszStationName,i);
		pPhysMotherVol->add(new GeoNameTag(szLabel));
		//pPhysMotherVol->add(new GeoTransform(PlateTotTrans));
		pPhysMotherVol->add(new GeoTransform(m_pGeometry->GetSIDTransform(ESTT_PLATE,pszStationName,i)));
		pPhysMotherVol->add(pPhysSIDPlate);
		//m_pDetectorManager->addTreeTop(pPhysSIDPlate);
			
		// create cooling channels
		/*
		sprintf(szLabel,"%s_LogSIDCooling[%i]",pszStationName,i);
		GeoLogVol* pLogSIDCooling=new GeoLogVol(szLabel,pSolidSIDCooling,m_MapMaterials[std::string("Water")]);
		GeoFullPhysVol* pPhysSIDCooling=new GeoFullPhysVol(pLogSIDCooling);
			
		sprintf(szLabel,"%s_SIDCooling[%i]",pszStationName,i);
		pPhysSIDPlate->add(new GeoNameTag(szLabel));
		pPhysSIDPlate->add(pPhysSIDCooling);
		*/
		
		// create logic SID
		sprintf(szLabel,"%s_LogSIDSensor[%i]",pszStationName,i);
		GeoLogVol* pLogSID=new GeoLogVol(szLabel,pSolidSIDSensor,m_MapMaterials[std::string("Silicon")]);
		GeoFullPhysVol* pPhysSID=new GeoFullPhysVol(pLogSID);

		//HepGeom::Transform3D TransSID=PlateTotTrans*HepGeom::Translate3D(DETXSIDE*(-pMainPlate->getXHalfLength()+((GeoBox*)m_MapShapes["Sensor"])->getXHalfLength()), 0.0*CLHEP::mm, +(pMainPlate->getZHalfLength()+((GeoBox*)m_MapShapes["Sensor"])->getZHalfLength()+0.0*CLHEP::mm));
		//HepGeom::Transform3D TotTransSIDInWorld=TransMotherInWorld*TransSID;
		sprintf(szLabel,"%s_SIDSensor[%i]",pszStationName,i);
		pPhysMotherVol->add(new GeoNameTag(szLabel));
		//pPhysMotherVol->add(new GeoTransform(TransSID));
		pPhysMotherVol->add(new GeoTransform(m_pGeometry->GetSIDTransform(ESTT_SENSOR,pszStationName,i)));
		pPhysMotherVol->add(pPhysSID);
		//m_MapSIDTransToLocal[std::string(pszStationName)][i]=m_pGeometry->GetSIDTransform(ESTT_SENSORLOCAL,pszStationName,i);

		if(m_CfgParams.sidcfg.bAddVacuumSensors){
			// create logic SID Vacuum Layer
			sprintf(szLabel,"%s_LogSIDVacuumSensor[%i]",pszStationName,i);
			GeoLogVol* pLogSIDVacuumSensor=new GeoLogVol(szLabel,pSolidSIDVacuumSensor,m_MapMaterials[std::string("std::Vacuum")]);
			GeoFullPhysVol* pPhysSIDVacuumSensor=new GeoFullPhysVol(pLogSIDVacuumSensor);

			sprintf(szLabel,"%s_SIDVacuumSensor[%i]",pszStationName,i);
			pPhysMotherVol->add(new GeoNameTag(szLabel));
			//pPhysMotherVol->add(new GeoTransform(TotTransform*HepGeom::Translate3D(fxm,0.0*CLHEP::mm,i*NOMINALSIDSPACING/cos(falpha))*HepGeom::RotateY3D(falpha)));
			//pPhysMotherVol->add(new GeoTransform(HepGeom::Translate3D(DETXSIDE*(-pMainPlate->getXHalfLength()+((GeoBox*)m_MapShapes["Sensor"])->getXHalfLength()), 0.0*CLHEP::mm, -pMainPlate->getZHalfLength()-0.5*SID_VACUUMSENSORTHICKNESS)));
			pPhysMotherVol->add(new GeoTransform(m_pGeometry->GetSIDTransform(ESTT_VACUUMSENSOR,pszStationName,i)));
			pPhysMotherVol->add(pPhysSIDVacuumSensor);
		}
	}
}

GeoShape* AFP_GeoModelFactory::CreateSolidSIDSupport()
{
	double fdelta=0.01*CLHEP::mm;

	// main plate + edge
	GeoBox* pMainBlock=new GeoBox(0.5*75.6*CLHEP::mm,0.5*(80-2*3.25)*CLHEP::mm,0.5*SID_PLATETHICKNESS);
	GeoBox* pEdge=new GeoBox(0.5*(94-75.6+10)*CLHEP::mm,0.5*(108-14)*CLHEP::mm, 0.5*SID_PLATETHICKNESS);
    HepGeom::Transform3D TransEdge=HepGeom::Translate3D(DETXSIDE*(pMainBlock->getXHalfLength()+pEdge->getXHalfLength()),pEdge->getYHalfLength()-pMainBlock->getYHalfLength(), 0.0*CLHEP::mm);
	GeoShapeShift* pMoveEdge=new GeoShapeShift(pEdge, TransEdge);
	GeoShapeUnion* pSupport1=new GeoShapeUnion(pMainBlock,pMoveEdge);

	// cut sensitive area
	GeoBox* pSensorCut=new GeoBox(0.5*(21.0+10)*CLHEP::mm, 0.5*(22+10.0)*CLHEP::mm, 0.5*SID_PLATETHICKNESS+fdelta);
    HepGeom::Transform3D TransSensorCut=HepGeom::Translate3D(DETXSIDE*(-pMainBlock->getXHalfLength()+pSensorCut->getXHalfLength()-fdelta), 0.0*CLHEP::mm, 0.0*CLHEP::mm);
	GeoShapeShift* pMoveSensorCut=new GeoShapeShift(pSensorCut, TransSensorCut);
	GeoShapeSubtraction* pSupport2=new GeoShapeSubtraction(pSupport1, pMoveSensorCut);

	return pSupport2;
}

GeoShape* AFP_GeoModelFactory::CreateSolidSIDPlate()
{
    double fdelta=0.01*CLHEP::mm;//0.01*CLHEP::mm;

    double fSensorHolderWidth=0.3*CLHEP::mm;
    double fSensorHolderPlateWidth=0.0*CLHEP::mm;//0.3*CLHEP::mm; //thin plate under sensor
	double fPlateThickness=m_CfgParams.sidcfg.fSupportThickness;

	//main plate including cover + edge
	GeoBox* pMainPlate=new GeoBox(0.5*SID_MAINPLATEXDIM, 0.5*SID_MAINPLATEYDIM, 0.5*fPlateThickness);
	GeoBox* pEdge=new GeoBox(0.5*(94*CLHEP::mm-SID_MAINPLATEXDIM),0.5*108*CLHEP::mm, 0.5*fPlateThickness);
    HepGeom::Transform3D TransEdge=HepGeom::Translate3D(DETXSIDE*(pMainPlate->getXHalfLength()+pEdge->getXHalfLength()),0.0*CLHEP::mm, 0.0*CLHEP::mm);
	GeoShapeShift* pMoveEdge=new GeoShapeShift(pEdge, TransEdge);
	GeoShapeUnion* pDetPlate1=new GeoShapeUnion(pMainPlate,pMoveEdge);

	//cut sensitive area
	GeoBox* pSensorCut=new GeoBox(0.5*21.0*CLHEP::mm, 0.5*22.0*CLHEP::mm,pMainPlate->getZHalfLength()-0.5*fSensorHolderWidth);
	HepGeom::Transform3D TransSensorCut=HepGeom::Translate3D(DETXSIDE*(-pMainPlate->getXHalfLength()+pSensorCut->getXHalfLength()-fdelta),0.0*CLHEP::mm, -pMainPlate->getZHalfLength()+pSensorCut->getZHalfLength()-fdelta);
	GeoShapeShift* pMoveSensorCut=new GeoShapeShift(pSensorCut, TransSensorCut);
	GeoShapeSubtraction* pDetPlate2=new GeoShapeSubtraction(pDetPlate1, pMoveSensorCut);

    GeoBox* pSensorCut2=new GeoBox(0.5*18*CLHEP::mm+fdelta,0.5*16*CLHEP::mm,0.5*(fSensorHolderWidth-fSensorHolderPlateWidth)+fdelta);
    HepGeom::Transform3D TransSensorCut2=HepGeom::Translate3D(DETXSIDE*(-pMainPlate->getXHalfLength()+pSensorCut2->getXHalfLength()-fdelta),0.0*CLHEP::mm, pMainPlate->getZHalfLength()-pSensorCut2->getZHalfLength()-fSensorHolderPlateWidth+fdelta);
	GeoShapeShift* pMoveSensorCut2=new GeoShapeShift(pSensorCut2, TransSensorCut2);
	GeoShapeSubtraction* pDetPlate3=new GeoShapeSubtraction(pDetPlate2, pMoveSensorCut2);

	//cut side edges
	GeoBox* pSideEdge=new GeoBox(0.5*27.5*CLHEP::mm, 0.5*12.5*CLHEP::mm, 0.5*2*CLHEP::mm);
	HepGeom::Transform3D TransSideEdgeUp=HepGeom::Translate3D(DETXSIDE*(-pMainPlate->getXHalfLength()+pSideEdge->getXHalfLength()-fdelta),pMainPlate->getYHalfLength()-pSideEdge->getYHalfLength()+fdelta,-pMainPlate->getZHalfLength()+pSideEdge->getZHalfLength()-fdelta);
	GeoShapeShift* pMoveSideEdgeUp=new GeoShapeShift(pSideEdge,TransSideEdgeUp);
	GeoShapeSubtraction* pDetPlate4=new GeoShapeSubtraction(pDetPlate3, pMoveSideEdgeUp);

	HepGeom::Transform3D TransSideEdgeDown=HepGeom::Translate3D(DETXSIDE*(-pMainPlate->getXHalfLength()+pSideEdge->getXHalfLength()-fdelta),-pMainPlate->getYHalfLength()+pSideEdge->getYHalfLength()-fdelta,-pMainPlate->getZHalfLength()+pSideEdge->getZHalfLength()-fdelta);
	GeoShapeShift* pMoveSideEdgeDown=new GeoShapeShift(pSideEdge,TransSideEdgeDown);
	GeoShapeSubtraction* pDetPlate5=new GeoShapeSubtraction(pDetPlate4, pMoveSideEdgeDown);

	//m_MapShapes["MainPlate"]=pMainPlate;
	//m_MapShapes["EdgePlate"]=pEdge;

	return pDetPlate5;
}


GeoShape* AFP_GeoModelFactory::CreateSolidSIDCooling()
{
	// Symmetric part
	GeoBox* pCoolingCh1=new GeoBox(0.5*22*CLHEP::mm, 0.5*4*CLHEP::mm, 0.5*4*CLHEP::mm);
	HepGeom::Transform3D DownLeft1=HepGeom::Translate3D(DETXSIDE*(-75.6/2.+pCoolingCh1->getXHalfLength()+2.)*CLHEP::mm, (13.+4.+2.+2.)*CLHEP::mm, 0.0*CLHEP::mm);
	GeoShapeShift* pMoveDownLeft1=new GeoShapeShift(pCoolingCh1, DownLeft1);
	
	GeoBox* pCoolingCh11=new GeoBox(0.5*25.*CLHEP::mm, 0.5*4*CLHEP::mm, 0.5*4*CLHEP::mm);
	HepGeom::Transform3D DownLeft2=HepGeom::Translate3D(DETXSIDE*(-75.6/2.+pCoolingCh11->getXHalfLength()+2.)*CLHEP::mm, (13.+2.)*CLHEP::mm, 0.0*CLHEP::mm);
	GeoShapeShift* pMoveDownLeft2=new GeoShapeShift(pCoolingCh11, DownLeft2);	
	GeoShapeUnion* pCoolingChU1=new GeoShapeUnion(pMoveDownLeft1,pMoveDownLeft2);	
	
	GeoBox* pCoolingCh2=new GeoBox(0.5*4*CLHEP::mm, 0.5*(4+2+4)*CLHEP::mm, 0.5*4*CLHEP::mm);
	HepGeom::Transform3D DownLeft3=HepGeom::Translate3D(DETXSIDE*(-75.6/2.+pCoolingCh2->getXHalfLength()+2.)*CLHEP::mm, (13.+pCoolingCh2->getYHalfLength())*CLHEP::mm, 0.0*CLHEP::mm);
	GeoShapeShift* pMoveDownLeft3=new GeoShapeShift(pCoolingCh2, DownLeft3);	
	GeoShapeUnion* pCoolingChU2=new GeoShapeUnion(pCoolingChU1,pMoveDownLeft3);
	
	double alpha1=-CLHEP::pi*15.5/180.;
	GeoBox* pCoolingCh3=new GeoBox(0.5*39./std::cos(alpha1)*CLHEP::mm, 0.5*4*CLHEP::mm, 0.5*4*CLHEP::mm);	
	HepGeom::Transform3D MidLeft=HepGeom::Translate3D(DETXSIDE*(-75.6/2.+pCoolingCh3->getXHalfLength()+23.)*CLHEP::mm, 26.25*CLHEP::mm, 0.0*CLHEP::mm)*HepGeom::RotateZ3D(alpha1);
	GeoShapeShift* pMoveMidLeft=new GeoShapeShift(pCoolingCh3, MidLeft);	
	GeoShapeUnion* pCoolingChU3=new GeoShapeUnion(pCoolingChU2,pMoveMidLeft);
	
	GeoBox* pCoolingCh4=new GeoBox(0.5*(18+14)*CLHEP::mm, 0.5*4*CLHEP::mm, 0.5*4*CLHEP::mm);
	HepGeom::Transform3D UpLeft=HepGeom::Translate3D(DETXSIDE*(-75.6/2.+pCoolingCh4->getXHalfLength()+62.)*CLHEP::mm, 31.5*CLHEP::mm, 0.0*CLHEP::mm);
	GeoShapeShift* pMoveUpLeft=new GeoShapeShift(pCoolingCh4, UpLeft);	
	GeoShapeUnion* pCoolingChU4=new GeoShapeUnion(pCoolingChU3,pMoveUpLeft);
	
	HepGeom::RotateX3D Rot=HepGeom::RotateX3D(CLHEP::pi);
	GeoShapeShift* pRot=new GeoShapeShift(pCoolingChU4, Rot);		
	GeoShapeUnion* pCoolingChU5=new GeoShapeUnion(pCoolingChU4,pRot);
	
    GeoBox* pCoolingCh5=new GeoBox(0.5*(25.99)*CLHEP::mm, 0.5*4*CLHEP::mm, 0.5*4*CLHEP::mm);
    HepGeom::Transform3D Center1=HepGeom::Translate3D(DETXSIDE*(-37.8+25.)*CLHEP::mm, 0.0*CLHEP::mm, 0.0*CLHEP::mm)*HepGeom::RotateZ3D(-CLHEP::pi/2.);
	GeoShapeShift* pMoveCenter1=new GeoShapeShift(pCoolingCh5, Center1);	
	GeoShapeUnion* pCoolingChU6=new GeoShapeUnion(pCoolingChU5,pMoveCenter1);
	
	// Asymmetric (central) part
	GeoBox* pCoolingCh6=new GeoBox(0.5*(48.2)*CLHEP::mm, 0.5*4*CLHEP::mm, 0.5*4*CLHEP::mm);
	HepGeom::Transform3D Center2=HepGeom::Translate3D(DETXSIDE*(-75.6/2.+pCoolingCh6->getXHalfLength()+45.8)*CLHEP::mm, -12.*CLHEP::mm, 0.0*CLHEP::mm);
	GeoShapeShift* pMoveCenter2=new GeoShapeShift(pCoolingCh6, Center2);	
	GeoShapeUnion* pCoolingChU7=new GeoShapeUnion(pCoolingChU6,pMoveCenter2);

    GeoBox* pCoolingCh7=new GeoBox(0.5*(18.6)*CLHEP::mm, 0.5*4*CLHEP::mm, 0.5*4*CLHEP::mm);
    HepGeom::Transform3D Center3=HepGeom::Translate3D(DETXSIDE*(-75.6/2.+pCoolingCh7->getXHalfLength()+31.4)*CLHEP::mm, -6.*CLHEP::mm, 0.0*CLHEP::mm)*HepGeom::RotateZ3D(+CLHEP::pi/4.01);
	GeoShapeShift* pMoveCenter3=new GeoShapeShift(pCoolingCh7, Center3);	
	GeoShapeUnion* pCoolingChU8=new GeoShapeUnion(pCoolingChU7,pMoveCenter3);

	GeoBox* pCoolingCh8=new GeoBox(0.5*(8.6)*CLHEP::mm, 0.5*4*CLHEP::mm, 0.5*4*CLHEP::mm);
	HepGeom::Transform3D Center4=HepGeom::Translate3D(DETXSIDE*(-75.6/2.+pCoolingCh8->getXHalfLength()+27.)*CLHEP::mm, 0.0*CLHEP::mm, 0.0*CLHEP::mm);
	GeoShapeShift* pMoveCenter4=new GeoShapeShift(pCoolingCh8, Center4);	
	GeoShapeUnion* pCoolingChU9=new GeoShapeUnion(pCoolingChU8,pMoveCenter4);
	
	return  pCoolingChU9;
}


GeoShape* AFP_GeoModelFactory::CreateSolidSIDVacuumLayer()
{
	// sensitive layer of vacuum (very thin)
	GeoBox* pVacuumSensor=new GeoBox(0.5*180.0*CLHEP::mm,0.5*160.0*CLHEP::mm,0.5*SID_VACUUMSENSORTHICKNESS);

	return pVacuumSensor;
}

