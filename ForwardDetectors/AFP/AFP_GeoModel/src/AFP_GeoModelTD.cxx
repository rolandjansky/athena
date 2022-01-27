/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_GeoModel/AFP_GeoModelFactory.h"
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
#include "StoreGate/StoreGateSvc.h"
#include "GeoModelUtilities/GeoModelTool.h"

#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelUtilities/GeoOpticalPhysVol.h"
#include "GeoModelUtilities/GeoOpticalSurface.h"
#include "GeoModelUtilities/GeoBorderSurfaceContainer.h"
#include "GeoModelUtilities/GeoMaterialPropertiesTable.h"

#include <iostream>
#include <fstream>
#include <string>

#include <math.h>
#include <algorithm>

#include <list>
#include <map>

#include <stdlib.h>

void AFP_GeoModelFactory::initializeTDParameters()
{
    // Surface definition (preliminary)
    m_pOpticalSurface = new GeoOpticalSurface("TDQuarticOpticalSurface", GeoOpticalSurface::unified, GeoOpticalSurface::polished, GeoOpticalSurface::dielectric_dielectric, GeoOpticalSurface::polished);

    m_pReflectionOptSurface=new GeoOpticalSurface("AirLightGuideSurface", GeoOpticalSurface::unified, GeoOpticalSurface::polished, GeoOpticalSurface::dielectric_metal, GeoOpticalSurface::polished);
    GeoMaterialPropertiesTable* pMPT=new GeoMaterialPropertiesTable();
    double pfEnergy1[2]={ 1.62*CLHEP::eV, 6.20*CLHEP::eV };
    double pfReflectivity1[2]={ 0.9, 0.9};
    pMPT->AddProperty("REFLECTIVITY",pfEnergy1,pfReflectivity1,2);
    m_pReflectionOptSurface->SetMaterialPropertiesTable(pMPT);
}

StatusCode AFP_GeoModelFactory::addTimingDetector(const char* pszStationName, GeoOpticalPhysVol* pPhysMotherVol, HepGeom::Transform3D& TransInMotherVolume, GeoBorderSurfaceContainer* bsContainer)
{
	int i,j,nPixelID;
	double fXShift,fYShift,fZShift;

	eAFPStation eStation=m_pGeometry->parseStationName(pszStationName);
    
	AFP_TDCONFIGURATION TofCfg=m_CfgParams.tdcfg[eStation];
	AFPTOF_LBARDIMENSIONS BarDims11=TofCfg.mapBarDims[11];
	fXShift=-73.5*CLHEP::mm; 
	fYShift=(BarDims11.fRadLength+TofCfg.mapTrainInfo[BarDims11.nTrainID].fPerpShiftInPixel-0.5*(BarDims11.fLGuideWidth-TofCfg.mapTrainInfo[BarDims11.nTrainID].fTaperOffset)-0.5*BarDims11.fLBarThickness/tan(TofCfg.fAlpha))*sin(TofCfg.fAlpha);
	fZShift=std::abs(fYShift)/tan(TofCfg.fAlpha)+0.5*BarDims11.fLBarThickness/sin(TofCfg.fAlpha);
    
	HepGeom::Transform3D TofTransform=TransInMotherVolume*HepGeom::Translate3D(fXShift,fYShift,fZShift)*HepGeom::RotateX3D((90.0*CLHEP::deg-TofCfg.fAlpha))*HepGeom::RotateZ3D(-90.0*CLHEP::deg);

	for(i=0;i<m_CfgParams.tdcfg[eStation].nX1PixCnt;i++)
	{
		for(j=0;j<m_CfgParams.tdcfg[eStation].nX2PixCnt;j++)
		{
			nPixelID=10*(i+1)+(j+1);
			addSepRadLBar(pszStationName,1,nPixelID,pPhysMotherVol,TofTransform,bsContainer);
		}
	}

	if(m_addSeparationWindow)
    {
        addLBarSensorSeparationWindow(pszStationName,1,pPhysMotherVol,TofTransform,bsContainer);
    }
	addSensor(pszStationName,1,pPhysMotherVol,TofTransform,bsContainer);

    return StatusCode::SUCCESS;
}

void AFP_GeoModelFactory::addSepRadLBar(const char* pszStationName, const int nQuarticID, const int nBarID, GeoOpticalPhysVol* pPhysMotherVolume, HepGeom::Transform3D& TransInMotherVolume, GeoBorderSurfaceContainer* bsContainer)
{
	double fX1Pos,fX2Pos,falpha,fd;
	HepGeom::Vector3D<double> vecA1, vecA2, vecX;
	CLHEP::Hep3Vector vecCutShift;
	HepGeom::Transform3D TransCut;
	GeoShapeShift* pMoveCut;
	GeoBox* pSolAux;
	char szlabel[64];

	CLHEP::HepRotation Rot1,Rot2,Rot3;

	eAFPStation eStation=m_pGeometry->parseStationName(pszStationName);
	AFP_TDCONFIGURATION TofCfg=m_CfgParams.tdcfg[eStation];
	AFPTOF_LBARDIMENSIONS BarDims=TofCfg.mapBarDims[nBarID];

	double fTaperOffset=TofCfg.mapTrainInfo[BarDims.nTrainID].fTaperOffset;
	m_pGeometry->getPixelLocalPosition(eStation,nBarID,&fX1Pos,&fX2Pos);

	fX1Pos+=TofCfg.mapTrainInfo[BarDims.nTrainID].fPerpShiftInPixel;

    
	//Light guide
	HepGeom::Transform3D TotTransform=TransInMotherVolume*HepGeom::Translate3D( fX1Pos,
                                                                                0.5*BarDims.fLGuideLength,
                                                                                fX2Pos);
	GeoShape* pSolVolume=new GeoBox(0.5*BarDims.fLGuideWidth,0.5*BarDims.fLGuideLength,0.5*BarDims.fLBarThickness);

	if(TofCfg.mapTrainInfo[BarDims.nTrainID].bUseTaper)
	{
		falpha=TofCfg.mapTrainInfo[BarDims.nTrainID].fTaperAngle;
		fd=fTaperOffset/sin(falpha);
		vecA1=-fd*cos(falpha)*CLHEP::Hep3Vector(0.0,1.0,0.0);
		vecA2=+0.5*fd*sqrt(2.0)*(CLHEP::HepRotationZ(+(45*CLHEP::deg-falpha))*CLHEP::Hep3Vector(0.0,1.0,0.0)).unit();
		vecX=vecA1+vecA2;
		vecCutShift=CLHEP::Hep3Vector(-0.5*BarDims.fLGuideWidth,0.5*BarDims.fLGuideLength,0.0)+CLHEP::Hep3Vector(vecX);
		Rot1.rotateZ(-falpha);

		pSolAux=new GeoBox(0.5*fd,0.5*fd,0.5*BarDims.fLBarThickness+SLIMCUT);
		TransCut=HepGeom::Transform3D(Rot1,vecCutShift);
		pMoveCut=new GeoShapeShift(pSolAux, TransCut);
		pSolVolume=new GeoShapeSubtraction(pSolVolume, pMoveCut);
	}

	sprintf(szlabel,"%s_Q%i_LogLGuide[%i]",pszStationName,nQuarticID,nBarID);
	GeoLogVol* pLogLGuide=new GeoLogVol(szlabel,pSolVolume,m_MapMaterials[std::string("Quartz")]);
	GeoOpticalPhysVol* pPhysLGuide=new GeoOpticalPhysVol(pLogLGuide);
	sprintf(szlabel,"%s_Q%i_LGuide[%i]",pszStationName,nQuarticID,nBarID);
	pPhysMotherVolume->add(new GeoNameTag(szlabel));
	pPhysMotherVolume->add(new GeoTransform(TotTransform));
	pPhysMotherVolume->add(pPhysLGuide);
	sprintf(szlabel,"%s_Q%i_LGuideSurface[%i]",pszStationName,nQuarticID,nBarID);
	bsContainer->push_back(GeoBorderSurface(szlabel, pPhysLGuide, pPhysMotherVolume, m_pOpticalSurface));

	//Radiator elbow
	double fRadYDim=BarDims.fRadYDim;
	double fElbowXDim=BarDims.fRadYDim;
	TotTransform=TransInMotherVolume*HepGeom::Translate3D(  fX1Pos-0.5*BarDims.fLGuideWidth+0.5*fElbowXDim+fTaperOffset,
                                                            BarDims.fLGuideLength+0.5*fRadYDim,
                                                            fX2Pos);
	pSolVolume=new GeoBox(0.5*fElbowXDim,0.5*fRadYDim,0.5*BarDims.fLBarThickness);

	falpha=45.0*CLHEP::deg;
	fd=fRadYDim/sin(falpha);//sqrt(2.0)*fRadYDim;
	vecA1=-fd*cos(falpha)*CLHEP::Hep3Vector(0.0,1.0,0.0);
	vecA2=+0.5*fd*sqrt(2.0)*(CLHEP::HepRotationZ(+(45*CLHEP::deg-falpha))*CLHEP::Hep3Vector(0.0,1.0,0.0)).unit();
	vecX=vecA1+vecA2;
	vecCutShift=CLHEP::Hep3Vector(-0.5*fElbowXDim,0.5*fRadYDim,0.0)+CLHEP::Hep3Vector(vecX);
	Rot2.rotateZ(-falpha);

	pSolAux=new GeoBox(0.5*fd,0.5*fd,0.5*BarDims.fLBarThickness+SLIMCUT);
	TransCut=HepGeom::Transform3D(Rot2,vecCutShift);
	pMoveCut=new GeoShapeShift(pSolAux, TransCut);
	pSolVolume=new GeoShapeSubtraction(pSolVolume, pMoveCut);

	sprintf(szlabel,"%s_Q%i_LogRadiator[%i]",pszStationName,nQuarticID,nBarID);
	GeoLogVol* pLogRadiator=new GeoLogVol(szlabel,pSolVolume,m_MapMaterials[std::string("Quartz")]);
	GeoOpticalPhysVol* pPhysRadiator=new GeoOpticalPhysVol(pLogRadiator);
	sprintf(szlabel,"%s_Q%i_Radiator[%i]",pszStationName,nQuarticID,nBarID);
	pPhysMotherVolume->add(new GeoNameTag(szlabel));
	pPhysMotherVolume->add(new GeoTransform(TotTransform));
	pPhysMotherVolume->add(pPhysRadiator);
	sprintf(szlabel,"%s_Q%i_RadiatorElbowSurface[%i]",pszStationName,nQuarticID,nBarID);
	bsContainer->push_back(GeoBorderSurface(szlabel, pPhysRadiator, pPhysMotherVolume, m_pReflectionOptSurface));

	//Radiator
	double fRadLength=BarDims.fRadLength-(BarDims.fLGuideWidth-fTaperOffset);
	TotTransform=TransInMotherVolume*HepGeom::Translate3D(  fX1Pos-0.5*BarDims.fLGuideWidth+fElbowXDim+fTaperOffset+0.5*fRadLength,
                                                            BarDims.fLGuideLength+0.5*fRadYDim,
                                                            fX2Pos);
	pSolVolume=new GeoBox(0.5*fRadLength,0.5*fRadYDim,0.5*BarDims.fLBarThickness);
    
	if(TofCfg.bApplyBottomCut){
		falpha = TofCfg.fAlpha;
		fd = BarDims.fLBarThickness/sin(falpha);
		vecA1 = -fd*cos(falpha)*CLHEP::Hep3Vector(1.0,0.0,0.0);
		vecA2 = 0.5*fd*sqrt(2.0)*(CLHEP::HepRotationY(-(45*CLHEP::deg-falpha))*CLHEP::Hep3Vector(1.0,0.0,0.0)).unit();
		vecX = vecA1 + vecA2;
		vecCutShift = CLHEP::Hep3Vector(0.5*fRadLength,0.0,0.5*BarDims.fLBarThickness) + CLHEP::Hep3Vector(vecX);
		Rot3.rotateY(falpha);

		pSolAux=new GeoBox(0.5*fd,0.5*fRadYDim+SLIMCUT,0.5*fd);
		TransCut=HepGeom::Transform3D(Rot3,vecCutShift);
		pMoveCut=new GeoShapeShift(pSolAux, TransCut);
		pSolVolume=new GeoShapeSubtraction(pSolVolume, pMoveCut);
	}

	sprintf(szlabel,"%s_Q%i_LogRadiator[%i]",pszStationName,nQuarticID,nBarID);
	pLogRadiator=new GeoLogVol(szlabel,pSolVolume,m_MapMaterials[std::string("Quartz")]);
	pPhysRadiator=new GeoOpticalPhysVol(pLogRadiator);
	sprintf(szlabel,"%s_Q%i_Radiator[%i]",pszStationName,nQuarticID,nBarID);
	pPhysMotherVolume->add(new GeoNameTag(szlabel));
	pPhysMotherVolume->add(new GeoTransform(TotTransform));
	pPhysMotherVolume->add(pPhysRadiator);
	sprintf(szlabel,"%s_Q%i_RadiatorSurface[%i]",pszStationName,nQuarticID,nBarID);
	bsContainer->push_back(GeoBorderSurface(szlabel, pPhysRadiator, pPhysMotherVolume, m_pOpticalSurface));
}


void AFP_GeoModelFactory::addLBarSensorSeparationWindow(const char* pszStationName, const int nQuarticID, GeoOpticalPhysVol* pPhysMotherVolume,
									HepGeom::Transform3D &TransInMotherVolume, GeoBorderSurfaceContainer* bsContainer)
{
	int nPixelID;
	char szlabel[64];
	double fX1Pos_1, fX2Pos_1, fX1Pos_2, fX2Pos_2;
	HepGeom::Transform3D TotTransform;

	eAFPStation eStation=m_pGeometry->parseStationName(pszStationName);
	AFP_TDCONFIGURATION TofCfg=m_CfgParams.tdcfg[eStation];
	double fSensor2BarDistance=(TofCfg.bEmulateImmersion)? 0.0:m_AfpConstants.ToF_Sensor2BarDist;

	GeoTube* pSolWindow=new GeoTube(0,0.5*m_AfpConstants.ToF_SeparationWindowDiameter,0.5*m_AfpConstants.ToF_SeparationWindowThickness);
    
    nPixelID=10*( TofCfg.nX1PixCnt/2-1 +1)+( TofCfg.nX1PixCnt/2-1 +1);
    m_pGeometry->getPixelLocalPosition(eStation,nPixelID,&fX1Pos_1,&fX2Pos_1);
    nPixelID=10*( TofCfg.nX1PixCnt/2 +1)+( TofCfg.nX1PixCnt/2 +1);
    m_pGeometry->getPixelLocalPosition(eStation,nPixelID,&fX1Pos_2,&fX2Pos_2);
    
    sprintf(szlabel,"%s_Q%i_LogLBarSensorSeparationWindow",pszStationName,nQuarticID);
    TotTransform=TransInMotherVolume*HepGeom::Translate3D(0.5*(fX1Pos_1+fX1Pos_2),-0.5*m_AfpConstants.ToF_SeparationWindowThickness-fSensor2BarDistance,0.5*(fX2Pos_1+fX2Pos_2))*HepGeom::RotateX3D(90.0*CLHEP::deg);
    GeoLogVol* pLogWindow=new GeoLogVol(szlabel,pSolWindow,m_MapMaterials["Quartz"]);
    GeoOpticalPhysVol* pPhysWindow=new GeoOpticalPhysVol(pLogWindow);
    sprintf(szlabel,"%s_Q%i_LBarSensorSeparationWindow",pszStationName,nQuarticID);
    pPhysMotherVolume->add(new GeoNameTag(szlabel));
    pPhysMotherVolume->add(new GeoTransform(TotTransform));
    pPhysMotherVolume->add(pPhysWindow);
    sprintf(szlabel,"%s_Q%i_WindowSurface",pszStationName,nQuarticID);
    bsContainer->push_back(GeoBorderSurface(szlabel, pPhysWindow, pPhysMotherVolume, m_pOpticalSurface));
    
    pPhysWindow=nullptr;

}



void AFP_GeoModelFactory::addSensor(const char* pszStationName, const int nQuarticID, GeoOpticalPhysVol* pPhysMotherVolume,
									HepGeom::Transform3D &TransInMotherVolume, GeoBorderSurfaceContainer* bsContainer)
{
	int i,j,nPixelID;
	char szlabel[64];
	double fX1Pos, fX2Pos;
	HepGeom::Transform3D TotTransform;

	eAFPStation eStation=m_pGeometry->parseStationName(pszStationName);
	AFP_TDCONFIGURATION TofCfg=m_CfgParams.tdcfg[eStation];
	double fSensor2BarDistance=(TofCfg.bEmulateImmersion)? 0.0:m_AfpConstants.ToF_Sensor2BarDist;
    if(m_addSeparationWindow)
    {
       fSensor2BarDistance += m_AfpConstants.ToF_SeparationWindowThickness;
    }

	GeoBox* pSolSensor=new GeoBox(0.5*TofCfg.fPixelX1Dim,0.5*m_AfpConstants.ToF_SensorThickness,0.5*TofCfg.fPixelX2Dim);

	for(i=0;i<TofCfg.nX1PixCnt;i++)
	{
		for(j=0;j<TofCfg.nX2PixCnt;j++)
		{
			nPixelID=10*(i+1)+(j+1);
            m_pGeometry->getPixelLocalPosition(eStation,nPixelID,&fX1Pos,&fX2Pos);

                        sprintf(szlabel,"%s_Q%i_LogTDSensor[%i]",pszStationName,nQuarticID,nPixelID);
			TotTransform=TransInMotherVolume*HepGeom::Translate3D(fX1Pos,-0.5*m_AfpConstants.ToF_SensorThickness-fSensor2BarDistance,fX2Pos);
			GeoLogVol* pLogSensor=new GeoLogVol(szlabel,pSolSensor,m_MapMaterials["SiliconPMT"]);
			GeoOpticalPhysVol* pPhysSensor=new GeoOpticalPhysVol(pLogSensor);
                        sprintf(szlabel,"%s_Q%i_TDSensor[%i]",pszStationName,nQuarticID,nPixelID);
			pPhysMotherVolume->add(new GeoNameTag(szlabel));
			pPhysMotherVolume->add(new GeoTransform(TotTransform));
			pPhysMotherVolume->add(pPhysSensor);
			sprintf(szlabel,"%s_Q%i_SensorSurface[%i]",pszStationName,nQuarticID,nPixelID);
			bsContainer->push_back(GeoBorderSurface(szlabel, pPhysSensor, pPhysMotherVolume, m_pOpticalSurface));

			pPhysSensor=nullptr;
		}
	}
}
