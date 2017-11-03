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

void AFP_GeoModelFactory::InitializeTDParameters()
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

StatusCode AFP_GeoModelFactory::AddTimingDetector(const char* pszStationName, GeoOpticalPhysVol* pPhysMotherVol, HepGeom::Transform3D& TransInMotherVolume, GeoBorderSurfaceContainer* bsContainer)
{
    int i,j,k;
    double fXShift,fZShift;
    AFP_LQBARDIMENSIONS LQBarDims, MainLQBarDims;
    HepGeom::Transform3D TotTransform;

    MainLQBarDims=m_CfgParams.tdcfg.MainLQBarDims;
    int nTrainsCnt=m_CfgParams.tdcfg.nRowsCnt;
    int nColsCnt=m_CfgParams.tdcfg.nColsCnt;
	int nLQBarID=-1;

    for(i=0;i<nTrainsCnt;i++)
    {
        for(j=0;j<nColsCnt;j++)
        {
			nLQBarID=nColsCnt*i+j;
            fXShift=0.0*CLHEP::mm;
            GetLQBarDimensions(i,j,&LQBarDims);

            for(k=0;k<i;k++) fXShift+=(m_CfgParams.tdcfg.vecBarXDim[k]+m_CfgParams.tdcfg.fVertXGap);
            fZShift=(MainLQBarDims.fAlpha>0.0)? j*(MainLQBarDims.fLBarZDim/sin(MainLQBarDims.fAlpha)+m_CfgParams.tdcfg.fVertZGap):0.0*CLHEP::mm;

            TotTransform=TransInMotherVolume*HepGeom::Translate3D(-0.5*LQBarDims.fVertBarXDim-fXShift,LQBarDims.fOffsetFromBeam,fZShift);
			AddLQBarSegment(pszStationName,0,nLQBarID,LQBarDims,pPhysMotherVol,TotTransform,bsContainer);

            TotTransform=TotTransform*HepGeom::Reflect3D(0.0,1.0,0.0,0.0);
			AddLQBarSegment(pszStationName,1,nLQBarID,LQBarDims,pPhysMotherVol,TotTransform,bsContainer);
        }
    }


    return StatusCode::SUCCESS;
}

void AFP_GeoModelFactory::AddLQBarSegment(const char* pszStationName, const int nQuarticID, const int nLQBarID, AFP_LQBARDIMENSIONS& LQBarDims, GeoOpticalPhysVol *pPhysMotherVolume, HepGeom::Transform3D& TransInMotherVolume, GeoBorderSurfaceContainer* bsContainer)
{
    switch(LQBarDims.eType)
    {
    case ELBT_REGULAR:
    case ELBT_HYBRID:
    case ELBT_MIKELBAR:
        //AddLBar(nLBarID,LQBarDims,pPhysMotherVolume,TransInMotherVolume);
        break;
    case ELBT_METALELBOW:
    case ELBT_AIRGUIDE:
    case ELBT_HYBRIDMETALELBOW:
        AddSepRadLBar(pszStationName, nQuarticID, nLQBarID,LQBarDims,pPhysMotherVolume,TransInMotherVolume,bsContainer);
        break;
    case ELBT_ONEARM:
        //AddOneArmLBar(nLBarID,LQBarDims,pPhysMotherVolume,TransInMotherVolume);
        break;
    case ELBT_ONEARMAIRGUIDE:
        //AddSepRadOneArmLBar(nLBarID,LQBarDims,pPhysMotherVolume,TransInMotherVolume);
        break;
    default:
        break;
    }
}

//row=train, column=position along z-axis
void AFP_GeoModelFactory::GetLQBarDimensions(const int nRowID, const int nColID, PAFP_LQBARDIMENSIONS pLQBarDims)
{
    int i;
    double fAux=0.0;
    AFP_LQBARDIMENSIONS MainLQBarDims=m_CfgParams.tdcfg.MainLQBarDims;
    memcpy(pLQBarDims,&m_CfgParams.tdcfg.MainLQBarDims,sizeof(AFP_LQBARDIMENSIONS));

    double falpha=MainLQBarDims.fAlpha;
    pLQBarDims->fVertBarXDim=m_CfgParams.tdcfg.vecBarXDim[nRowID];

    pLQBarDims->fVertBarYDim=MainLQBarDims.fVertBarYDim-(nColID*(MainLQBarDims.fLBarZDim+m_CfgParams.tdcfg.fVertZGap*sin(falpha))/tan(falpha));
    pLQBarDims->fVertBarYDim-=nRowID*(MainLQBarDims.fHorzBarYDim+m_CfgParams.tdcfg.fHorzYGap);
    for(i=0;i<nRowID;i++)fAux+=(m_CfgParams.tdcfg.vecBarXDim[i]+m_CfgParams.tdcfg.fVertXGap);
    pLQBarDims->fHorzBarXDim=MainLQBarDims.fHorzBarXDim+MainLQBarDims.fVertBarXDim-fAux-pLQBarDims->fVertBarXDim;

    if(MainLQBarDims.eType==ELBT_METALELBOW || MainLQBarDims.eType==ELBT_HYBRIDMETALELBOW)
    {
        pLQBarDims->fRadiatorLength=pLQBarDims->fVertBarYDim-pLQBarDims->fVertBarXDim;
    }

}

HepGeom::Vector3D<double> AFP_GeoModelFactory::GetBarShift(AFP_LQBARDIMENSIONS& LQBarDims, eLQBarType eSpecType)
{
    //calculate auxilliary parameters for position
    double fz1=0.0,fz2=0.0,fy1=0.0,fy2=0.0,fyd,fzd,falpha;
    double fXShift,fYShift,fZShift;
    falpha=LQBarDims.fAlpha;

    fyd=LQBarDims.fVertBarYDim;
    fzd=LQBarDims.fLBarZDim;

    eLQBarType eLBType=(eSpecType==ELBT_UNDEFINED)? LQBarDims.eType:eSpecType;

    switch(eLBType){
    case ELBT_REGULAR:
    case ELBT_HYBRID:
    case ELBT_ONEARM:
        fy1=0.5*fzd/tan(falpha);
        fy2=0.5*fyd-fy1;
        fz1=0.5*fzd/sin(falpha);
        fz2=fy2*cos(falpha);
        break;
    case ELBT_MIKELBAR:
        fy1=0.0*CLHEP::mm;
        fy2=0.5*fyd;
        fz1=0.0*CLHEP::mm;
        fz2=fy2*cos(falpha);
        break;
    case ELBT_METALELBOW:
    case ELBT_HYBRIDMETALELBOW:
        fy1=0.5*fzd/tan(falpha);
        fy2=0.5*fyd+0.5*LQBarDims.fRadiatorLength-fy1;
        fz1=0.5*fzd/sin(falpha);
        fz2=fy2*cos(falpha);
        break;
    case ELBT_ONEARMAIRGUIDE:
    case ELBT_AIRGUIDE:
        fy1=0.5*fzd/tan(falpha);
        fy2=0.5*fyd+0.5*LQBarDims.fRadiatorLength-fy1;
        fz1=0.5*fzd/sin(falpha);
        fz2=fy2*cos(falpha);
        break;
    default:
        fy2=0.0*CLHEP::mm;
    }

    fXShift=0.0*CLHEP::mm;//0.5*LQBarDims.fVertBarXDim;
    fYShift=fy2*sin(falpha)+((LQBarDims.eType==ELBT_MIKELBAR)? 0.5*LQBarDims.fLBarZDim:0.0*CLHEP::mm);
    fZShift=fz1+fz2;

    return HepGeom::Vector3D<double>(fXShift,fYShift,fZShift);
}

void AFP_GeoModelFactory::AddSepRadLBar(const char* pszStationName, const int nQuarticID, const int nLQBarID,AFP_LQBARDIMENSIONS& LQBarDims, GeoOpticalPhysVol* pPhysMotherVolume, HepGeom::Transform3D& TransInMotherVolume, GeoBorderSurfaceContainer* bsContainer)
{
    //G4LogicalVolume* pLogMotherVolume=pPhysMotherVolume->GetLogicalVolume();

    double fd,falpha;
    const double fslimcut=TD_SLIMCUT;
    CLHEP::Hep3Vector vecCutShift;
    CLHEP::HepRotation Rot;
    GeoBox* pSolAux;
    char szLabel[64];
    //G4LogicalBorderSurface* pSurface; --TODO
    std::string VertBarMaterial;

    if(LQBarDims.eType==ELBT_AIRGUIDE){
        VertBarMaterial="OpticalVacuum";
    }
    else if(LQBarDims.eType==ELBT_METALELBOW){
        VertBarMaterial="Quartz";
    }
    else if(LQBarDims.eType==ELBT_HYBRIDMETALELBOW){
        VertBarMaterial="Quartz";
    }

    falpha=LQBarDims.fAlpha;
    HepGeom::Vector3D<double> BarShift=GetBarShift(LQBarDims);
    HepGeom::Transform3D LQBarTotTransform=TransInMotherVolume*HepGeom::Translate3D(BarShift)*HepGeom::RotateX3D((90.0*CLHEP::deg-falpha));
    //HepGeom::Transform3D LQBarTotTransform=TransInMotherVolume;

    //G4OpticalSurface* pOptSurface=CreateReflectionOptSurface(); --TODO

    //vertical bar ------------------------------------------------------------------------------------
    double fVertAirYDim=LQBarDims.fVertBarYDim-LQBarDims.fRadiatorLength;
    GeoBox* pSolVertBar1=new GeoBox(0.5*LQBarDims.fVertBarXDim,0.5*fVertAirYDim,0.5*LQBarDims.fLBarZDim);

    //upper cut
    GeoShape* pSolVertBar=NULL;
    if(LQBarDims.bIs45degElbow){
        fd=LQBarDims.fVertBarXDim/sin(45.0*CLHEP::deg); //cut box dim
        Rot=CLHEP::HepRotation(); Rot.rotateZ(45.0*CLHEP::deg);
        vecCutShift=HepGeom::Vector3D<double>(0.5*LQBarDims.fVertBarXDim,0.5*fVertAirYDim,0.0*CLHEP::mm);
        pSolAux=new GeoBox(0.5*fd+fslimcut,0.5*fd+fslimcut,0.5*LQBarDims.fLBarZDim+fslimcut);

        HepGeom::Transform3D TransCutWnd=HepGeom::Transform3D(Rot,vecCutShift);
        GeoShapeShift* pMoveCutWnd=new GeoShapeShift(pSolAux, TransCutWnd);
        GeoShapeSubtraction* pSolVertBar2=new GeoShapeSubtraction(pSolVertBar1, pMoveCutWnd);
        //G4SubtractionSolid* pSolVertBar2=new G4SubtractionSolid("SolVertBar2",pSolVertBar1,pSolAux,pRot,vecCutShift);

        pSolVertBar=pSolVertBar2;
    }
    else{
        pSolVertBar=pSolVertBar1;
    }

    sprintf(szLabel,"%s_Q%i_LogVertBar[%i]",pszStationName,nQuarticID,nLQBarID);
    GeoLogVol* pLogVertBar=new GeoLogVol(szLabel,pSolVertBar,m_MapMaterials[std::string(VertBarMaterial)]);
    GeoOpticalPhysVol* pPhysVertBar=new GeoOpticalPhysVol(pLogVertBar);
    sprintf(szLabel,"%s_Q%i_VertBar[%i]",pszStationName,nQuarticID,nLQBarID);
    pPhysMotherVolume->add(new GeoNameTag(szLabel));
    pPhysMotherVolume->add(new GeoTransform(LQBarTotTransform));
    pPhysMotherVolume->add(pPhysVertBar);
    sprintf(szLabel,"%s_Q%i_VertAirLightGuideSurface[%i]",pszStationName,nQuarticID,nLQBarID);
    bsContainer->push_back(GeoBorderSurface(szLabel, pPhysVertBar, pPhysMotherVolume, m_pReflectionOptSurface));
    //G4LogicalVolume* pLogVertBar=new G4LogicalVolume(pSolVertBar,G4Material::GetMaterial(VertBarMaterial),"LogVertBar",NULL,0,0);
    //pLogVertBar->SetVisAttributes(pVertVisAttr);
    //G4VPhysicalVolume* pPhysVertBar=new G4PVPlacement(LBarTotTransform,pLogVertBar,"VertBar",pLogMotherVolume,false,0);
    //pSurface=new G4LogicalBorderSurface("VertAirGuideSurface",pPhysVertBar,pPhysMotherVolume,pOptSurface);

    //radiator ------------------------------------------------------------------------------------
    GeoBox* pSolRadiator1=new GeoBox(0.5*LQBarDims.fVertBarXDim,0.5*LQBarDims.fRadiatorLength,0.5*LQBarDims.fLBarZDim);

    //bottom cut
    GeoShape* pSolRadiator=NULL;
    if(LQBarDims.bApplyBottomCut)
    {
        fd=LQBarDims.fLBarZDim/sin(falpha); //cut box dim
        HepGeom::Vector3D<double> vecA4=-LQBarDims.fLBarZDim*HepGeom::Vector3D<double>(0.0,0.0,1.0);
        HepGeom::Vector3D<double> vecA5=fd/sqrt(2)*(HepGeom::RotateX3D(-(45.0*CLHEP::deg-falpha))*HepGeom::Vector3D<double>(0.0,0.0,1.0)).unit();
        HepGeom::Vector3D<double> vecX=vecA4+vecA5;
        vecCutShift=CLHEP::Hep3Vector(0.0*CLHEP::mm,-0.5*LQBarDims.fRadiatorLength,0.5*LQBarDims.fLBarZDim)+CLHEP::Hep3Vector(vecX);

        Rot=CLHEP::HepRotation(); Rot.rotateX(-(90.0*CLHEP::deg-falpha));
        pSolAux=new GeoBox(0.5*LQBarDims.fVertBarXDim+fslimcut,0.5*fd+fslimcut,0.5*fd+fslimcut);

        HepGeom::Transform3D TransCutWnd=HepGeom::Transform3D(Rot,vecCutShift);
        GeoShapeShift* pMoveCutWnd=new GeoShapeShift(pSolAux, TransCutWnd);
        GeoShapeSubtraction* pSolRadiator2=new GeoShapeSubtraction(pSolRadiator1, pMoveCutWnd);
        //G4SubtractionSolid* pSolRadiator2=new G4SubtractionSolid("SolRadiator2",pSolRadiator1,pSolAux,pRot,vecCutShift);

        //        GeoLogVol* pAuxLog=new GeoLogVol("Aux",pSolAux,m_MapMaterials[std::string("Quartz")]);
        //        GeoPhysVol* pAuxPhys=new GeoPhysVol(pAuxLog);
        //        pPhysMotherVolume->add(new GeoNameTag("Aux"));
        //        pPhysMotherVolume->add(new GeoTransform(LQBarTotTransform*HepGeom::TranslateY3D(-0.5*LQBarDims.fVertBarYDim)*TransCutWnd));
        //        pPhysMotherVolume->add(pAuxPhys);

        pSolRadiator=pSolRadiator2;
    }
    else
    {
        pSolRadiator=pSolRadiator1;
    }

    HepGeom::Transform3D TransRadiator2VertBar=HepGeom::TranslateY3D(-0.5*LQBarDims.fVertBarYDim);
    sprintf(szLabel,"%s_Q%i_LogRadiator[%i]",pszStationName,nQuarticID,nLQBarID);
    GeoLogVol* pLogRadiator=new GeoLogVol(szLabel,pSolRadiator,m_MapMaterials[std::string("Quartz")]);
    GeoPhysVol* pPhysRadiator=new GeoPhysVol(pLogRadiator);
    sprintf(szLabel,"%s_Q%i_Radiator[%i]",pszStationName,nQuarticID,nLQBarID);
    pPhysMotherVolume->add(new GeoNameTag(szLabel));
    pPhysMotherVolume->add(new GeoTransform(LQBarTotTransform*TransRadiator2VertBar));
    pPhysMotherVolume->add(pPhysRadiator);
    sprintf(szLabel,"%s_Q%i_RadiatorSurface[%i]",pszStationName,nQuarticID,nLQBarID);
    bsContainer->push_back(GeoBorderSurface(szLabel, pPhysVertBar, pPhysMotherVolume, m_pOpticalSurface));
    //G4Transform3D TransRadiator2VertBar=G4TranslateY3D(-0.5*LQBarDims.fVertBarYDim);
    //G4LogicalVolume* pLogRadiator=new G4LogicalVolume(pSolRadiator,G4Material::GetMaterial("Quartz"),"LogRadiator",NULL,0,0);
    //pLogRadiator->SetVisAttributes(m_pVisAttQuartz);
    //G4VPhysicalVolume* pPhysRadiator=new G4PVPlacement(LBarTotTransform*TransRadiator2VertBar,pLogRadiator,"Radiator",pLogMotherVolume,false,0);

    //horizontal bar ------------------------------------------------------------------------------------
    HepGeom::Transform3D HorzArmPartialTransform=LQBarTotTransform*HepGeom::TranslateY3D(0.5*fVertAirYDim-0.5*LQBarDims.fHorzBarYDim+LQBarDims.fHorzBarExtYOffset);
    AddHorizontalArm(pszStationName,nQuarticID,nLQBarID,LQBarDims,pPhysMotherVolume,HorzArmPartialTransform,bsContainer);

    //sensor --------------------------------------------------------------------------------------------
    HepGeom::Transform3D TransTotSensor=LQBarTotTransform*HepGeom::Translate3D(-0.5*TD_SENSORTHICKNESS-LQBarDims.fHorzBarXDim-0.5*LQBarDims.fVertBarXDim,0.5*fVertAirYDim-0.5*LQBarDims.fHorzBarYDim+LQBarDims.fHorzBarExtYOffset,0.0*CLHEP::mm);
    AddSensor(pszStationName,nQuarticID,nLQBarID,LQBarDims,pPhysMotherVolume,TransTotSensor);
}

void AFP_GeoModelFactory::AddHorizontalArm(const char* pszStationName, const int nQuarticID, const int nLQBarID, AFP_LQBARDIMENSIONS& LQBarDims, GeoOpticalPhysVol* pPhysMotherVolume, HepGeom::Transform3D& PartialTransInMotherVolume, GeoBorderSurfaceContainer* bsContainer)
{
    const double fslimcut=TD_SLIMCUT;
    char szLabel[64];

    std::string HorzBarMaterial;
    switch(LQBarDims.eType)
    {
    case ELBT_REGULAR:
    case ELBT_MIKELBAR:
    case ELBT_METALELBOW:
        HorzBarMaterial="Quartz";
        break;
    case ELBT_AIRGUIDE:
    case ELBT_HYBRID:
    case ELBT_HYBRIDMETALELBOW:
        HorzBarMaterial="OpticalVacuum";
        break;
    default:
        break;
    }

    bool bAddSepPart=false;
    double fTaperPartXDim=0.0*CLHEP::mm;
    GeoShape* pSolHorzBar=NULL;
    GeoBox* pSolHorzBar1=NULL;/*, *pSolHorzBar2=NULL;*/

    if((LQBarDims.eType==ELBT_HYBRID || LQBarDims.eType==ELBT_HYBRIDMETALELBOW) && LQBarDims.bSepHorzBarInTaper)
    {
        if(LQBarDims.fHorzBarTaperAngle>0.0 && LQBarDims.fHorzBarExtYOffset>0.0)
            fTaperPartXDim=LQBarDims.fHorzBarExtYOffset/tan(LQBarDims.fHorzBarTaperAngle)+LQBarDims.fHorzBarXTaperOffset;
        else fTaperPartXDim=LQBarDims.fHorzBarXTaperOffset;

        //if(LQBarDims.fHorzBarXDim-fTaperPartXDim<0) throw G4Exception("CMyDetectorConstruction::AddHorizontalArm","",RunMustBeAborted,"Cannot create taper, insufficient length of horizontal arm");

        pSolHorzBar1=new GeoBox(0.5*fTaperPartXDim,0.5*LQBarDims.fHorzBarYDim,0.5*LQBarDims.fLBarZDim);
        /*pSolHorzBar2=new GeoBox(0.5*(LQBarDims.fHorzBarXDim-fTaperPartXDim),0.5*LQBarDims.fHorzBarYDim,0.5*LQBarDims.fLBarZDim);*/
        bAddSepPart=true;
    }
    else
    {
        pSolHorzBar1=new GeoBox(0.5*LQBarDims.fHorzBarXDim,0.5*LQBarDims.fHorzBarYDim,0.5*LQBarDims.fLBarZDim);
    }

    if(LQBarDims.fHorzBarTaperAngle>0.0 && LQBarDims.fHorzBarExtYOffset>0.0)
    {
        double falpha=LQBarDims.fHorzBarTaperAngle;
        double fd=LQBarDims.fHorzBarExtYOffset/sin(falpha);
        HepGeom::Vector3D<double> vecA1=-fd*cos(falpha)*HepGeom::Vector3D<double>(1.0,0.0,0.0);
        HepGeom::Vector3D<double> vecA2=+0.5*fd*sqrt(2.0)*(HepGeom::RotateZ3D(+(45.0*CLHEP::deg-falpha))*HepGeom::Vector3D<double>(1.0,0.0,0.0)).unit();
        HepGeom::Vector3D<double> vecX=vecA1+vecA2;
        CLHEP::Hep3Vector vecCutShift=CLHEP::Hep3Vector(bAddSepPart? 0.5*fTaperPartXDim:0.5*LQBarDims.fHorzBarXDim,0.5*LQBarDims.fHorzBarYDim,0.0)+CLHEP::Hep3Vector(vecX);
        CLHEP::HepRotation Rot=CLHEP::HepRotation(); Rot.rotateZ(falpha);

        GeoBox* pSolAux=new GeoBox(0.5*fd,0.5*fd,0.5*LQBarDims.fLBarZDim+fslimcut);

        HepGeom::Transform3D TransCutWnd=HepGeom::Transform3D(Rot,vecCutShift);
        GeoShapeShift* pMoveCutWnd=new GeoShapeShift(pSolAux, TransCutWnd);
        pSolHorzBar=new GeoShapeSubtraction(pSolHorzBar1, pMoveCutWnd);
        //pSolHorzBar=new G4SubtractionSolid("SolHorzBarWithTaper",pSolHorzBar1,pSolAux,pRot,vecCutShift);
    }
    else
    {
        pSolHorzBar=pSolHorzBar1;
    }

    HepGeom::Transform3D TransInMotherVolume;
    GeoLogVol* pLogHorzBar=NULL;
    GeoOpticalPhysVol* pPhysHorzBar;

    if(bAddSepPart)
    {
        TransInMotherVolume=PartialTransInMotherVolume*HepGeom::TranslateX3D(-0.5*fTaperPartXDim-0.5*LQBarDims.fVertBarXDim);
        sprintf(szLabel,"%s_Q%i_LogHorzBarTaper[%i]",pszStationName, nQuarticID, nLQBarID);
        pLogHorzBar=new GeoLogVol(szLabel,pSolHorzBar,m_MapMaterials[(LQBarDims.eTaperMaterial==EM_VACUUM)? std::string("OpticalVacuum"):std::string("Quartz")]);
        pPhysHorzBar=new GeoOpticalPhysVol(pLogHorzBar);
        sprintf(szLabel,"%s_Q%i_HorzBarTaper[%i]",pszStationName, nQuarticID, nLQBarID);
        pPhysMotherVolume->add(new GeoNameTag(szLabel));
        pPhysMotherVolume->add(new GeoTransform(TransInMotherVolume));
        pPhysMotherVolume->add(pPhysHorzBar);
        sprintf(szLabel,"%s_Q%i_HorzTaperAirGuideSurface[%i]",pszStationName,nQuarticID,nLQBarID);
        bsContainer->push_back(GeoBorderSurface(szLabel, pPhysHorzBar, pPhysMotherVolume, (LQBarDims.eTaperMaterial==EM_VACUUM)? m_pReflectionOptSurface:m_pOpticalSurface));
        //TransInMotherVolume=PartialTransInMotherVolume*G4TranslateX3D(-0.5*fTaperPartXDim-0.5*LQBarDims.fVertBarXDim);
        //pLogHorzBar=new G4LogicalVolume(pSolHorzBar, (LQBarDims.eTaperMaterial==EM_OpticalVacuum)? G4Material::GetMaterial("OpticalVacuum"):G4Material::GetMaterial("Quartz"),"LogHorzBarTaper",NULL,0,0);
        //pLogHorzBar->SetVisAttributes((LQBarDims.eTaperMaterial==EM_OpticalVacuum)? m_pVisAttAirGuide:m_pVisAttQuartz);
        //pPhysHorzBar=new G4PVPlacement(TransInMotherVolume,pLogHorzBar,"HorzBarTaper",pPhysMotherVolume->GetLogicalVolume(),false,0);
        //if(LQBarDims.eTaperMaterial==EM_OpticalVacuum) pDummyPtr=new G4LogicalBorderSurface("HorzTaperAirGuideSurface",pPhysHorzBar,pPhysMotherVolume,pOptSurface);

        TransInMotherVolume=PartialTransInMotherVolume*HepGeom::TranslateX3D(-0.5*(LQBarDims.fHorzBarXDim-fTaperPartXDim)-fTaperPartXDim-0.5*LQBarDims.fVertBarXDim);
        sprintf(szLabel,"%s_Q%i_LogHorzBar[%i]",pszStationName, nQuarticID, nLQBarID);
        pLogHorzBar=new GeoLogVol(szLabel,pSolHorzBar,m_MapMaterials[std::string("OpticalVacuum")]);
        pPhysHorzBar=new GeoOpticalPhysVol(pLogHorzBar);
        sprintf(szLabel,"%s_Q%i_HorzBar[%i]",pszStationName, nQuarticID, nLQBarID);
        pPhysMotherVolume->add(new GeoNameTag(szLabel));
        pPhysMotherVolume->add(new GeoTransform(TransInMotherVolume));
        pPhysMotherVolume->add(pPhysHorzBar);
        sprintf(szLabel,"%s_Q%i_HorzAirGuideSurface[%i]",pszStationName,nQuarticID,nLQBarID);
        bsContainer->push_back(GeoBorderSurface(szLabel, pPhysHorzBar, pPhysMotherVolume, m_pReflectionOptSurface));
        //TransInMotherVolume=PartialTransInMotherVolume*G4TranslateX3D(-0.5*(LQBarDims.fHorzBarXDim-fTaperPartXDim)-fTaperPartXDim-0.5*LQBarDims.fVertBarXDim);
        //pLogHorzBar=new G4LogicalVolume(pSolHorzBar2,G4Material::GetMaterial("OpticalVacuum"),"LogHorzBar",NULL,0,0);
        //pLogHorzBar->SetVisAttributes(m_pVisAttAirGuide);
        //pPhysHorzBar=new G4PVPlacement(TransInMotherVolume,pLogHorzBar,"HorzBar",pPhysMotherVolume->GetLogicalVolume(),false,0);
        //pDummyPtr=new G4LogicalBorderSurface("HorzAirGuideSurface",pPhysHorzBar,pPhysMotherVolume,pOptSurface);
    }
    else
    {
        TransInMotherVolume=PartialTransInMotherVolume*HepGeom::TranslateX3D(-0.5*LQBarDims.fHorzBarXDim-0.5*LQBarDims.fVertBarXDim);
        sprintf(szLabel,"%s_Q%i_LogHorzBar[%i]",pszStationName, nQuarticID, nLQBarID);
        pLogHorzBar=new GeoLogVol(szLabel,pSolHorzBar,m_MapMaterials[HorzBarMaterial]);
        pPhysHorzBar=new GeoOpticalPhysVol(pLogHorzBar);
        sprintf(szLabel,"%s_Q%i_HorzBar[%i]",pszStationName, nQuarticID, nLQBarID);
        pPhysMotherVolume->add(new GeoNameTag(szLabel));
        pPhysMotherVolume->add(new GeoTransform(TransInMotherVolume));
        pPhysMotherVolume->add(pPhysHorzBar);
        if(LQBarDims.eType==ELBT_HYBRID || LQBarDims.eType==ELBT_HYBRIDMETALELBOW)
        {
            sprintf(szLabel,"%s_Q%i_HorzAirGuideSurface[%i]",pszStationName,nQuarticID,nLQBarID);
            bsContainer->push_back(GeoBorderSurface(szLabel, pPhysHorzBar, pPhysMotherVolume, m_pReflectionOptSurface));
        }
        //TransInMotherVolume=PartialTransInMotherVolume*G4TranslateX3D(-0.5*LQBarDims.fHorzBarXDim-0.5*LQBarDims.fVertBarXDim);
        //pLogHorzBar=new G4LogicalVolume(pSolHorzBar,G4Material::GetMaterial(HorzBarMaterial),"LogHorzBar",NULL,0,0);
        //pLogHorzBar->SetVisAttributes(pHorzVisAttr);
        //pPhysHorzBar=new G4PVPlacement(TransInMotherVolume,pLogHorzBar,"HorzBar",pPhysMotherVolume->GetLogicalVolume(),false,0);
        //if(LQBarDims.eType==ELBT_HYBRID || LQBarDims.eType==ELBT_HYBRIDMETALELBOW){
        //   pDummyPtr=new G4LogicalBorderSurface("HorzAirGuideSurface",pPhysHorzBar,pPhysMotherVolume,pOptSurface);
        //}
    }
}

void AFP_GeoModelFactory::AddSensor(const char* pszStationName, const int nQuarticID, const int nLQBarID, AFP_LQBARDIMENSIONS& LQBarDims, GeoOpticalPhysVol* pPhysMotherVolume, HepGeom::Transform3D &TransInMotherVolume)
{
    int i;
    double fFirstSensorZPos;
    HepGeom::Transform3D TotTransform;
    char szLabel[64];

    double fSensorLength=LQBarDims.fLBarZDim/LQBarDims.nNumOfSensors;
    bool bIsOddSensorNumber=(LQBarDims.nNumOfSensors%2==0)? false:true;
    GeoBox* pSolSensor=new GeoBox(0.5*TD_SENSORTHICKNESS,0.5*LQBarDims.fHorzBarYDim,0.5*fSensorLength);

    fFirstSensorZPos=bIsOddSensorNumber? -((LQBarDims.nNumOfSensors-1)>>1)*fSensorLength:-((LQBarDims.nNumOfSensors>>1)-0.5)*fSensorLength;

    for(i=0;i<LQBarDims.nNumOfSensors;i++)
    {
        TotTransform=TransInMotherVolume*HepGeom::TranslateZ3D(fFirstSensorZPos+i*fSensorLength);
        sprintf(szLabel,"%s_Q%i_LogTDSensor[%i][%02i]",pszStationName,nQuarticID,nLQBarID,(m_CfgParams.tdcfg.nColsCnt>1)? nLQBarID:i);
        GeoLogVol* pLogSensor=new GeoLogVol(szLabel,pSolSensor,m_MapMaterials["SiliconPMT"]);
        GeoOpticalPhysVol* pPhysSensor=new GeoOpticalPhysVol(pLogSensor);
        //sprintf(szLabel,"%s_Q%i_TDSensor[%i]",pszStationName,nQuarticID,nRowsCnt*i+j);
        sprintf(szLabel,"%s_Q%i_TDSensor[%i][%02i]",pszStationName,nQuarticID,nLQBarID,(m_CfgParams.tdcfg.nColsCnt>1)? nLQBarID:i);
        pPhysMotherVolume->add(new GeoNameTag(szLabel));
        pPhysMotherVolume->add(new GeoTransform(TotTransform));
        pPhysMotherVolume->add(pPhysSensor);
        //TotTransform=TransInMotherVolume*HepGeom::TranslateZ3D(fFirstSensorZPos+i*fSensorLength);
        //sprintf(szLabel,"%s_Q%i_LogSensor[%i][%02i]",pszStationName,nQuarticID,nLQBarID,(m_LBarDimensions.nNumOfLBars>1)? nLQBarID:i);
        //G4LogicalVolume* pLogSensor=new G4LogicalVolume(pSolSensor,G4Material::GetMaterial("BoroGlass"),szbuff,NULL,0,0);
        //sprintf(szbuff,"Sensor%02i",i);
        //G4VPhysicalVolume* pPhysSensor=new G4PVPlacement(TotTransform,pLogSensor,szbuff,pLogMotherVolume,false,0);
        //m_mapTransToSensorCS[i]=TransInMotherVolume.inverse();

        pPhysSensor=NULL;
    }

}
