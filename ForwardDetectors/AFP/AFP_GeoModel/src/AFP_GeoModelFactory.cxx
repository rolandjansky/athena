/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_GeoModelFactory.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoBox.h"
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
#include "StoreGate/DataHandle.h"

#include "GeoModelUtilities/GeoOpticalPhysVol.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelUtilities/GeoExtendedMaterial.h"
#include "GeoModelUtilities/GeoMaterialPropertiesTable.h"
#include "GeoModelUtilities/GeoBorderSurfaceContainer.h"
#include "AFP_Geometry/AFP_Geometry.h"

#include <iostream>
#include <fstream>

#include <math.h>
#include <algorithm>
#include <list>
#include <map>
#include <stdlib.h>
#include <string>


using namespace Genfun;
using namespace CLHEP;

AFP_GeoModelFactory::AFP_GeoModelFactory(StoreGateSvc *detStore, AFP_Geometry* pGeometry)
    :m_detectorManager(NULL), m_detectorStore(detStore)
{
    pGeometry->GetCfgParams(&m_CfgParams);
    m_pGeometry=pGeometry;

    InitializeTDParameters();
}


AFP_GeoModelFactory::~AFP_GeoModelFactory()
{
    m_CfgParams.clear();
}

void AFP_GeoModelFactory::DefineMaterials()
{
    int i;
    const double fWl2E=1239.85; //nm<->eV
    std::string matName;
    GeoMaterialPropertiesTable *pMPT=NULL;

    DataHandle<StoredMaterialManager> materialManager;
    if (StatusCode::SUCCESS != m_detectorStore->retrieve(materialManager, std::string("MATERIALS"))) {
        return;
    }

    double aH,aC,aN,aSi,aP,aS,aCr,aMn,aFe,aNi,aMo,aAl,aO,Atot;
    const GeoElement* H  = materialManager->getElement("Hydrogen");
    const GeoElement* C  = materialManager->getElement("Carbon");
    const GeoElement* N  = materialManager->getElement("Nitrogen");
    const GeoElement* Si = materialManager->getElement("Silicon");
    const GeoElement* P  = materialManager->getElement("Phosphorus");
    const GeoElement* S  = materialManager->getElement("Sulfur");
    const GeoElement* Cr = materialManager->getElement("Chromium");
    const GeoElement* Mn = materialManager->getElement("Manganese");
    const GeoElement* Fe = materialManager->getElement("Iron");
    const GeoElement* Ni = materialManager->getElement("Nickel");
    const GeoElement* Mo = materialManager->getElement("Molybdenum");
    const GeoElement* Al = materialManager->getElement("Aluminium");
    const GeoElement* O = materialManager->getElement("Oxygen");
    const GeoElement* Be = materialManager->getElement("Beryllium");

    // vacuum
    matName = "std::Vacuum";
    GeoMaterial *vacuum = materialManager->getMaterial(matName);
    m_MapMaterials.insert(std::pair<std::string,GeoMaterial*>(matName,vacuum));

    // optical vacuum
    matName = "OpticalVacuum";
    GeoExtendedMaterial* pMatOptVacuum=new GeoExtendedMaterial(matName, (1E-24)*CLHEP::g/CLHEP::cm3);
    pMatOptVacuum->add(const_cast<GeoElement*> (H), 1);
    double PhotonEnergyOptVac[2] = {0.44*CLHEP::eV, 6.3*CLHEP::eV};
    double RefractiveIndexOptVac[2] = {1.0, 1.0};
	//double AbsorptionOptVac[2] ={10E+8*CLHEP::m, 10E+8*CLHEP::m};
	double AbsorptionOptVac[2] ={1E-5*CLHEP::m, 1E-5*CLHEP::m};

    pMPT= new GeoMaterialPropertiesTable();
    pMPT->AddProperty("RINDEX", PhotonEnergyOptVac, RefractiveIndexOptVac , 2);
    pMPT->AddProperty("ABSLENGTH", PhotonEnergyOptVac, AbsorptionOptVac, 2);
    pMatOptVacuum->SetMaterialPropertiesTable(pMPT);
    pMatOptVacuum->lock();
    m_MapMaterials.insert(std::pair<std::string,GeoMaterial*>(matName,pMatOptVacuum));

    // Steel Grade 316L (Roman Pot)
    matName="Steel";
    GeoMaterial *steel=new GeoMaterial("Steel", 8*g/cm3);

    aFe=62.045*Fe->getA()/(CLHEP::g/CLHEP::mole);
    aC =0.03*C ->getA()/(CLHEP::g/CLHEP::mole);
    aMn=2.0*Mn ->getA()/(CLHEP::g/CLHEP::mole);
    aSi=0.75*Si->getA()/(CLHEP::g/CLHEP::mole);
    aP =0.045*P->getA()/(CLHEP::g/CLHEP::mole);
    aS =0.03*S ->getA()/(CLHEP::g/CLHEP::mole);
    aCr=18.0*Cr->getA()/(CLHEP::g/CLHEP::mole);
    aMo=3.0*Mo ->getA()/(CLHEP::g/CLHEP::mole);
    aNi=14.0*Ni->getA()/(CLHEP::g/CLHEP::mole);
    aN =0.10*N ->getA()/(CLHEP::g/CLHEP::mole);
    Atot=aFe+aC+aMn+aSi+aP+aS+aCr+aMo+aNi+aN;

    steel->add(const_cast<GeoElement*> (Fe),aFe/Atot);
    steel->add(const_cast<GeoElement*> (C),  aC/Atot);
    steel->add(const_cast<GeoElement*> (Mn),aMn/Atot);
    steel->add(const_cast<GeoElement*> (Si),aSi/Atot);
    steel->add(const_cast<GeoElement*> (P),  aP/Atot);
    steel->add(const_cast<GeoElement*> (S),  aS/Atot);
    steel->add(const_cast<GeoElement*> (Cr),aCr/Atot);
    steel->add(const_cast<GeoElement*> (Mo),aMo/Atot);
    steel->add(const_cast<GeoElement*> (Ni),aNi/Atot);
    steel->add(const_cast<GeoElement*> (N),  aN/Atot);
    steel->lock();
    m_MapMaterials.insert(std::pair<std::string,GeoMaterial*>(matName,steel));

    // Silicon 100% (Detector)
    matName="Silicon";
    GeoMaterial* pMaterial=new GeoMaterial(matName, 2.329*CLHEP::g/CLHEP::cm3);

    pMaterial->add(const_cast<GeoElement*> (Si), 1);
    pMaterial->lock();
    m_MapMaterials.insert(std::pair<std::string,GeoMaterial*>(matName,pMaterial));

    // CE7 70% Si, 30% Al
    matName="CE7";
    GeoMaterial *pMatCE7=new GeoMaterial(matName, 2.4*g/cm3);
    aSi=0.70*Si->getA()/(g/mole);
    aAl=0.30*Al->getA()/(g/mole);
    Atot=aSi+aAl;
    pMatCE7->add(const_cast<GeoElement*> (Si),aSi/Atot);
    pMatCE7->add(const_cast<GeoElement*> (Al),aAl/Atot);
    pMatCE7->lock();
    m_MapMaterials.insert(std::pair<std::string,GeoMaterial*>(matName,pMatCE7));

    // Quartz 70% SiO2 for TD Quartic (with refractive index and absorption length)
    matName="Quartz";
    GeoExtendedMaterial *pMatQuartz=new GeoExtendedMaterial(matName, 2.6*g/cm3, stateSolid, CLHEP::STP_Temperature);
    aSi=1.0*Si->getA()/(g/mole);
    aO=2.0*O->getA()/(g/mole);
    Atot=aSi+aO;
    pMatQuartz->add(const_cast<GeoElement*> (Si),aSi/Atot);
    pMatQuartz->add(const_cast<GeoElement*> (O),aO/Atot);

    // Defining the refractive index of quartz
    const int nEntriesCnt1=12; // (M.G.Albrow, JINST 2012) + extrapolation to 200.0 nm
    double arrEnergy1[nEntriesCnt1] = { 750.0*nm, 700.0*nm, 650.0*nm, 600.0*nm, 550.0*nm, 500.0*nm, 450.0*nm, 400.0*nm, 350.0*nm, 300.0*nm, 250.0*nm, 200.0*nm };
    double arrQuartzRefIndex[nEntriesCnt1] = { 1.450, 1.455, 1.456, 1.458, 1.460, 1.462, 1.465, 1.470, 1.475, 1.488, 1.510, 1.541 };
    double arrQuartzAbsLength[nEntriesCnt1] = { 130.0*cm, 130.0*cm, 130.0*cm, 130.0*cm, 128.0*cm, 125.0*cm, 122.0*cm, 120.0*cm, 111.0*cm, 104.0*cm, 95.0*cm, 83.3*cm };
    for(i=0;i<nEntriesCnt1;i++) arrEnergy1[i]=(fWl2E/(arrEnergy1[i]/nm))*eV;

    pMPT= new GeoMaterialPropertiesTable();
    pMPT->AddProperty("RINDEX", arrEnergy1, arrQuartzRefIndex, nEntriesCnt1);
    pMPT->AddProperty("ABSLENGTH", arrEnergy1, arrQuartzAbsLength, nEntriesCnt1);
    pMatQuartz->SetMaterialPropertiesTable(pMPT);
    pMatQuartz->lock();
    m_MapMaterials.insert(std::pair<std::string,GeoMaterial*>(matName,pMatQuartz));

    // Silicon for TD SiPMT (with refractive index)
    matName="SiliconPMT";
    GeoExtendedMaterial *pMatSiliconPMT=new GeoExtendedMaterial(matName, 2.3290*g/cm3, stateSolid, CLHEP::STP_Temperature);
    aSi=1.0*Si->getA()/(g/mole);
    pMatSiliconPMT->add(const_cast<GeoElement*> (Si),1.0);

    const int nEntriesCnt2=2;
    double arrEnergy2[nEntriesCnt2] = { 2800.0*nm, 190.0*nm };
    double arrSiliconRefIndex[nEntriesCnt2] = { 4.0, 4.0 };
    double arrSiliconAbsLength[nEntriesCnt2] = { 0.00001*m, 0.00001*m };
    for(i=0;i<nEntriesCnt2;i++) arrEnergy2[i]=(fWl2E/(arrEnergy2[i]/nm))*eV;

    pMPT = new GeoMaterialPropertiesTable();
    pMPT->AddProperty("RINDEX", arrEnergy2, arrSiliconRefIndex, nEntriesCnt2);
    pMPT->AddProperty("ABSLENGTH", arrEnergy2, arrSiliconAbsLength, nEntriesCnt2);
    pMatSiliconPMT->SetMaterialPropertiesTable(pMPT);
    pMatSiliconPMT->lock();
    m_MapMaterials.insert(std::pair<std::string,GeoMaterial*>(matName,pMatSiliconPMT));


    // Silicon
    matName="Silicon";
    GeoMaterial *pMatSilicon=new GeoMaterial(matName, 2.3290*g/cm3);
    aSi=1.0*Si->getA()/(g/mole);
    pMatSilicon->add(const_cast<GeoElement*> (Si),1.0);
    pMatSilicon->lock();
    m_MapMaterials.insert(std::pair<std::string,GeoMaterial*>(matName,pMatSilicon));

    // Water
    matName="Water";
    GeoMaterial *pMatWater=new GeoMaterial(matName, 1.0*g/cm3);
    aH=0.11*Si->getA()/(g/mole);
    aO=0.89*Al->getA()/(g/mole);
    Atot=aH+aO;
    pMatWater->add(const_cast<GeoElement*> (H),aH/Atot);
    pMatWater->add(const_cast<GeoElement*> (O),aO/Atot);
    pMatWater->lock();
    m_MapMaterials.insert(std::pair<std::string,GeoMaterial*>(matName,pMatWater));

    // Beryllium
    matName="Beryllium";
    GeoMaterial* pMaterialBe=new GeoMaterial(matName, 1.848*CLHEP::g/CLHEP::cm3);

    pMaterialBe->add(const_cast<GeoElement*> (Be), 1);
    pMaterialBe->lock();
    m_MapMaterials.insert(std::pair<std::string,GeoMaterial*>(matName,pMaterialBe));
}


void AFP_GeoModelFactory::create(GeoPhysVol *world)
{
    char szLabel[32];
    StatusCode SCode;
    m_detectorManager=new AFP_GeoModelManager();
    HepGeom::Transform3D TransEnvInWorld;
    HepGeom::Transform3D PosElementInEnv;

    //define materials
    DefineMaterials();

    // Initialization of Surface Container for TD Surface(s)
    GeoBorderSurfaceContainer* pBSContainer = new GeoBorderSurfaceContainer();
    pBSContainer->reserve(TD_TRAINSCNT*TD_COLUMNSCNT);

    //AFP00 (SIDE A (+z)) ------------------------------------------------------------------------------------------------------------------------------------------------------
    //add envelope -- long beampipe (station A)
    TransEnvInWorld=m_pGeometry->GetStationTransform("AFP00"); //HepGeom::Translate3D(-97.0*CLHEP::mm,0.0*CLHEP::mm,m_CfgParams.fLongHBZPos);
    const GeoBox* pBoxLongEnv= new GeoBox(150*mm, 150*CLHEP::mm, 280*CLHEP::mm);
    const GeoLogVol* pLogLongEnv = new GeoLogVol("AFP00_LogLongHamburgPipeEnv", pBoxLongEnv, m_MapMaterials[std::string("OpticalVacuum")]);
    GeoOpticalPhysVol* pPhysLongEnv   = new GeoOpticalPhysVol(pLogLongEnv);
    sprintf(szLabel,"AFP00_LongHamburgPipeEnv");
    world->add(new GeoTransform(TransEnvInWorld));
    world->add(new GeoNameTag(szLabel));
    world->add(pPhysLongEnv);
    m_detectorManager->addTreeTop(pPhysLongEnv);

    //add Long Hamburg Pipe (station A)
    PosElementInEnv=m_pGeometry->GetStationElementTransform("AFP00",ESE_RPOT);
    AddRomanPot(pPhysLongEnv,"AFP00",PosElementInEnv);

    //add Timing detector
    //PosElementInEnv=HepGeom::Translate3D(m_CfgParams.tdcfg.fDistanceToFloor, m_CfgParams.fLongHBYPos+m_CfgParams.fQuarticYPosInTube[0], m_CfgParams.fQuarticZPosInTube[0]);
    PosElementInEnv=m_pGeometry->GetStationElementTransform("AFP00",ESE_TOF);
    SCode=AddTimingDetector("AFP00",pPhysLongEnv,PosElementInEnv, pBSContainer);

    //add Silicon detector
    //PosElementInEnv=HepGeom::Translate3D(m_CfgParams.fLongHBFloorXPos,m_CfgParams.fLongHBYPos,-200.0*CLHEP::mm);
    AddSiDetector(pPhysLongEnv,"AFP00");

    //AFP01 (SIDE A (+z)) ------------------------------------------------------------------------------------------------------------------------------------------------------
    //add envelope -- short beampipe (station A)
    TransEnvInWorld=m_pGeometry->GetStationTransform("AFP01"); //HepGeom::Translate3D(-97.0*CLHEP::mm, 0.0*CLHEP::mm, m_CfgParams.fShortHBZPos);
    const GeoBox* pBoxShortEnv= new GeoBox(150*CLHEP::mm, 150*CLHEP::mm, 280*CLHEP::mm);
    const GeoLogVol* pLogShortEnv = new GeoLogVol("AFP01_LogShortHamburgPipeEnv", pBoxShortEnv, m_MapMaterials[std::string("std::Vacuum")]);
    GeoPhysVol* pPhysShortEnv   = new GeoPhysVol(pLogShortEnv);
    sprintf(szLabel,"AFP01_ShortHamburgPipeEnv");
    world->add(new GeoTransform(TransEnvInWorld));
    world->add(new GeoNameTag(szLabel));
    world->add(pPhysShortEnv);
    m_detectorManager->addTreeTop(pPhysShortEnv);

    //add Short Hamburg Pipe (station A)
    PosElementInEnv=m_pGeometry->GetStationElementTransform("AFP01",ESE_RPOT);
    AddRomanPot(pPhysShortEnv,"AFP01",PosElementInEnv);

    //add Silicon detector
    //PosElementInEnv=HepGeom::Translate3D(m_CfgParams.fShortHBFloorXPos,m_CfgParams.fShortHBYPos,0.0*CLHEP::mm);
    AddSiDetector(pPhysShortEnv,"AFP01");

    //AFP02 (SIDE C (-z)) ------------------------------------------------------------------------------------------------------------------------------------------------------
    //add envelope -- short beampipe (station C)
    TransEnvInWorld=m_pGeometry->GetStationTransform("AFP02"); //HepGeom::Translate3D(-97.0*CLHEP::mm,0.0*CLHEP::mm,-m_CfgParams.fShortHBZPos);
    const GeoBox* pBoxShortEnv1= new GeoBox(150*CLHEP::mm, 150*CLHEP::mm, 280*CLHEP::mm);
    const GeoLogVol* pLogShortEnv1 = new GeoLogVol("AFP02_LogShortHamburgPipeEnv", pBoxShortEnv1, m_MapMaterials[std::string("std::Vacuum")]);
    GeoPhysVol* pPhysShortEnv1   = new GeoPhysVol(pLogShortEnv1);
    sprintf(szLabel,"AFP02_ShortHamburgPipeEnv");
    world->add(new GeoTransform(TransEnvInWorld));
    world->add(new GeoNameTag(szLabel));
    world->add(pPhysShortEnv1);
    m_detectorManager->addTreeTop(pPhysShortEnv1);

    //add Short Hamburg Pipe (station C)
    PosElementInEnv=m_pGeometry->GetStationElementTransform("AFP02",ESE_RPOT);
    AddRomanPot(pPhysShortEnv1,"AFP02",PosElementInEnv);

    //add Silicon detector
    //PosElementInEnv=HepGeom::Translate3D(m_CfgParams.fShortHBFloorXPos,m_CfgParams.fShortHBYPos,0.0*CLHEP::mm)*HepGeom::Reflect3D(0.0,0.0,1.0,0.0);
    AddSiDetector(pPhysShortEnv1,"AFP02");

    //AFP03 (SIDE C (-z)) ------------------------------------------------------------------------------------------------------------------------------------------------------
    //add envelope -- long beampipe (station C)
    TransEnvInWorld=m_pGeometry->GetStationTransform("AFP03"); //HepGeom::Translate3D(-97.0*CLHEP::mm,0.0*CLHEP::mm,-m_CfgParams.fLongHBZPos);
    const GeoBox* pBoxLongEnv1= new GeoBox(150*mm, 150*CLHEP::mm, 280*CLHEP::mm);
    const GeoLogVol* pLogLongEnv1 = new GeoLogVol("AFP03_LogLongHamburgPipeEnv", pBoxLongEnv1, m_MapMaterials[std::string("OpticalVacuum")]);
    GeoOpticalPhysVol* pPhysLongEnv1   = new GeoOpticalPhysVol(pLogLongEnv1);
    sprintf(szLabel,"AFP03_LongHamburgPipeEnv");
    world->add(new GeoTransform(TransEnvInWorld));
    world->add(new GeoNameTag(szLabel));
    world->add(pPhysLongEnv1);
    m_detectorManager->addTreeTop(pPhysLongEnv1);

    //add Long Hamburg Pipe (station C)
    PosElementInEnv=m_pGeometry->GetStationElementTransform("AFP03",ESE_RPOT);
	AddRomanPot( pPhysLongEnv1,"AFP03",PosElementInEnv);

    //add Timing detector - Q1
    //PosElementInEnv=HepGeom::Translate3D(m_CfgParams.fLongHBFloorXPos, m_CfgParams.fLongHBYPos+m_CfgParams.fQuarticYPosInTube[0], -m_CfgParams.fQuarticZPosInTube[0])*HepGeom::Reflect3D(0.0,0.0,1.0,0.0);
    PosElementInEnv=m_pGeometry->GetStationElementTransform("AFP03",ESE_TOF);
    SCode=AddTimingDetector("AFP03",pPhysLongEnv1,PosElementInEnv, pBSContainer);

    //add Silicon detector
    //PosElementInEnv=HepGeom::Translate3D(m_CfgParams.fLongHBFloorXPos,m_CfgParams.fLongHBYPos,+200.0*CLHEP::mm)*HepGeom::Reflect3D(0.0,0.0,1.0,0.0);
    AddSiDetector(pPhysLongEnv1,"AFP03");

    //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // Register the Surface Container with the Detector Store
    SCode=m_detectorStore->record(pBSContainer, "AFP_GeoModel");
    if (SCode.isFailure()){

    }
}

const AFP_GeoModelManager * AFP_GeoModelFactory::getDetectorManager() const
{
    return m_detectorManager;
}


void AFP_GeoModelFactory::UpdatePositions(PAFP_BPMCOOLPARAMS /*pBpmParams*/)
{

}
