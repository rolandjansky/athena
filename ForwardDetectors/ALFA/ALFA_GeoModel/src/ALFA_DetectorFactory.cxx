/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#include "GeoModelSvc/AbsMaterialManager.h"
#include "GeoModelKernel/GeoMaterial.h"  

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeUnion.h" 
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoLogVol.h"  
#include "GeoModelKernel/GeoNameTag.h"  

#include "GeoModelKernel/GeoPhysVol.h" 
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelUtilities/StoredAlignX.h"
#include "GeoModelUtilities/StoredPhysVol.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

//#include "RDBAccessSvc/IRDBAccessSvc.h"
#include <math.h>
#include <stdio.h>

#include <list>
#include <map>
#include <string>
#include <algorithm>

#include "ALFA_DetectorTool.h"
#include "ALFA_DetectorFactory.h"
#include "ALFA_Geometry/ALFA_GeometryReader.h"

using namespace std;

double ALFA_stagger[10] = {0.0, 0.283, -0.141, 0.141, -0.283, 0.354, -0.071, 0.212, -0.212, 0.071};
double OD_stagger[3] = {0.0, -0.167, -0.334};

#define BEAMPIPEINNERRADIUS (0.5*80.0*CLHEP::mm)
#define BEAMPIPEOUTERRADIUS (0.5*84.0*CLHEP::mm)

void CONFIGURATION::clear()
{
	GeometryConfig.clear();

	bConstructBeampipe=false;
	bAddIBP=true;

	bIsTransformInStation.assign(8, false);
	bIsTransformInDetector.assign(8, false);

	pointTransformInDetectorB7L1U.assign(3, 0.0);
	pointTransformInDetectorB7L1L.assign(3, 0.0);
	pointTransformInDetectorA7L1U.assign(3, 0.0);
	pointTransformInDetectorA7L1L.assign(3, 0.0);
	pointTransformInDetectorB7R1U.assign(3, 0.0);
	pointTransformInDetectorB7R1L.assign(3, 0.0);
	pointTransformInDetectorA7R1U.assign(3, 0.0);
	pointTransformInDetectorA7R1L.assign(3, 0.0);

	vecTransformInDetectorB7L1U.assign(7, 0.0);
	vecTransformInDetectorB7L1L.assign(7, 0.0);
	vecTransformInDetectorA7L1U.assign(7, 0.0);
	vecTransformInDetectorA7L1L.assign(7, 0.0);
	vecTransformInDetectorB7R1U.assign(7, 0.0);
	vecTransformInDetectorB7R1L.assign(7, 0.0);
	vecTransformInDetectorA7R1U.assign(7, 0.0);
	vecTransformInDetectorA7R1L.assign(7, 0.0);

	vecTransformInStationB7L1U.assign(7, 0.0);
	vecTransformInStationB7L1L.assign(7, 0.0);
	vecTransformInStationA7L1U.assign(7, 0.0);
	vecTransformInStationA7L1L.assign(7, 0.0);
	vecTransformInStationB7R1U.assign(7, 0.0);
	vecTransformInStationB7R1L.assign(7, 0.0);
	vecTransformInStationA7R1U.assign(7, 0.0);
	vecTransformInStationA7R1L.assign(7, 0.0);
}

ALFA_DetectorFactory::ALFA_DetectorFactory(StoreGateSvc *pDetStore,IRDBAccessSvc *pAccess, const PCONFIGURATION pConfig)
  :m_pDetectorManager(NULL),m_pMaterialManager(NULL),m_pDetectorStore(pDetStore),m_pIRDBAccess(pAccess)
{
	//create geometry reader
	m_eRequestedMetrologyType=(eMetrologyType)(pConfig->GeometryConfig.eRPMetrologyGeoType);
	m_Config=*pConfig;
	m_ListExistingRPots.clear();
	m_MapMaterials.clear();
	m_pGeoReader=(ALFA_GeometryReader*)new ALFA_GeometryReader();

}

ALFA_DetectorFactory::~ALFA_DetectorFactory()
{
	if(m_pGeoReader!=NULL) delete m_pGeoReader;
}

bool ALFA_DetectorFactory::ReadGeometry(bool bAlignCorrections)
{
	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_DetectorFactory::ReadGeometry");

	bool bRes=false;
	eRPotName eRPName;
	list<eRPotName>::const_iterator iterRPName;

	if(m_pGeoReader!=NULL) delete m_pGeoReader;
	m_pGeoReader=(ALFA_GeometryReader*)new ALFA_GeometryReader();

	//first we need to setup to zero positions for SWCORRECTIONS
	if(m_eRequestedMetrologyType==EMT_SWCORRECTIONS){
		if(bAlignCorrections) m_Config.GeometryConfig.eRPMetrologyGeoType=EMT_SWCORRECTIONS;
		else m_Config.GeometryConfig.eRPMetrologyGeoType=EMT_UNDEFINED; //!!!
	}
	else if(m_eRequestedMetrologyType==EMT_NOMINAL){
		for(int i=0;i<RPOTSCNT;i++){
			eRPName=(eRPotName)(i+1);
			if(m_Config.bIsTransformInDetector[i]==true){
				m_Config.GeometryConfig.CfgRPosParams[i].usercorr.bIsEnabledUserTranform=true;
				m_Config.GeometryConfig.CfgRPosParams[i].usercorr.UserOriginOfDetTransInRPot=Point3DInDetector(eRPName);
				m_Config.GeometryConfig.CfgRPosParams[i].usercorr.UserTransformOfDetInRPot=UserTransformInDetector(eRPName);
			}
			if(m_Config.bIsTransformInStation[i]==true){
				m_Config.GeometryConfig.CfgRPosParams[i].usercorr.bIsEnabledUserTranform=true;
				m_Config.GeometryConfig.CfgRPosParams[i].usercorr.UserTransformOfRPInStation=UserTransformInStation(eRPName);
			}
		}
	}

		if((bRes=m_pGeoReader->Initialize(&m_Config.GeometryConfig,EFCS_CLADDING))==true){
		LogStream<<MSG::INFO<<"Geometry successfully initialized"<<endmsg;
		m_pGeoReader->GetListOfExistingRPotIDs(&m_ListExistingRPots);
	}
	else
	{
		bRes=false;
		LogStream<<MSG::FATAL<<"Cannot initialize geometry"<<endmsg;
		throw GaudiException(" Could not load geometry ", "ALFA_DetectorFactory::ReadGeometry", StatusCode::FAILURE);
	}

	if(m_eRequestedMetrologyType==EMT_SWCORRECTIONS || bAlignCorrections==true){
		SaveGeometry();
	}
	
	return bRes;
}

void ALFA_DetectorFactory::SaveGeometry()
{
	eGeoSourceType eGeoType;
	char szFilename[64];
	char szPrefix[8];
	list<eRPotName>::const_iterator iterRPName;
	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_DetectorFactory::SaveGeometry");
		
	for(iterRPName=m_ListExistingRPots.begin();iterRPName!=m_ListExistingRPots.end();iterRPName++)
	{
		if(m_eRequestedMetrologyType==EMT_NOMINAL || m_eRequestedMetrologyType==EMT_METROLOGY){
			//MD fibers
			eGeoType=m_pGeoReader->GetRPGeometryType(*iterRPName,EFT_FIBERMD);
			if(eGeoType!=EGST_UNDEFINED){
				switch(eGeoType){
					case EGST_IDEALGEOMETRY:
						strcpy(szPrefix,"ideal");
						break;
					case EGST_FILE:
						strcpy(szPrefix,"real");
						break;
					case EGST_DATABASE:
						strcpy(szPrefix,"realdb");
						break;
					default:
						strcpy(szPrefix,"");
						break;
				}
				sprintf(szFilename,"%sgeometry_MD_RP-%s.txt",szPrefix,m_pGeoReader->GetRPotLabel(*iterRPName));
				m_pGeoReader->StoreReconstructionGeometry(*iterRPName, EFT_FIBERMD, szFilename);
				LogStream<<MSG::INFO<<"The MD fiber geometry was stored in the "<<szFilename<<" file"<<endmsg;
			}
			else{
				LogStream<<MSG::INFO<<"Unknown MD fiber geometry of the RPot "<<m_pGeoReader->GetRPotLabel(*iterRPName)<<" file"<<endmsg;
			}

			//OD fibers
			eGeoType=m_pGeoReader->GetRPGeometryType(*iterRPName,EFT_FIBEROD);
			if(eGeoType!=EGST_UNDEFINED){
				switch(eGeoType){
					case EGST_IDEALGEOMETRY:
						strcpy(szPrefix,"ideal");
						break;
					case EGST_FILE:
						strcpy(szPrefix,"real");
						break;
					case EGST_DATABASE:
						strcpy(szPrefix,"realdb");
						break;
					default:
						strcpy(szPrefix,"");
						break;
				}
				sprintf(szFilename,"%sgeometry_OD_RP-%s.txt",szPrefix,m_pGeoReader->GetRPotLabel(*iterRPName));
				m_pGeoReader->StoreReconstructionGeometry(*iterRPName, EFT_FIBEROD, szFilename);
				LogStream<<MSG::INFO<<"The OD fiber geometry was stored in the "<<szFilename<<" file"<<endmsg;
			}
			else{
				LogStream<<MSG::INFO<<"Unknown OD fiber geometry of the RPot "<<m_pGeoReader->GetRPotLabel(*iterRPName)<<" file"<<endmsg;
			}
		}

		//save RP geometry params
		sprintf(szFilename,"geometryinfo_MD_RP-%s.txt",m_pGeoReader->GetRPotLabel(*iterRPName));
		m_pGeoReader->SaveRPGeometryParams(*iterRPName, szFilename);
		LogStream<<MSG::INFO<<"RP geometry info was stored in the "<<szFilename<<" file"<<endmsg;
	}
}

void ALFA_DetectorFactory::DefineMaterials()
{
	string MatName;
	GeoMaterial* pMaterial;

	//--------- Material definition ---------

	double aH,aB,aC,aN,aO,aNa,aMg,aAl,aSi,aP,aS,aK,aCa,aTi,aCr,aMn,aFe,aNi,aMo,Atot;

	const GeoElement* H  = m_pMaterialManager->getElement("Hydrogen");
	const GeoElement* B  = m_pMaterialManager->getElement("Boron");
	const GeoElement* C  = m_pMaterialManager->getElement("Carbon");
	const GeoElement* N  = m_pMaterialManager->getElement("Nitrogen");
	const GeoElement* O  = m_pMaterialManager->getElement("Oxygen");
	const GeoElement* Na = m_pMaterialManager->getElement("Sodium");
	const GeoElement* Mg = m_pMaterialManager->getElement("Magnesium");
	const GeoElement* Al = m_pMaterialManager->getElement("Aluminium");
	const GeoElement* Si = m_pMaterialManager->getElement("Silicon");
	const GeoElement* P  = m_pMaterialManager->getElement("Phosphorus");
	const GeoElement* S  = m_pMaterialManager->getElement("Sulfur");
	const GeoElement* K  = m_pMaterialManager->getElement("Potassium");
	const GeoElement* Ca = m_pMaterialManager->getElement("Calcium");
	const GeoElement* Ti = m_pMaterialManager->getElement("Titanium");
	const GeoElement* Cr = m_pMaterialManager->getElement("Chromium");
	const GeoElement* Mn = m_pMaterialManager->getElement("Manganese");
	const GeoElement* Fe = m_pMaterialManager->getElement("Iron");
	const GeoElement* Ni = m_pMaterialManager->getElement("Nickel");
	const GeoElement* Mo = m_pMaterialManager->getElement("Molybdenum");
	
	// Steel Grade 316L (Roman Pot)
	MatName="Steel";
	pMaterial=new GeoMaterial(MatName, 8*CLHEP::g/CLHEP::cm3);

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

	pMaterial->add(const_cast<GeoElement*> (Fe),aFe/Atot);
	pMaterial->add(const_cast<GeoElement*> (C),  aC/Atot);
	pMaterial->add(const_cast<GeoElement*> (Mn),aMn/Atot);
	pMaterial->add(const_cast<GeoElement*> (Si),aSi/Atot);
	pMaterial->add(const_cast<GeoElement*> (P),  aP/Atot);
	pMaterial->add(const_cast<GeoElement*> (S),  aS/Atot);
	pMaterial->add(const_cast<GeoElement*> (Cr),aCr/Atot);
	pMaterial->add(const_cast<GeoElement*> (Mo),aMo/Atot);
	pMaterial->add(const_cast<GeoElement*> (Ni),aNi/Atot);
	pMaterial->add(const_cast<GeoElement*> (N),  aN/Atot);
	pMaterial->lock();
	m_MapMaterials.insert(pair<string,GeoMaterial*>(MatName,pMaterial));

	// Titanium 100% (Plates)
	MatName="Titanium";
	pMaterial=new GeoMaterial(MatName, 4.506*CLHEP::g/CLHEP::cm3);

	aTi=Ti->getA()/(CLHEP::g/CLHEP::mole);
	pMaterial->add(const_cast<GeoElement*> (Ti), 1);
	pMaterial->lock();
	m_MapMaterials.insert(pair<string,GeoMaterial*>(MatName,pMaterial));

	// PVT - Polyvinyltoluene - BC408 (Trigger)
	MatName="PVT";
	pMaterial=new GeoMaterial(MatName, 1.032*CLHEP::g/CLHEP::cm3);

	aH=52.5*H->getA()/(CLHEP::g/CLHEP::mole);
	aC=47.5*C->getA()/(CLHEP::g/CLHEP::mole);
	Atot=aH+aC;
	
	pMaterial->add(const_cast<GeoElement*> (H), aH/Atot);
	pMaterial->add(const_cast<GeoElement*> (C), aC/Atot);
	pMaterial->lock();
	m_MapMaterials.insert(pair<string,GeoMaterial*>(MatName,pMaterial));

	// Isola DE156 - G10 material
	// resin matrix:  22% C, 12% O, 3% H, 3.5% Al, 1% P, 0.5% N
	// reinforcement; 2% B, 8% Ca, 4% Al, 15% Si, 1% Mg, <1% Na/K, <0.5% Ti, <0.2% Fe, 27% O 
	MatName="G10";
	pMaterial=new GeoMaterial(MatName, 1.8*CLHEP::g/CLHEP::cm3);

	aH =.03* H ->getA()/(CLHEP::g/CLHEP::mole);
	aB =.02* B ->getA()/(CLHEP::g/CLHEP::mole);
	aC =.22* C ->getA()/(CLHEP::g/CLHEP::mole);
	aN =.005*N ->getA()/(CLHEP::g/CLHEP::mole);
	aO =.39* O ->getA()/(CLHEP::g/CLHEP::mole);
	aNa=.002*Na->getA()/(CLHEP::g/CLHEP::mole);
	aMg=.01* Mg->getA()/(CLHEP::g/CLHEP::mole);
	aAl=.075*Al->getA()/(CLHEP::g/CLHEP::mole);
	aSi=.15* Si->getA()/(CLHEP::g/CLHEP::mole);
	aP =.01* P ->getA()/(CLHEP::g/CLHEP::mole);
	aK =.002*K ->getA()/(CLHEP::g/CLHEP::mole);
	aCa=.08* Ca->getA()/(CLHEP::g/CLHEP::mole);
	aTi=.004*Ti->getA()/(CLHEP::g/CLHEP::mole);
	aFe=.002*Fe->getA()/(CLHEP::g/CLHEP::mole);
	Atot=aH+aB+aC+aN+aO+aNa+aMg+aAl+aSi+aP+aK+aCa+aTi+aFe;
	 
	pMaterial->add(const_cast<GeoElement*> (H), aH/Atot);
	pMaterial->add(const_cast<GeoElement*> (B), aB/Atot);
	pMaterial->add(const_cast<GeoElement*> (C), aC/Atot);
	pMaterial->add(const_cast<GeoElement*> (N), aN/Atot);
	pMaterial->add(const_cast<GeoElement*> (O), aO/Atot);
	pMaterial->add(const_cast<GeoElement*> (Na),aNa/Atot);
	pMaterial->add(const_cast<GeoElement*> (Mg),aMg/Atot);
	pMaterial->add(const_cast<GeoElement*> (Al),aAl/Atot);
	pMaterial->add(const_cast<GeoElement*> (Si),aSi/Atot);
	pMaterial->add(const_cast<GeoElement*> (P), aP/Atot);
	pMaterial->add(const_cast<GeoElement*> (K), aK/Atot);
	pMaterial->add(const_cast<GeoElement*> (Ca),aCa/Atot);
	pMaterial->add(const_cast<GeoElement*> (Ti),aTi/Atot);
	pMaterial->add(const_cast<GeoElement*> (Fe),aFe/Atot);
	pMaterial->lock();
	m_MapMaterials.insert(pair<string,GeoMaterial*>(MatName,pMaterial));

	// Polystyrene (Fiber Core) - (C8H9)n 
	MatName="Polystyrene";
	pMaterial=new GeoMaterial(MatName,  1.05*CLHEP::g/CLHEP::cm3);

	aH=9*H->getA()/(CLHEP::g/CLHEP::mole);
	aC=8*C->getA()/(CLHEP::g/CLHEP::mole);
	Atot=aH+aC;
	
	pMaterial->add(const_cast<GeoElement*> (H), aH/Atot);
	pMaterial->add(const_cast<GeoElement*> (C), aC/Atot);
	pMaterial->lock();
	m_MapMaterials.insert(pair<string,GeoMaterial*>(MatName,pMaterial));

	// PMMA (Cladding) - (C5O2H8)n
	MatName="PMMA";
	pMaterial=new GeoMaterial(MatName, 1.19*CLHEP::g/CLHEP::cm3);

	aH=8*H->getA()/(CLHEP::g/CLHEP::mole);
	aC=5*C->getA()/(CLHEP::g/CLHEP::mole);
	aO=2*O->getA()/(CLHEP::g/CLHEP::mole);
	Atot=aC+aH+aO;
	  
	pMaterial->add(const_cast<GeoElement*> (H), aH/Atot);
	pMaterial->add(const_cast<GeoElement*> (C), aC/Atot);
	pMaterial->add(const_cast<GeoElement*> (O), aO/Atot);
	pMaterial->lock();
	m_MapMaterials.insert(pair<string,GeoMaterial*>(MatName,pMaterial));

	// std::AIR
	MatName="std::Air";
	m_MapMaterials.insert(pair<string,const GeoMaterial*>(MatName,m_pMaterialManager->getMaterial(MatName)));

	// std::Vacuum
	MatName="std::Vacuum";
	m_MapMaterials.insert(pair<string,const GeoMaterial*>(MatName,m_pMaterialManager->getMaterial(MatName)));

}

void ALFA_DetectorFactory::CreateAxes(GeoPhysVol* pMotherVolume)
{
	GeoBox* pSolBoxX = new GeoBox(500.0*CLHEP::mm,0.5*CLHEP::mm,0.5*CLHEP::mm);
	GeoBox* pSolBoxY = new GeoBox(0.5*CLHEP::mm,1000.0*CLHEP::mm,0.5*CLHEP::mm);
	GeoBox* pSolBoxZ = new GeoBox(0.5*CLHEP::mm,0.5*CLHEP::mm,250.0*CLHEP::mm);
	
	GeoLogVol* pLogBoxX=new GeoLogVol("BoxX",pSolBoxX,m_MapMaterials[string("PMMA")]);
	GeoLogVol* pLogBoxY=new GeoLogVol("BoxY",pSolBoxY,m_MapMaterials[string("PMMA")]);
	GeoLogVol* pLogBoxZ=new GeoLogVol("BoxZ",pSolBoxZ,m_MapMaterials[string("PMMA")]);
	
	GeoFullPhysVol* pPhysBoxX=new GeoFullPhysVol(pLogBoxX);
	GeoFullPhysVol* pPhysBoxY=new GeoFullPhysVol(pLogBoxY);
	GeoFullPhysVol* pPhysBoxZ=new GeoFullPhysVol(pLogBoxZ);
	
	pMotherVolume->add(new GeoNameTag("BoxX"));
	pMotherVolume->add(new GeoTransform(HepGeom::Translate3D(0, 0, 0)));
	pMotherVolume->add(pPhysBoxX);
	
	pMotherVolume->add(new GeoNameTag("BoxY"));
	pMotherVolume->add(new GeoTransform(HepGeom::Translate3D(0, 0, 0)));
	pMotherVolume->add(pPhysBoxY);
	
	pMotherVolume->add(new GeoNameTag("BoxZ"));
	pMotherVolume->add(new GeoTransform(HepGeom::Translate3D(0, 0, 0)));
	pMotherVolume->add(pPhysBoxZ);
	
	//helper box
	/*
	GeoBox* pSolBox = new GeoBox(5.0*CLHEP::mm,5.0*CLHEP::mm,5.0*CLHEP::mm);
	GeoLogVol* pLogBox=new GeoLogVol("Box",pSolBox,MapMaterials[string("PMMA")]);
	GeoFullPhysVol* pPhysBox=new GeoFullPhysVol(pLogBox);
	pMotherVolume->add(new GeoNameTag("Box"));
	pMotherVolume->add(new GeoTransform(HepGeom::Translate3D(50.0*CLHEP::mm, 0, 0)));
	pMotherVolume->add(pPhysBox);*/
}

void ALFA_DetectorFactory::AddBeamPipeInStation(GeoFullPhysVol* pPhysStation, const char* pszStationLabel)
{
	char szLabel[32];
	HepGeom::Transform3D TransTube;

	double fzs=2*ALFASTATIONHSIZEZ;
	double fzd=INNERDETZSPACE;
	double fZLength=0.5*(fzs-fzd);
	double fZPos=0.5*fzd+0.5*fZLength;

	sprintf(szLabel,"%s_LogIBP",pszStationLabel);
	GeoTube* pTube = new GeoTube(BEAMPIPEINNERRADIUS,BEAMPIPEOUTERRADIUS,0.5*fZLength);
	GeoLogVol* pLogTube=new GeoLogVol(szLabel,pTube,m_MapMaterials[string("Steel")]);
	GeoFullPhysVol* pPhysTube=new GeoFullPhysVol(pLogTube);

	sprintf(szLabel,"%s_IBP01",pszStationLabel);
	TransTube=HepGeom::TranslateZ3D(fZPos);
	pPhysStation->add(new GeoNameTag(szLabel));
	pPhysStation->add(new GeoTransform(TransTube));
	pPhysStation->add(pPhysTube);

	sprintf(szLabel,"%s_IBP02",pszStationLabel);
	TransTube=HepGeom::TranslateZ3D(-fZPos);
	pPhysStation->add(new GeoNameTag(szLabel));
	pPhysStation->add(new GeoTransform(TransTube));
	pPhysStation->add(pPhysTube);
}

void ALFA_DetectorFactory::AddGlobalVacuumSensorInStation(GeoFullPhysVol *pPhysStation, eAStationName eStatName)
{
	char szLabel[32];
	double fZOffset=0.0*CLHEP::mm;

	if(eStatName==EASN_B7L1 || eStatName==EASN_A7L1) fZOffset=-ALFASTATIONHSIZEZ+0.5*ALFA_GVSTHICKNESS+100.0*CLHEP::mm;
	else if(eStatName==EASN_A7R1 || eStatName==EASN_B7R1) fZOffset=+ALFASTATIONHSIZEZ-0.5*ALFA_GVSTHICKNESS-100.0*CLHEP::mm;

	sprintf(szLabel,"%s_GVS",m_pGeoReader->GetAStationLabel(eStatName));

	GeoBox* pSolGVS=new GeoBox(0.5*200.0*CLHEP::mm,0.5*200.0*CLHEP::mm,0.5*ALFA_GVSTHICKNESS);
	GeoLogVol* pLogGVS=new GeoLogVol(szLabel,pSolGVS,m_MapMaterials[string("std::Vacuum")]);
	GeoFullPhysVol* pPhysGVS=new GeoFullPhysVol(pLogGVS);

	HepGeom::Transform3D TransGVS=HepGeom::TranslateZ3D(fZOffset);
	pPhysStation->add(new GeoNameTag(szLabel));
	pPhysStation->add(new GeoTransform(TransGVS));
	pPhysStation->add(pPhysGVS);
}

void ALFA_DetectorFactory::ConstructAlfaStations(map<eAStationName,ALFAPHYSVOLUME>* pmapActiveStations, GeoPhysVol* pWorld)
{
	char szLabel[32];
	eAStationName eAStation=EASN_UNDEFINED;
	ASPOSPARAMS AStationParams;
	ALFAPHYSVOLUME APhysVolume;

	MsgStream LogStream(Athena::getMessageSvc(), "ConstructAlfaStations");
	
	GeoFullPhysVol* pPhysAlfaBox=NULL;
	//HepGeom::Transform3D TransAlfaBox;
	GeoBox* pSolidAlfaBox=new GeoBox(500.0*CLHEP::mm,1000.0*CLHEP::mm,ALFASTATIONHSIZEZ);
	GeoLogVol* pLogAlfaBox=new GeoLogVol("ALFA",pSolidAlfaBox,m_MapMaterials[string("std::Vacuum")]);

	//B7L1
	eAStation=EASN_B7L1;
	m_pGeoReader->GetASPosParams(&AStationParams,eAStation);
	sprintf(szLabel,"ALFA_%s",AStationParams.szLabel);
	LogStream<<MSG::INFO<<"Create ALFA station "<<szLabel<<endmsg;
	pPhysAlfaBox=new GeoFullPhysVol(pLogAlfaBox);
	//TransAlfaBox=HepGeom::Translate3D(AStationParams.IdealMainPoint[0], AStationParams.IdealMainPoint[1], AStationParams.IdealMainPoint[2]);
	pWorld->add(new GeoNameTag(szLabel));
	pWorld->add(new GeoTransform(AStationParams.ASTransformInATLAS));
	pWorld->add(pPhysAlfaBox);
	m_pDetectorManager->addTreeTop(pPhysAlfaBox);
	if(m_Config.bAddIBP) AddBeamPipeInStation(pPhysAlfaBox,AStationParams.szLabel);
	AddGlobalVacuumSensorInStation(pPhysAlfaBox,eAStation);

	APhysVolume.pPhysVolume=pPhysAlfaBox;
	APhysVolume.Transform=AStationParams.ASTransformInATLAS;
	pmapActiveStations->insert(pair<eAStationName,ALFAPHYSVOLUME>(eAStation,APhysVolume));

	//A7L1
	eAStation=EASN_A7L1;
	m_pGeoReader->GetASPosParams(&AStationParams,eAStation);
	sprintf(szLabel,"ALFA_%s",AStationParams.szLabel);
	LogStream<<MSG::INFO<<"Create ALFA station "<<szLabel<<endmsg;
	pPhysAlfaBox=new GeoFullPhysVol(pLogAlfaBox);
	//TransAlfaBox=HepGeom::Translate3D(AStationParams.IdealMainPoint[0], AStationParams.IdealMainPoint[1], AStationParams.IdealMainPoint[2]);
	pWorld->add(new GeoNameTag(szLabel));
	pWorld->add(new GeoTransform(AStationParams.ASTransformInATLAS));
	pWorld->add(pPhysAlfaBox);
	m_pDetectorManager->addTreeTop(pPhysAlfaBox);
	if(m_Config.bAddIBP) AddBeamPipeInStation(pPhysAlfaBox,AStationParams.szLabel);
	AddGlobalVacuumSensorInStation(pPhysAlfaBox,eAStation);
	
	APhysVolume.pPhysVolume=pPhysAlfaBox;
	APhysVolume.Transform=AStationParams.ASTransformInATLAS;
	pmapActiveStations->insert(pair<eAStationName,ALFAPHYSVOLUME>(eAStation,APhysVolume));

	//A7R1
	eAStation=EASN_A7R1;
	m_pGeoReader->GetASPosParams(&AStationParams,eAStation);
	sprintf(szLabel,"ALFA_%s",AStationParams.szLabel);
	LogStream<<MSG::INFO<<"Create ALFA station "<<szLabel<<endmsg;
	pPhysAlfaBox=new GeoFullPhysVol(pLogAlfaBox);
	//TransAlfaBox=HepGeom::Translate3D(AStationParams.IdealMainPoint[0], AStationParams.IdealMainPoint[1], AStationParams.IdealMainPoint[2]);
	pWorld->add(new GeoNameTag(szLabel));
	pWorld->add(new GeoTransform(AStationParams.ASTransformInATLAS));
	pWorld->add(pPhysAlfaBox);
	m_pDetectorManager->addTreeTop(pPhysAlfaBox);
	if(m_Config.bAddIBP) AddBeamPipeInStation(pPhysAlfaBox,AStationParams.szLabel);
	AddGlobalVacuumSensorInStation(pPhysAlfaBox,eAStation);
	
	APhysVolume.pPhysVolume=pPhysAlfaBox;
	APhysVolume.Transform=AStationParams.ASTransformInATLAS;
	pmapActiveStations->insert(pair<eAStationName,ALFAPHYSVOLUME>(eAStation,APhysVolume));

	//B7R1
	eAStation=EASN_B7R1;
	m_pGeoReader->GetASPosParams(&AStationParams,eAStation);
	sprintf(szLabel,"ALFA_%s",AStationParams.szLabel);
	LogStream<<MSG::INFO<<"Create ALFA station "<<szLabel<<endmsg;
	pPhysAlfaBox=new GeoFullPhysVol(pLogAlfaBox);
	//TransAlfaBox=HepGeom::Translate3D(AStationParams.IdealMainPoint[0], AStationParams.IdealMainPoint[1], AStationParams.IdealMainPoint[2]);
	pWorld->add(new GeoNameTag(szLabel));
	pWorld->add(new GeoTransform(AStationParams.ASTransformInATLAS));
	pWorld->add(pPhysAlfaBox);
	m_pDetectorManager->addTreeTop(pPhysAlfaBox);
	if(m_Config.bAddIBP) AddBeamPipeInStation(pPhysAlfaBox,AStationParams.szLabel);
	AddGlobalVacuumSensorInStation(pPhysAlfaBox,eAStation);
	
	APhysVolume.pPhysVolume=pPhysAlfaBox;
	APhysVolume.Transform=AStationParams.ASTransformInATLAS;
	pmapActiveStations->insert(pair<eAStationName,ALFAPHYSVOLUME>(eAStation,APhysVolume));
}

void ALFA_DetectorFactory::ConstructBeampipe(GeoPhysVol* pWorld)
{
	char szLabel[32], szLabel2[32];
	double fInnerTubeHLength;
	GeoTube* pTube, *pTube2;
	GeoLogVol* pLogTube, *pLogTube2;
	GeoFullPhysVol* pPhysTube, *pPhysTube2;
	ASPOSPARAMS AStationParams01, AStationParams02;
	HepGeom::Transform3D TransTube;

	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_DetectorFactory::create");

	strcpy(szLabel,"ALFA_Beampipe01");
	strcpy(szLabel2,"ALFA_Beampipe01Fill");
	m_pGeoReader->GetASPosParams(&AStationParams01,EASN_B7L1);
	pTube = new GeoTube(BEAMPIPEINNERRADIUS,BEAMPIPEOUTERRADIUS,ALFAEDGEBEAMPIPEHLENGTH);
	pTube2=new GeoTube(0*CLHEP::mm,BEAMPIPEINNERRADIUS,ALFAEDGEBEAMPIPEHLENGTH);
	pLogTube=new GeoLogVol(szLabel,pTube,m_MapMaterials[string("Steel")]);
	pLogTube2=new GeoLogVol(szLabel2,pTube2,m_MapMaterials[string("std::Vacuum")]);
	pPhysTube=new GeoFullPhysVol(pLogTube);
	pPhysTube2=new GeoFullPhysVol(pLogTube2);
	//TransTube=HepGeom::Translate3D(0.0*CLHEP::mm, 0.0*CLHEP::mm, AStationParams01.IdealMainPoint[2]+ALFASTATIONHSIZEZ+ALFAEDGEBEAMPIPEHLENGTH);
	TransTube=HepGeom::TranslateZ3D(ALFASTATIONHSIZEZ+ALFAEDGEBEAMPIPEHLENGTH)*AStationParams01.ASTransformInATLAS;
	pWorld->add(new GeoNameTag(szLabel));
	pWorld->add(new GeoTransform(TransTube));
	pWorld->add(pPhysTube);
	m_pDetectorManager->addTreeTop(pPhysTube);
	pWorld->add(new GeoNameTag(szLabel2));
	pWorld->add(new GeoTransform(TransTube));
	pWorld->add(pPhysTube2);
	m_pDetectorManager->addTreeTop(pPhysTube2);
	LogStream<<MSG::INFO<<"ALFA_Beampipe01: hlength="<<ALFAEDGEBEAMPIPEHLENGTH<<", z="<<AStationParams01.IdealMainPoint[2]+ALFASTATIONHSIZEZ+ALFAEDGEBEAMPIPEHLENGTH<<endmsg;

	strcpy(szLabel,"ALFA_Beampipe02");
	strcpy(szLabel2,"ALFA_Beampipe02Fill");
	m_pGeoReader->GetASPosParams(&AStationParams01,EASN_B7L1);
	m_pGeoReader->GetASPosParams(&AStationParams02,EASN_A7L1);
	fInnerTubeHLength=0.5*((AStationParams01.IdealMainPoint[2]-ALFASTATIONHSIZEZ)-(AStationParams02.IdealMainPoint[2]+ALFASTATIONHSIZEZ));
	pTube = new GeoTube(BEAMPIPEINNERRADIUS,BEAMPIPEOUTERRADIUS,fInnerTubeHLength);
	pTube2 = new GeoTube(0*CLHEP::mm,BEAMPIPEINNERRADIUS,fInnerTubeHLength);
	pLogTube=new GeoLogVol(szLabel,pTube,m_MapMaterials[string("Steel")]);
	pLogTube2=new GeoLogVol(szLabel2,pTube2,m_MapMaterials[string("std::Vacuum")]);
	pPhysTube=new GeoFullPhysVol(pLogTube);
	pPhysTube2=new GeoFullPhysVol(pLogTube2);
	//TransTube=HepGeom::Translate3D(0.0*CLHEP::mm, 0.0*CLHEP::mm, AStationParams01.IdealMainPoint[2]-ALFASTATIONHSIZEZ-fInnerTubeHLength);
	TransTube=HepGeom::TranslateZ3D(-ALFASTATIONHSIZEZ-fInnerTubeHLength)*AStationParams01.ASTransformInATLAS;
	pWorld->add(new GeoNameTag(szLabel));
	pWorld->add(new GeoTransform(TransTube));
	pWorld->add(pPhysTube);
	m_pDetectorManager->addTreeTop(pPhysTube);
	pWorld->add(new GeoNameTag(szLabel2));
	pWorld->add(new GeoTransform(TransTube));
	pWorld->add(pPhysTube2);
	m_pDetectorManager->addTreeTop(pPhysTube2);
	LogStream<<MSG::INFO<<"ALFA_Beampipe02: hlength="<<fInnerTubeHLength<<", z="<<AStationParams01.IdealMainPoint[2]-ALFASTATIONHSIZEZ-fInnerTubeHLength<<endmsg;

	strcpy(szLabel,"ALFA_Beampipe03");
	strcpy(szLabel2,"ALFA_Beampipe03Fill");
	m_pGeoReader->GetASPosParams(&AStationParams01,EASN_A7L1);
	pTube = new GeoTube(BEAMPIPEINNERRADIUS,BEAMPIPEOUTERRADIUS,ALFAEDGEBEAMPIPEHLENGTH);
	pTube2 = new GeoTube(0*CLHEP::mm,BEAMPIPEINNERRADIUS,ALFAEDGEBEAMPIPEHLENGTH);
	pLogTube=new GeoLogVol(szLabel,pTube,m_MapMaterials[string("Steel")]);
	pLogTube2=new GeoLogVol(szLabel2,pTube2,m_MapMaterials[string("std::Vacuum")]);
	pPhysTube=new GeoFullPhysVol(pLogTube);
	pPhysTube2=new GeoFullPhysVol(pLogTube2);
	//TransTube=HepGeom::Translate3D(0.0*CLHEP::mm, 0.0*CLHEP::mm, AStationParams01.IdealMainPoint[2]-ALFASTATIONHSIZEZ-ALFAEDGEBEAMPIPEHLENGTH);
	TransTube=HepGeom::TranslateZ3D(-ALFASTATIONHSIZEZ-ALFAEDGEBEAMPIPEHLENGTH)*AStationParams01.ASTransformInATLAS;
	pWorld->add(new GeoNameTag(szLabel));
	pWorld->add(new GeoTransform(TransTube));
	pWorld->add(pPhysTube);
	m_pDetectorManager->addTreeTop(pPhysTube);
	pWorld->add(new GeoNameTag(szLabel2));
	pWorld->add(new GeoTransform(TransTube));
	pWorld->add(pPhysTube2);
	m_pDetectorManager->addTreeTop(pPhysTube2);
	LogStream<<MSG::INFO<<"ALFA_Beampipe03: hlength="<<ALFAEDGEBEAMPIPEHLENGTH<<", z="<<AStationParams01.IdealMainPoint[2]-ALFASTATIONHSIZEZ-ALFAEDGEBEAMPIPEHLENGTH<<endmsg;

	strcpy(szLabel,"ALFA_Beampipe04");
	strcpy(szLabel2,"ALFA_Beampipe04Fill");
	m_pGeoReader->GetASPosParams(&AStationParams01,EASN_A7R1);
	pTube = new GeoTube(BEAMPIPEINNERRADIUS,BEAMPIPEOUTERRADIUS,ALFAEDGEBEAMPIPEHLENGTH);
	pTube2 = new GeoTube(0*CLHEP::mm,BEAMPIPEINNERRADIUS,ALFAEDGEBEAMPIPEHLENGTH);
	pLogTube=new GeoLogVol(szLabel,pTube,m_MapMaterials[string("Steel")]);
	pLogTube2=new GeoLogVol(szLabel2,pTube2,m_MapMaterials[string("std::Vacuum")]);
	pPhysTube=new GeoFullPhysVol(pLogTube);
	pPhysTube2=new GeoFullPhysVol(pLogTube2);
	//TransTube=HepGeom::Translate3D(0.0*CLHEP::mm, 0.0*CLHEP::mm, AStationParams01.IdealMainPoint[2]+ALFASTATIONHSIZEZ+ALFAEDGEBEAMPIPEHLENGTH);
	TransTube=HepGeom::TranslateZ3D(+ALFASTATIONHSIZEZ+ALFAEDGEBEAMPIPEHLENGTH)*AStationParams01.ASTransformInATLAS;
	pWorld->add(new GeoNameTag(szLabel));
	pWorld->add(new GeoTransform(TransTube));
	pWorld->add(pPhysTube);
	m_pDetectorManager->addTreeTop(pPhysTube);
	pWorld->add(new GeoNameTag(szLabel2));
	pWorld->add(new GeoTransform(TransTube));
	pWorld->add(pPhysTube2);
	m_pDetectorManager->addTreeTop(pPhysTube2);
	LogStream<<MSG::INFO<<"ALFA_Beampipe04: hlength="<<ALFAEDGEBEAMPIPEHLENGTH<<", z="<<AStationParams01.IdealMainPoint[2]+ALFASTATIONHSIZEZ+ALFAEDGEBEAMPIPEHLENGTH<<endmsg;

	strcpy(szLabel,"ALFA_Beampipe05");
	strcpy(szLabel2,"ALFA_Beampipe05Fill");
	m_pGeoReader->GetASPosParams(&AStationParams01,EASN_A7R1);
	m_pGeoReader->GetASPosParams(&AStationParams02,EASN_B7R1);
	fInnerTubeHLength=0.5*((AStationParams01.IdealMainPoint[2]-ALFASTATIONHSIZEZ)-(AStationParams02.IdealMainPoint[2]+ALFASTATIONHSIZEZ));
	pTube = new GeoTube(BEAMPIPEINNERRADIUS,BEAMPIPEOUTERRADIUS,fInnerTubeHLength);
	pTube2 = new GeoTube(0*CLHEP::mm,BEAMPIPEINNERRADIUS,fInnerTubeHLength);
	pLogTube=new GeoLogVol(szLabel,pTube,m_MapMaterials[string("Steel")]);
	pLogTube2=new GeoLogVol(szLabel2,pTube2,m_MapMaterials[string("std::Vacuum")]);
	pPhysTube=new GeoFullPhysVol(pLogTube);
	pPhysTube2=new GeoFullPhysVol(pLogTube2);
	//TransTube=HepGeom::Translate3D(0.0*CLHEP::mm, 0.0*CLHEP::mm, AStationParams01.IdealMainPoint[2]-ALFASTATIONHSIZEZ-fInnerTubeHLength);
	TransTube=HepGeom::TranslateZ3D(-ALFASTATIONHSIZEZ-fInnerTubeHLength)*AStationParams01.ASTransformInATLAS;
	pWorld->add(new GeoNameTag(szLabel));
	pWorld->add(new GeoTransform(TransTube));
	pWorld->add(pPhysTube);
	m_pDetectorManager->addTreeTop(pPhysTube);
	pWorld->add(new GeoNameTag(szLabel2));
	pWorld->add(new GeoTransform(TransTube));
	pWorld->add(pPhysTube2);
	m_pDetectorManager->addTreeTop(pPhysTube2);
	LogStream<<MSG::INFO<<"ALFA_Beampipe02: hlength="<<fInnerTubeHLength<<", z="<<AStationParams01.IdealMainPoint[2]-ALFASTATIONHSIZEZ-fInnerTubeHLength<<endmsg;

	strcpy(szLabel,"ALFA_Beampipe06");
	strcpy(szLabel2,"ALFA_Beampipe06Fill");
	m_pGeoReader->GetASPosParams(&AStationParams01,EASN_B7R1);
	pTube = new GeoTube(BEAMPIPEINNERRADIUS,BEAMPIPEOUTERRADIUS,ALFAEDGEBEAMPIPEHLENGTH);
	pTube2 = new GeoTube(0*CLHEP::mm,BEAMPIPEINNERRADIUS,ALFAEDGEBEAMPIPEHLENGTH);
	pLogTube=new GeoLogVol(szLabel,pTube,m_MapMaterials[string("Steel")]);
	pLogTube2=new GeoLogVol(szLabel2,pTube2,m_MapMaterials[string("std::Vacuum")]);
	pPhysTube=new GeoFullPhysVol(pLogTube);
	pPhysTube2=new GeoFullPhysVol(pLogTube2);
	//TransTube=HepGeom::Translate3D(0.0*CLHEP::mm, 0.0*CLHEP::mm, AStationParams01.IdealMainPoint[2]-ALFASTATIONHSIZEZ-ALFAEDGEBEAMPIPEHLENGTH);
	TransTube=HepGeom::TranslateZ3D(-ALFASTATIONHSIZEZ-ALFAEDGEBEAMPIPEHLENGTH)*AStationParams01.ASTransformInATLAS;
	pWorld->add(new GeoNameTag(szLabel));
	pWorld->add(new GeoTransform(TransTube));
	pWorld->add(pPhysTube);
	m_pDetectorManager->addTreeTop(pPhysTube);
	pWorld->add(new GeoNameTag(szLabel2));
	pWorld->add(new GeoTransform(TransTube));
	pWorld->add(pPhysTube2);
	m_pDetectorManager->addTreeTop(pPhysTube2);
	LogStream<<MSG::INFO<<"ALFA_Beampipe06: hlength="<<ALFAEDGEBEAMPIPEHLENGTH<<", z="<<AStationParams01.IdealMainPoint[2]-ALFASTATIONHSIZEZ-ALFAEDGEBEAMPIPEHLENGTH<<endmsg;
}

void ALFA_DetectorFactory::create(GeoPhysVol* pWorld)
{
	int i;
	StatusCode sc;
	eRPotName eRPName;
	list<eRPotName>::const_iterator iterRPName;
	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_DetectorFactory::create");

	LogStream<<MSG::INFO<<"ALFA_DetectorFactory::buildALFA_RP"<<endmsg;
	LogStream<<MSG::DEBUG<<"Build ALFA Roman Pot"<<endmsg;

	// Create ALFA_ Detector Manager
	m_pDetectorManager= new ALFA_DetectorManager();
	// Retrieve material manager
	if (m_pDetectorStore->retrieve(m_pMaterialManager, std::string("MATERIALS"))!=StatusCode::SUCCESS){
		LogStream<<MSG::INFO<<"Could not load material manager"<<endmsg;
		return;
	}
	else{
		//ostream* outmat=new ofstream("materials.txt",ios_base::out);
		//pMaterialManager->printAll(cout);
	}

	//define materials
	DefineMaterials();
	
	//create auxiliary axes
	CreateAxes(pWorld);

	//create geometry reader & inicialize wafer data object
	if(ReadGeometry()==true){
		LogStream<<MSG::INFO<<"Geometry loaded successfully"<<endmsg;
	}
	else{
		LogStream<<MSG::INFO<<"Could not load geometry"<<endmsg;
		return;
	}
	
	//detector construction
	GeoShape* pSolidRP=CreateSolidRP();
	GeoShape* pSolidAir=CreateSolidAir();
	GeoShape* pSolidTrigger=CreateSolidTrigger();
	GeoShape* pSolidRPSupport=CreateSolidRPSupport();
	GeoShape* pSolidG10Shapes=CreateSolidG10Shapes();
	map<int,GeoShape*>* pMapSolidTiPlates=CreateSolidTiPlates();
	map<int,GeoShape*>* pMapSolidOdPlates=CreateSolidODPlates();
	
	char szLabel[32];
	RPPOSPARAMS RPosParams;
	ASPOSPARAMS AStationParams;
	GeoLogVol* pLogRPBox=NULL;
	GeoLogVol* pLogRPAir=NULL;
	GeoFullPhysVol* pPhysAlfaBox=NULL;
	GeoFullPhysVol* pPhysRPBox=NULL;
	GeoFullPhysVol* pPhysRPAir=NULL;
	HepGeom::Translate3D ObjTranslate;
	HepGeom::Rotate3D ObjRotate;
	
	GeoLogVol* pLogObj=NULL;
	GeoFullPhysVol* pPhysObj=NULL;
	HepGeom::Transform3D TransAlfaBox,TransRPAir,TransTot;
	HepGeom::Transform3D TransAlfaBoxRotate;
	HepGeom::Transform3D TransRPBoxRotate;

	StoredAlignX *pAlignX;
	GeoAlignableTransform* pAlTransRPBox;
	GeoAlignableTransform* pAlTransDetInRPMainPoint;
	StoredPhysVol* pStPhysRPBox=NULL;

	//Construct ALFA stations & beampipe
	map<eAStationName,ALFAPHYSVOLUME> mapActiveStations;
	ConstructAlfaStations(&mapActiveStations, pWorld);
	if(m_Config.bConstructBeampipe) ConstructBeampipe(pWorld);
	//LogStream<<MSG::INFO<<"MARK1"<<endmsg;
	
	for(iterRPName=m_ListExistingRPots.begin();iterRPName!=m_ListExistingRPots.end();iterRPName++)
	{	
		eRPName=*iterRPName;
		m_pGeoReader->GetRPPosParams(&RPosParams,eRPName);
		m_pGeoReader->GetASPosParams(&AStationParams,RPosParams.eASName);
		pPhysAlfaBox=mapActiveStations[RPosParams.eASName].pPhysVolume;
		TransAlfaBox=mapActiveStations[RPosParams.eASName].Transform;

		pAlTransRPBox=new GeoAlignableTransform(RPosParams.RPTransformInStation);
		pAlTransDetInRPMainPoint=new GeoAlignableTransform(RPosParams.DetTransformInMainPoint);

		//create Roman pot ----------------------------------------------------------------------------
		sprintf(szLabel,"LogRPBox[%02d]",eRPName);
		pLogRPBox=new GeoLogVol(szLabel,pSolidRP,m_MapMaterials[string("Steel")]);
		pPhysRPBox=new GeoFullPhysVol(pLogRPBox);

		sprintf(szLabel,"RPBox[%02d]",eRPName);
		pPhysAlfaBox->add(new GeoNameTag(szLabel));
		pPhysAlfaBox->add(pAlTransRPBox);
		pPhysAlfaBox->add(pPhysRPBox);
		//LogStream<<MSG::INFO<<"MARK3"<<endmsg;

		//create RP Air
		sprintf(szLabel,"LogRPAir[%02d]",eRPName);
		pLogRPAir=new GeoLogVol(szLabel,pSolidAir,m_MapMaterials[string("std::Vacuum")]);
		pPhysRPAir=new GeoFullPhysVol(pLogRPAir);
		
		sprintf(szLabel,"RPAir[%02d]",eRPName);
		TransRPAir=HepGeom::Translate3D(0.0,0.0,0.0);
		pPhysRPBox->add(new GeoNameTag(szLabel));
		pPhysRPBox->add(new GeoTransform(TransRPAir));
		pPhysRPBox->add(pPhysRPAir);
		//LogStream<<MSG::INFO<<"MARK4"<<endmsg;

		//create Trigger
		sprintf(szLabel,"LogTrigger[%02d]",eRPName);
		pLogObj=new GeoLogVol(szLabel,pSolidTrigger,m_MapMaterials[string("PVT")]);
		pPhysObj=new GeoFullPhysVol(pLogObj);
		sprintf(szLabel,"Trigger[%02d]",eRPName);
		pPhysRPAir->add(new GeoNameTag(szLabel));
		pPhysRPAir->add(pAlTransDetInRPMainPoint);
		pPhysRPAir->add(new GeoTransform(HepGeom::Translate3D(-22.0*CLHEP::mm,-31.325*CLHEP::mm,11.3*CLHEP::mm)));
		pPhysRPAir->add(pPhysObj);
		//LogStream<<MSG::INFO<<"MARK5"<<endmsg;

		//create RP Support
		sprintf(szLabel,"LogRPSupport[%02d]",eRPName);
		pLogObj=new GeoLogVol(szLabel,pSolidRPSupport,m_MapMaterials[string("Steel")]);
		pPhysObj=new GeoFullPhysVol(pLogObj);
		sprintf(szLabel,"RPSupport[%02d]",eRPName);
		pPhysRPAir->add(new GeoNameTag(szLabel));
		pPhysRPAir->add(pAlTransDetInRPMainPoint);
		pPhysRPAir->add(new GeoTransform(HepGeom::Translate3D(0.0*CLHEP::mm,85.475*CLHEP::mm,-28.5*CLHEP::mm)));
		pPhysRPAir->add(pPhysObj);
		//LogStream<<MSG::INFO<<"MARK6"<<endmsg;

		//create G10 Shapes
		sprintf(szLabel,"LogG10Substrate[%02d]",eRPName);
		pLogObj=new GeoLogVol(szLabel,pSolidG10Shapes,m_MapMaterials[string("PMMA")]);
		for(i=0;i<ALFAPLATESCNT;i++){
			pPhysObj=new GeoFullPhysVol(pLogObj);
			sprintf(szLabel,"G10Substrate[%02d][%d]",eRPName,i);
			pPhysRPAir->add(new GeoNameTag(szLabel));
			pPhysRPAir->add(pAlTransDetInRPMainPoint);
			pPhysRPAir->add(new GeoTransform(HepGeom::Translate3D(ALFA_stagger[i]*CLHEP::mm,-27.525*CLHEP::mm,(-17.5+i*2)*CLHEP::mm)*HepGeom::RotateX3D(-90.0*CLHEP::deg)));
			pPhysRPAir->add(pPhysObj);
		}

		//create Ti plates
		for(i=0;i<ALFAPLATESCNT;i++){
			sprintf(szLabel,"LogRPSupport[%02d][%d]",eRPName,i);
			pLogObj=new GeoLogVol(szLabel,(*pMapSolidTiPlates)[i],m_MapMaterials[string("Titanium")]);
			pPhysObj=new GeoFullPhysVol(pLogObj);
			sprintf(szLabel,"RPSupport[%02d][%d]",eRPName,i);
			pPhysRPAir->add(new GeoNameTag(szLabel));
			pPhysRPAir->add(pAlTransDetInRPMainPoint);
			pPhysRPAir->add(new GeoTransform(HepGeom::Translate3D(0.0*CLHEP::mm,5.975*CLHEP::mm,(-17.5+i*2)*CLHEP::mm)));
			pPhysRPAir->add(pPhysObj);
		}
		//LogStream<<MSG::INFO<<"MARK7"<<endmsg;

		//create Claddings & Fibers
		TransTot=TransAlfaBox*RPosParams.RPTransformInStation*TransRPAir*RPosParams.DetTransformInMainPoint;
		
		ConstructUFiberCladdings(eRPName,pPhysRPAir,TransTot, pAlTransDetInRPMainPoint);
		ConstructVFiberCladdings(eRPName,pPhysRPAir,TransTot, pAlTransDetInRPMainPoint);
		//LogStream<<MSG::INFO<<"MARK8"<<endmsg;

		//create OD Plates
		for(i=0;i<ODPLATESCNT;i++){
			sprintf(szLabel,"LogODPlate[%02d][%d]",eRPName,i);
			pLogObj=new GeoLogVol(szLabel,(*pMapSolidOdPlates)[i],m_MapMaterials[string("Titanium")]);
			pPhysObj=new GeoFullPhysVol(pLogObj);
			sprintf(szLabel,"ODPlate[%02d][%d]",eRPName,i);
			pPhysRPAir->add(new GeoNameTag(szLabel));
			pPhysRPAir->add(pAlTransDetInRPMainPoint);
			pPhysRPAir->add(new GeoTransform(HepGeom::Translate3D(0.0*CLHEP::mm,-0.175*CLHEP::mm,(15+i*2)*CLHEP::mm)));
			pPhysRPAir->add(pPhysObj);
		}
		//LogStream<<MSG::INFO<<"MARK9"<<endmsg;
		
		//create OD Claddings & Fibers
		ConstructODFiberCladdings(eRPName,pPhysRPAir,TransTot, pAlTransDetInRPMainPoint);
		//LogStream<<MSG::INFO<<"MARK10"<<endmsg;

		//store alignable transforms -----------------------------------------------------------------
		sprintf(szLabel,"StRPBox[%02d]",eRPName);
		pStPhysRPBox=new StoredPhysVol(pPhysRPBox);
		sc=m_pDetectorStore->record(pStPhysRPBox,szLabel);
		if(!sc.isSuccess()) throw std::runtime_error("Cannot store alignable record");

		sprintf(szLabel,"AlTransRPBox[%02d]",eRPName);
		pAlignX = new StoredAlignX(pAlTransRPBox);
		sc=m_pDetectorStore->record(pAlignX,szLabel);
		if(!sc.isSuccess()) throw std::runtime_error("Cannot store alignable record");

		sprintf(szLabel,"AlTransDetInRPMainPoint[%02d]",eRPName);
		pAlignX = new StoredAlignX(pAlTransDetInRPMainPoint);
		sc=m_pDetectorStore->record(pAlignX,szLabel);
		if(!sc.isSuccess()) throw std::runtime_error("Cannot store alignable record");
	}
	
	LogStream<<MSG::INFO<<"ALFA detector successfully created."<<endmsg;

	if(m_eRequestedMetrologyType==EMT_NOMINAL || m_eRequestedMetrologyType==EMT_METROLOGY)
	{
		SaveGeometry();
	}

	if(pMapSolidOdPlates) delete pMapSolidOdPlates;
	if(pMapSolidTiPlates) delete pMapSolidTiPlates;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Create Solids

GeoShape* ALFA_DetectorFactory::CreateSolidRP()
{
	GeoBox* RPbox1 = new GeoBox(64*CLHEP::mm,29.825*CLHEP::mm,23*CLHEP::mm);
	GeoBox* RPbox2 = new GeoBox(21*CLHEP::mm,5.425*CLHEP::mm,7.5*CLHEP::mm);
	GeoBox* RPbox3 = new GeoBox(22*CLHEP::mm,1*CLHEP::mm,8.5*CLHEP::mm);

	HepGeom::Transform3D RP_Move1= HepGeom::Translate3D(0, 4.425*CLHEP::mm, 0);
	GeoShapeShift * mowe1 = new GeoShapeShift(RPbox3,RP_Move1);
	GeoShapeUnion * RPsemi1 = new GeoShapeUnion(RPbox2, mowe1);

	HepGeom::Transform3D RP_Move2= HepGeom::Translate3D( 38.5*CLHEP::mm, -35.25*CLHEP::mm, 14.5*CLHEP::mm);
	GeoShapeShift * mowe2 = new GeoShapeShift(RPsemi1,RP_Move2);
	GeoShapeUnion * RPsemi2 = new GeoShapeUnion(RPbox1, mowe2);

	HepGeom::Transform3D RP_Move3= HepGeom::Translate3D(-38.5*CLHEP::mm, -35.25*CLHEP::mm, 14.5*CLHEP::mm);
	GeoShapeShift * mowe3 = new GeoShapeShift(RPsemi1,RP_Move3);
	GeoShapeUnion * RPsemi3 = new GeoShapeUnion(RPsemi2, mowe3);

	GeoTube* RPtube1 = new GeoTube(0*CLHEP::mm,74*CLHEP::mm,26.675*CLHEP::mm);
	GeoTube* RPtube2 = new GeoTube(0*CLHEP::mm,126.5*CLHEP::mm,9.875*CLHEP::mm);

	HepGeom::Transform3D RP_Move4= HepGeom::Translate3D(0*CLHEP::mm, 0*CLHEP::mm, -36.55*CLHEP::mm);
	GeoShapeShift * mowe4 = new GeoShapeShift(RPtube2,RP_Move4);
	GeoShapeUnion * RPsemi4 = new GeoShapeUnion(RPtube1, mowe4);

	GeoBox* RPbox4 = new GeoBox(104*CLHEP::mm,104*CLHEP::mm,9.875*CLHEP::mm);
	GeoBox* RPbox5 = new GeoBox(131.75*CLHEP::mm,131.75*CLHEP::mm,22.5*CLHEP::mm);

	HepGeom::Transform3D RP_Move5= HepGeom::Translate3D(0*CLHEP::mm, 0*CLHEP::mm, -56.3*CLHEP::mm);
	GeoShapeShift * mowe5 = new GeoShapeShift(RPbox4,RP_Move5);
	GeoShapeUnion * RPsemi5 = new GeoShapeUnion(RPsemi4, mowe5);

	HepGeom::Transform3D RP_Move6= HepGeom::Translate3D(0*CLHEP::mm, 0*CLHEP::mm, -88.675*CLHEP::mm);
	GeoShapeShift * mowe6 = new GeoShapeShift(RPbox5,RP_Move6);
	GeoShapeUnion * RPsemi6 = new GeoShapeUnion(RPsemi5, mowe6);

	HepGeom::Transform3D RP_MoveRot7= HepGeom::Translate3D(0*CLHEP::mm, 56.5*CLHEP::mm, -8.5*CLHEP::mm)*HepGeom::RotateX3D(90*CLHEP::deg);
	GeoShapeShift * mowe7 = new GeoShapeShift(RPsemi6,RP_MoveRot7);
	GeoShapeUnion * RPsemi7 = new GeoShapeUnion(RPsemi3, mowe7);

	GeoBox* RPbox6 = new GeoBox(16.97*CLHEP::mm,16.97*CLHEP::mm,0.75*CLHEP::mm);

	HepGeom::Transform3D RP_MoveRot8= HepGeom::Translate3D(0*CLHEP::mm, -21.525*CLHEP::mm, 22.25*CLHEP::mm)*HepGeom::RotateZ3D(45*CLHEP::deg);
	GeoShapeShift * mowe8 = new GeoShapeShift(RPbox6,RP_MoveRot8);
	GeoShapeSubtraction * RPsemi8 = new GeoShapeSubtraction(RPsemi7, mowe8);

	HepGeom::Transform3D RP_MoveRot9= HepGeom::Translate3D(0*CLHEP::mm, -21.525*CLHEP::mm, -22.25*CLHEP::mm)*HepGeom::RotateZ3D(45*CLHEP::deg);
	GeoShapeShift * mowe9 = new GeoShapeShift(RPbox6,RP_MoveRot9);
	GeoShapeSubtraction * shapeRP = new GeoShapeSubtraction(RPsemi8, mowe9);
		
	return shapeRP;
}

GeoShape* ALFA_DetectorFactory::CreateSolidAir()
{
	GeoBox* RP_Airbox1 = new GeoBox(62*CLHEP::mm,27.825*CLHEP::mm,21*CLHEP::mm);
	GeoBox* RP_Airbox2 = new GeoBox(62*CLHEP::mm,3.5*CLHEP::mm,21*CLHEP::mm);
	GeoBox* RP_Airbox3 = new GeoBox(16.5*CLHEP::mm,0.925*CLHEP::mm,21*CLHEP::mm);
	GeoBox* RP_Airbox4 = new GeoBox(20*CLHEP::mm,5.675*CLHEP::mm,6.5*CLHEP::mm);
	GeoTube* RP_Airtube1 = new GeoTube(0*CLHEP::mm,69*CLHEP::mm,65.425*CLHEP::mm);

	HepGeom::Transform3D RP_Air_Move1= HepGeom::Translate3D(0,31.325*CLHEP::mm, 0);
	GeoShapeShift * mowe1 = new GeoShapeShift(RP_Airbox2,RP_Air_Move1);
	GeoShapeUnion * RP_Airsemi1 = new GeoShapeUnion(RP_Airbox1, mowe1);

	HepGeom::Transform3D RP_Air_Move2= HepGeom::Translate3D(0,-28.75*CLHEP::mm, 0);
	GeoShapeShift * mowe2 = new GeoShapeShift(RP_Airbox3,RP_Air_Move2);
	GeoShapeUnion * RP_Airsemi2 = new GeoShapeUnion(RP_Airsemi1, mowe2);

	HepGeom::Transform3D RP_Air_Move3= HepGeom::Translate3D( 38.5*CLHEP::mm,-33.5*CLHEP::mm,14.5*CLHEP::mm);
	GeoShapeShift * mowe3 = new GeoShapeShift(RP_Airbox4,RP_Air_Move3);
	GeoShapeUnion * RP_Airsemi3 = new GeoShapeUnion(RP_Airsemi2, mowe3);

	HepGeom::Transform3D RP_Air_Move4= HepGeom::Translate3D(-38.5*CLHEP::mm,-33.5*CLHEP::mm,14.5*CLHEP::mm);
	GeoShapeShift * mowe4 = new GeoShapeShift(RP_Airbox4,RP_Air_Move4);
	GeoShapeUnion * RP_Airsemi4 = new GeoShapeUnion(RP_Airsemi3, mowe4);

	HepGeom::Transform3D RP_Air_MoveRot5= HepGeom::Translate3D(0*CLHEP::mm,100.25*CLHEP::mm, -8.5*CLHEP::mm)*HepGeom::RotateX3D(90*CLHEP::deg);
	GeoShapeShift * mowe5 = new GeoShapeShift(RP_Airtube1,RP_Air_MoveRot5);
	GeoShapeUnion * shapeRP_Air = new GeoShapeUnion(RP_Airsemi4, mowe5);
		
	return shapeRP_Air;
}

GeoShape* ALFA_DetectorFactory::CreateSolidTrigger()
{
	GeoBox* ALFA_Trigbox1 = new GeoBox(15.75*CLHEP::mm,15.75*CLHEP::mm,1.5*CLHEP::mm);
	GeoBox* ALFA_Trigbox2 = new GeoBox(15.75*CLHEP::mm,15.75*CLHEP::mm,3*CLHEP::mm);
	GeoBox* ALFA_Trigbox3 = new GeoBox(3*CLHEP::mm,7.5*CLHEP::mm,1.5*CLHEP::mm);

	HepGeom::Transform3D ALFA_Trig_MoveRot1= HepGeom::Translate3D(17.235*CLHEP::mm, -17.235*CLHEP::mm, 0*CLHEP::mm)*HepGeom::RotateZ3D(45*CLHEP::deg);
	GeoShapeShift * mowe1 = new GeoShapeShift(ALFA_Trigbox2, ALFA_Trig_MoveRot1);
	GeoShapeSubtraction * ALFA_Trigsemi1 = new GeoShapeSubtraction(ALFA_Trigbox1, mowe1);

	HepGeom::Transform3D ALFA_Trig_Move2= HepGeom::Translate3D(0*CLHEP::mm,0*CLHEP::mm, 3.5*CLHEP::mm);
	GeoShapeShift * mowe2 = new GeoShapeShift(ALFA_Trigsemi1,ALFA_Trig_Move2);
	GeoShapeUnion * ALFA_Trigsemi2 = new GeoShapeUnion(ALFA_Trigsemi1, mowe2);

	HepGeom::Transform3D ALFA_Trig_Move3= HepGeom::Translate3D(44*CLHEP::mm, 0*CLHEP::mm, 0*CLHEP::mm);
	GeoShapeShift * mowe3 = new GeoShapeShift(ALFA_Trigbox3,ALFA_Trig_Move3);
	GeoShapeUnion * ALFA_Trigsemi3 = new GeoShapeUnion(ALFA_Trigbox3, mowe3);

	HepGeom::Transform3D ALFA_Trig_MoveRot4= HepGeom::Translate3D(22*CLHEP::mm, 10.424*CLHEP::mm, -7.6*CLHEP::mm)*HepGeom::RotateZ3D(-45*CLHEP::deg);
	GeoShapeShift * mowe4 = new GeoShapeShift(ALFA_Trigsemi2,ALFA_Trig_MoveRot4);
	GeoShapeUnion * shapeALFA_Trigger = new GeoShapeUnion(ALFA_Trigsemi3, mowe4);
	
	return shapeALFA_Trigger;
}

GeoShape* ALFA_DetectorFactory::CreateSolidRPSupport()
{
	GeoBox* RP_Supbox1 = new GeoBox(10*CLHEP::mm,33*CLHEP::mm,10*CLHEP::mm);
	GeoBox* RP_Supbox2 = new GeoBox( 6*CLHEP::mm, 9*CLHEP::mm,15*CLHEP::mm);
	GeoBox* RP_Supbox3 = new GeoBox(18*CLHEP::mm, 5*CLHEP::mm,18*CLHEP::mm);
	GeoBox* RP_Supbox4 = new GeoBox(18*CLHEP::mm,3.5*CLHEP::mm,18*CLHEP::mm);

	HepGeom::Transform3D RP_Sup_Move1= HepGeom::Translate3D(0, 42*CLHEP::mm, 8*CLHEP::mm);
	GeoShapeShift * mowe1 = new GeoShapeShift(RP_Supbox2, RP_Sup_Move1);
	GeoShapeUnion * RP_Supsemi1 = new GeoShapeUnion(RP_Supbox1, mowe1);

	HepGeom::Transform3D RP_Sup_Move2= HepGeom::Translate3D(0, 56*CLHEP::mm, 20*CLHEP::mm);
	GeoShapeShift * mowe2 = new GeoShapeShift(RP_Supbox3, RP_Sup_Move2);
	GeoShapeUnion * RP_Supsemi2 = new GeoShapeUnion(RP_Supsemi1, mowe2);

	HepGeom::Transform3D RP_Sup_Move3= HepGeom::Translate3D(0, 64.5*CLHEP::mm, 20*CLHEP::mm);
	GeoShapeShift * mowe3 = new GeoShapeShift(RP_Supbox4, RP_Sup_Move3);
	GeoShapeUnion * shapeRP_Support = new GeoShapeUnion(RP_Supsemi2, mowe3);
	
	return shapeRP_Support;
}

GeoShape* ALFA_DetectorFactory::CreateSolidG10Shapes()
{
	GeoTrd* G10trd1 = new GeoTrd( 15*CLHEP::mm, 17*CLHEP::mm, 0.15*CLHEP::mm, 0.15*CLHEP::mm, 1*CLHEP::mm);
	GeoTrd* G10trd2 = new GeoTrd( 30*CLHEP::mm, 9.5*CLHEP::mm, 0.15*CLHEP::mm, 0.15*CLHEP::mm, 10.25*CLHEP::mm);
	GeoBox* G10box1 = new GeoBox( 30*CLHEP::mm, 0.15*CLHEP::mm, 3.75*CLHEP::mm);

	HepGeom::Transform3D G10_Move1= HepGeom::Translate3D(0, 0, 4.75*CLHEP::mm);
	GeoShapeShift* mowe1 = new GeoShapeShift(G10box1, G10_Move1);
	GeoShapeUnion* G10_Subsemi1 = new GeoShapeUnion(G10trd1, mowe1);

	HepGeom::Transform3D G10_Move2= HepGeom::Translate3D(0, 0, 18.75*CLHEP::mm);
	GeoShapeShift* mowe2 = new GeoShapeShift(G10trd2, G10_Move2);
	GeoShapeUnion* shapeG10_Substrate = new GeoShapeUnion(G10_Subsemi1, mowe2);
	
	return shapeG10_Substrate;
}

map<int,GeoShape*>* ALFA_DetectorFactory::CreateSolidTiPlates()
{
	int i;
	std::map<int,GeoShape*>* pMapTiPlates=new std::map<int,GeoShape*>();

	GeoBox* TiPlate_Holder = new GeoBox(10*CLHEP::mm,36.5*CLHEP::mm,1*CLHEP::mm);
	GeoBox* TiPlate_Substr = new GeoBox(50*CLHEP::mm,32.5*CLHEP::mm,1*CLHEP::mm);
	GeoBox* TiPlate_box1 = new GeoBox(16*CLHEP::mm,120*CLHEP::mm,0.75*CLHEP::mm);
	GeoBox* TiPlate_box2 = new GeoBox(19.9793*CLHEP::mm,19.9793*CLHEP::mm,2*CLHEP::mm);

	HepGeom::Transform3D TiPlate_Move1= HepGeom::Translate3D(0, 69*CLHEP::mm, 0);
	GeoShapeShift * mowe1 = new GeoShapeShift(TiPlate_Holder,TiPlate_Move1);
	GeoShapeUnion * TiPlatesemi1 = new GeoShapeUnion(TiPlate_Substr, mowe1);

	for(i=0; i<ALFA_Nb_Plates; i++){
		HepGeom::Transform3D TiPlate_MoveRot2 = HepGeom::Translate3D(ALFA_stagger[i]*CLHEP::mm, -26.872*CLHEP::mm, 1*CLHEP::mm)*HepGeom::RotateZ3D( 45*CLHEP::deg);
		HepGeom::Transform3D TiPlate_MoveRot3 = HepGeom::Translate3D(ALFA_stagger[i]*CLHEP::mm, -26.872*CLHEP::mm,-1*CLHEP::mm)*HepGeom::RotateZ3D(-45*CLHEP::deg);

		GeoShapeShift * mowe2 = new GeoShapeShift(TiPlate_box1, TiPlate_MoveRot2);
		GeoShapeShift * mowe3 = new GeoShapeShift(TiPlate_box1, TiPlate_MoveRot3);
		GeoShapeSubtraction * TiPlatesemi2 = new GeoShapeSubtraction(TiPlatesemi1,mowe2);
		GeoShapeSubtraction * TiPlatesemi3 = new GeoShapeSubtraction(TiPlatesemi2,mowe3);

		HepGeom::Transform3D TiPlate_MoveRot4 = HepGeom::Translate3D(ALFA_stagger[i]*CLHEP::mm, -32.5*CLHEP::mm,0*CLHEP::mm)*HepGeom::RotateZ3D(-45*CLHEP::deg);
		GeoShapeShift * mowe4 = new GeoShapeShift(TiPlate_box2, TiPlate_MoveRot4);
		GeoShapeSubtraction* shapeTi_Plate = new GeoShapeSubtraction(TiPlatesemi3, mowe4);

		pMapTiPlates->insert(std::pair<int,GeoShape*>(i,shapeTi_Plate));
	}
	
	return pMapTiPlates;
}

map<int,GeoShape*>* ALFA_DetectorFactory::CreateSolidODPlates()
{
	int i;
	std::map<int,GeoShape*>* pMapODPlates=new std::map<int,GeoShape*>();
	
	GeoBox* OD_Tibox1 = new GeoBox(60*CLHEP::mm,38.65*CLHEP::mm,0.4*CLHEP::mm);
	GeoBox* OD_Tibox2 = new GeoBox(10*CLHEP::mm,36.5*CLHEP::mm,1*CLHEP::mm);
	GeoBox* OD_Tibox3 = new GeoBox(19*CLHEP::mm,21.5*CLHEP::mm,2*CLHEP::mm);

	HepGeom::Transform3D OD_Move01= HepGeom::Translate3D(0, 75.15*CLHEP::mm, 0);
	GeoShapeShift * mowe01 = new GeoShapeShift(OD_Tibox2,OD_Move01);
	GeoShapeUnion * OD_Tisemi01 = new GeoShapeUnion(OD_Tibox1, mowe01);

	HepGeom::Transform3D OD_Move02= HepGeom::Translate3D(0, -19.15*CLHEP::mm, 0);//(0, -29.383*CLHEP::mm, 0);
	GeoShapeShift * mowe02 = new GeoShapeShift(OD_Tibox3, OD_Move02);
	GeoShapeSubtraction * OD_Tisemi02 = new GeoShapeSubtraction(OD_Tisemi01, mowe02);

	GeoBox* OD_Tibox5 = new GeoBox(60*CLHEP::mm,38.65*CLHEP::mm,0.3*CLHEP::mm);

	GeoTube* OD_Titube1 = new GeoTube(22.5*CLHEP::mm,72.5*CLHEP::mm,0.6*CLHEP::mm);
	GeoBox* OD_Tibox6 = new GeoBox(72.5*CLHEP::mm,150*CLHEP::mm,1*CLHEP::mm);
	GeoBox* OD_Tibox7 = new GeoBox(150*CLHEP::mm,72.5*CLHEP::mm,1*CLHEP::mm);

	HepGeom::Transform3D OD_Move04 = HepGeom::Translate3D(72.5*CLHEP::mm, 0*CLHEP::mm, 0*CLHEP::mm);
	GeoShapeShift * mowe04 = new GeoShapeShift(OD_Tibox6,OD_Move04);
	GeoShapeSubtraction * OD_Tisemi04 = new GeoShapeSubtraction(OD_Titube1, mowe04);

	HepGeom::Transform3D OD_Move05 = HepGeom::Translate3D(0*CLHEP::mm, 72.5*CLHEP::mm, 0*CLHEP::mm);
	GeoShapeShift * mowe05 = new GeoShapeShift(OD_Tibox7,OD_Move05);
	GeoShapeSubtraction * OD_Tisemi05 = new GeoShapeSubtraction(OD_Tisemi04, mowe05);

	GeoBox* OD_Tibox8 = new GeoBox(8.*CLHEP::mm,20.*CLHEP::mm,0.6*CLHEP::mm);
	GeoBox* OD_Tibox9 = new GeoBox(20.*CLHEP::mm,60.*CLHEP::mm,0.6*CLHEP::mm);

	HepGeom::Transform3D OD_Move06= HepGeom::Translate3D(4*CLHEP::mm, -42.5*CLHEP::mm, 0.*CLHEP::mm);
	GeoShapeShift * mowe06 = new GeoShapeShift(OD_Tibox8,OD_Move06);
	GeoShapeUnion * OD_Tisemi06 = new GeoShapeUnion(OD_Tisemi05, mowe06);

	HepGeom::Transform3D OD_Move07= HepGeom::Translate3D(-42.5*CLHEP::mm, 60*CLHEP::mm, 0.*CLHEP::mm);
	GeoShapeShift * mowe07 = new GeoShapeShift(OD_Tibox9,OD_Move07);
	GeoShapeUnion * OD_Tisemi07 = new GeoShapeUnion(OD_Tisemi06, mowe07);

	for(i=0;i<OD_Nb_Plates;i++)
	{
		HepGeom::Transform3D OD_Move08= HepGeom::Translate3D(-27*CLHEP::mm, (-8.183+OD_stagger[i])*CLHEP::mm, 0);//(0, -29.383*CLHEP::mm, 0);
		GeoShapeShift * mowe08 = new GeoShapeShift(OD_Tisemi07, OD_Move08);
		GeoShapeSubtraction * OD_Tisemi08 = new GeoShapeSubtraction(OD_Tibox5, mowe08);

		HepGeom::Transform3D OD_Move09= HepGeom::Translate3D(27*CLHEP::mm, (-8.183+OD_stagger[i])*CLHEP::mm, 0)*HepGeom::RotateY3D(180*CLHEP::deg);
		GeoShapeShift * mowe09 = new GeoShapeShift(OD_Tisemi07, OD_Move09);
		GeoShapeSubtraction * OD_Tisemi09 = new GeoShapeSubtraction(OD_Tisemi08, mowe09);

		HepGeom::Transform3D OD_Move11= HepGeom::Translate3D(-27*CLHEP::mm, (-0.383+OD_stagger[i])*CLHEP::mm, 0);//(0, -29.383*CLHEP::mm, 0);
		GeoShapeShift * mowe11 = new GeoShapeShift(OD_Tisemi07, OD_Move11);
		GeoShapeSubtraction * OD_Tisemi11 = new GeoShapeSubtraction(OD_Tibox5, mowe11);

		HepGeom::Transform3D OD_Move12= HepGeom::Translate3D(27*CLHEP::mm, (-0.383+OD_stagger[i])*CLHEP::mm, 0)*HepGeom::RotateY3D(180*CLHEP::deg);
		GeoShapeShift * mowe12 = new GeoShapeShift(OD_Tisemi07, OD_Move12);
		GeoShapeSubtraction * OD_Tisemi12 = new GeoShapeSubtraction(OD_Tisemi11, mowe12);

		HepGeom::Transform3D OD_Move14= HepGeom::Translate3D(0*CLHEP::mm, 0*CLHEP::mm, 0.7*CLHEP::mm);
		GeoShapeShift * mowe14 = new GeoShapeShift(OD_Tisemi09,OD_Move14);
		GeoShapeUnion * OD_Tisemi14 = new GeoShapeUnion(OD_Tisemi02, mowe14);

		HepGeom::Transform3D OD_Move15= HepGeom::Translate3D(0*CLHEP::mm, 0*CLHEP::mm,-0.7*CLHEP::mm);
		GeoShapeShift * mowe15 = new GeoShapeShift(OD_Tisemi12,OD_Move15);
		GeoShapeUnion * OD_Tisemi15 = new GeoShapeUnion(OD_Tisemi14, mowe15);

		HepGeom::Transform3D OD_Move16 = HepGeom::Translate3D(0, -19.15*CLHEP::mm, 0);
		GeoShapeShift * mowe16 = new GeoShapeShift(OD_Tibox3, OD_Move16);
		GeoShapeSubtraction * shapeOD_TiPlate = new GeoShapeSubtraction(OD_Tisemi15, mowe16);

		pMapODPlates->insert(std::pair<int,GeoShape*>(i,shapeOD_TiPlate));
	}
	
	return pMapODPlates;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Overlap Detector Fiber & Claddings

//void ALFA_DetectorFactory::ConstructODFiberCladdings(const eRPotName eRPName, GeoFullPhysVol* pPhysMotherVolume, const HepGeom::Transform3D& MotherTransform)
void ALFA_DetectorFactory::ConstructODFiberCladdings(const eRPotName eRPName, GeoFullPhysVol* pPhysMotherVolume, const HepGeom::Transform3D& MotherTransform, GeoAlignableTransform* pDetTransform)
{
	int i;
	char szLabel[32];
	
	GeoBox* OD_Cladbox1 = new GeoBox(4.*CLHEP::mm,3.75*CLHEP::mm,.25*CLHEP::mm);
	GeoBox* OD_Cladbox2 = new GeoBox(3.75*CLHEP::mm,15*CLHEP::mm,.25*CLHEP::mm);

	GeoTube* OD_Cladtube1 = new GeoTube(22.5*CLHEP::mm,30.0*CLHEP::mm,0.25*CLHEP::mm);
	GeoBox* OD_Cladbox3 = new GeoBox(30*CLHEP::mm,50*CLHEP::mm,1*CLHEP::mm);
	GeoBox* OD_Cladbox4 = new GeoBox(50*CLHEP::mm,30*CLHEP::mm,1*CLHEP::mm);

	HepGeom::Transform3D OD_CladMove1 = HepGeom::Translate3D(30*CLHEP::mm, 0*CLHEP::mm, 0*CLHEP::mm);
	GeoShapeShift * mowe1 = new GeoShapeShift(OD_Cladbox3, OD_CladMove1);
	GeoShapeSubtraction * OD_Cladsemi1 = new GeoShapeSubtraction(OD_Cladtube1, mowe1);

	HepGeom::Transform3D OD_CladMove2 = HepGeom::Translate3D(0*CLHEP::mm, 30*CLHEP::mm, 0*CLHEP::mm);
	GeoShapeShift * mowe2 = new GeoShapeShift(OD_Cladbox4, OD_CladMove2);
	GeoShapeSubtraction * OD_Cladsemi2 = new GeoShapeSubtraction(OD_Cladsemi1, mowe2);

	HepGeom::Transform3D OD_CladMov3= HepGeom::Translate3D(4*CLHEP::mm, -26.25*CLHEP::mm, 0);
	GeoShapeShift * mowe3 = new GeoShapeShift(OD_Cladbox1, OD_CladMov3);
	GeoShapeUnion * OD_Cladsemi3 = new GeoShapeUnion(OD_Cladsemi2, mowe3);

	HepGeom::Transform3D OD_CladMov4= HepGeom::Translate3D(-26.25*CLHEP::mm, 15*CLHEP::mm, 0);
	GeoShapeShift * mowe4 = new GeoShapeShift(OD_Cladbox2, OD_CladMov4);
	GeoShapeUnion * shapeOD_Cladding = new GeoShapeUnion(OD_Cladsemi3, mowe4);

  	// OD_Cladding - logical volume
	GeoLogVol* logOD_Cladding = new GeoLogVol("logOD_Cladding", shapeOD_Cladding, m_MapMaterials[string("PMMA")]);

	GeoFullPhysVol* physOD_CladdingU_0=NULL;
	GeoFullPhysVol* physOD_CladdingU_1=NULL;
	GeoFullPhysVol* physOD_CladdingV_0=NULL;
	GeoFullPhysVol* physOD_CladdingV_1=NULL;
	
	//RPPOSPARAMS RPosParams;
	//m_pGeoReader->GetRPPosParams(&RPosParams,eRPName);

	for(i=0; i<ODPLATESCNT; i++)
	{
		//physOD_CladdingU_0
		physOD_CladdingU_0 = new GeoFullPhysVol(logOD_Cladding);
		HepGeom::Transform3D TransODCladdingU0 = HepGeom::Translate3D(-27.*CLHEP::mm,(-0.858+OD_stagger[i])*CLHEP::mm,(14.3500000000+i*2)*CLHEP::mm);
		ConstructODFibers00(eRPName, i, EFT_ODFIBERU0, physOD_CladdingU_0, MotherTransform, TransODCladdingU0);

		pPhysMotherVolume->add(pDetTransform);
		pPhysMotherVolume->add(new GeoTransform(TransODCladdingU0));
		sprintf(szLabel,"physODclad[%d][0][%d]",eRPName,i);
		pPhysMotherVolume->add(new GeoNameTag(szLabel));
		pPhysMotherVolume->add(physOD_CladdingU_0);

		//physOD_CladdingU_1
		physOD_CladdingU_1 = new GeoFullPhysVol(logOD_Cladding);
		HepGeom::Transform3D TransODCladdingU1 = HepGeom::Translate3D( 27.*CLHEP::mm,(-0.858+OD_stagger[i])*CLHEP::mm,(14.3500000000+i*2)*CLHEP::mm)*HepGeom::RotateY3D(-180*CLHEP::deg);
		ConstructODFibers01(eRPName, i, EFT_ODFIBERU1, physOD_CladdingU_1, MotherTransform, TransODCladdingU1);

		pPhysMotherVolume->add(pDetTransform);
		pPhysMotherVolume->add(new GeoTransform(TransODCladdingU1));
		sprintf(szLabel,"physODclad[%d][1][%d]",eRPName,i);
		pPhysMotherVolume->add(new GeoNameTag(szLabel));
		pPhysMotherVolume->add(physOD_CladdingU_1);

		//physOD_CladdingV_0
		physOD_CladdingV_0 = new GeoFullPhysVol(logOD_Cladding);
		HepGeom::Transform3D TransODCladdingV0=HepGeom::Translate3D(-27.*CLHEP::mm,(-8.358+OD_stagger[i])*CLHEP::mm,(15.6500000000+i*2)*CLHEP::mm);
		ConstructODFibers00(eRPName, i, EFT_ODFIBERV0, physOD_CladdingV_0, MotherTransform, TransODCladdingV0);

		pPhysMotherVolume->add(pDetTransform);
		pPhysMotherVolume->add(new GeoTransform(TransODCladdingV0));
		sprintf(szLabel,"physODclad[%d][2][%d]",eRPName,i);
		pPhysMotherVolume->add(new GeoNameTag(szLabel));
		pPhysMotherVolume->add(physOD_CladdingV_0);

		//physOD_CladdingV_1
		physOD_CladdingV_1 = new GeoFullPhysVol(logOD_Cladding);
		HepGeom::Transform3D TransODCladdingV1 = HepGeom::Translate3D( 27.*CLHEP::mm,(-8.358+OD_stagger[i])*CLHEP::mm,(15.6500000000+i*2)*CLHEP::mm)*HepGeom::RotateY3D(-180*CLHEP::deg);
		ConstructODFibers01(eRPName, i, EFT_ODFIBERV1, physOD_CladdingV_1, MotherTransform, TransODCladdingV1);

		pPhysMotherVolume->add(pDetTransform);
		pPhysMotherVolume->add(new GeoTransform(TransODCladdingV1));
		sprintf(szLabel,"physODclad[%d][3][%d]",eRPName,i);
		pPhysMotherVolume->add(new GeoNameTag(szLabel));
		pPhysMotherVolume->add(physOD_CladdingV_1);
	}
}

void ALFA_DetectorFactory::ConstructODFibers00(const eRPotName eRPName, const int iODPlate, eFiberType eFType, GeoFullPhysVol* pMotherVolume, const HepGeom::Transform3D& MotherTransform, const HepGeom::Transform3D& TransODCladding)
{
	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_GeoModel::ALFA_DetectorFactory");
	
	int i;
	char szLabel[32];
	HepGeom::Transform3D TransODFiber;
	
	GeoBox* OD_Fiberbox1 = new GeoBox(4.*CLHEP::mm,.24*CLHEP::mm,.24*CLHEP::mm);
	GeoBox* OD_Fiberbox2 = new GeoBox(.24*CLHEP::mm,15*CLHEP::mm,.24*CLHEP::mm);
	GeoBox* OD_Fiberbox3 = new GeoBox(30*CLHEP::mm,50*CLHEP::mm,1*CLHEP::mm);
	GeoBox* OD_Fiberbox4 = new GeoBox(50*CLHEP::mm,30*CLHEP::mm,1*CLHEP::mm);

	for(i=0; i<OD_Nb_Fibers; i++){
		GeoTube* OD_Fibertube1 = new GeoTube((29.51-0.5*i)*CLHEP::mm,(29.99-0.5*i)*CLHEP::mm,0.24*CLHEP::mm);

		HepGeom::Transform3D OD_FiberMove1 = HepGeom::Translate3D(30*CLHEP::mm, 0*CLHEP::mm, 0*CLHEP::mm);
		GeoShapeShift * mowe1 = new GeoShapeShift(OD_Fiberbox3, OD_FiberMove1);
		GeoShapeSubtraction * OD_Fibersemi1 = new GeoShapeSubtraction(OD_Fibertube1, mowe1);

		HepGeom::Transform3D OD_FiberMove2 = HepGeom::Translate3D(0*CLHEP::mm, 30*CLHEP::mm, 0*CLHEP::mm);
		GeoShapeShift * mowe2 = new GeoShapeShift(OD_Fiberbox4, OD_FiberMove2);
		GeoShapeSubtraction * OD_Fibersemi2 = new GeoShapeSubtraction(OD_Fibersemi1, mowe2);

		HepGeom::Transform3D OD_FiberMov4= HepGeom::Translate3D((-29.75+0.5*i)*CLHEP::mm, 15*CLHEP::mm, 0);
		GeoShapeShift * mowe4 = new GeoShapeShift(OD_Fiberbox2, OD_FiberMov4);
		GeoShapeUnion * shapeOD_Fiber = new GeoShapeUnion(OD_Fibersemi2, mowe4);

		GeoLogVol* logOD_Fiber = new GeoLogVol("logOD_Fiber", shapeOD_Fiber, m_MapMaterials[string("Polystyrene")]);
		GeoLogVol* logOD_FiberActive = new GeoLogVol("logOD_FiberActive", OD_Fiberbox1, m_MapMaterials[string("Polystyrene")]);

		double fYPos = 0.0;
		double fAngle = 0.0;
		if (eFType==EFT_ODFIBERU0)
		{
			//(+16 because U0-nFiberID is indexed from 16 to 30)
			fYPos=m_pGeoReader->GetODFiberCentreYPos(eRPName, EFT_ODFIBERU0, iODPlate+1, i+16);
			fAngle=m_pGeoReader->GetODFiberAngle(eRPName, EFT_ODFIBERU0, iODPlate+1, i+16);
			sprintf(szLabel,"ODFiberU0Active[%d][%d][%d]",eRPName,iODPlate+1,i+1);
		}
		else if (eFType==EFT_ODFIBERV0)
		{
			fYPos=m_pGeoReader->GetODFiberCentreYPos(eRPName, EFT_ODFIBERV0, iODPlate+1, i+1);
			fAngle=m_pGeoReader->GetODFiberAngle(eRPName, EFT_ODFIBERV0, iODPlate+1, i+1);
			sprintf(szLabel,"ODFiberV0Active[%d][%d][%d]",eRPName,iODPlate+1,i+1);
		}
	
		TransODFiber=HepGeom::Translate3D(4*CLHEP::mm, fYPos, 0.0*CLHEP::mm)*HepGeom::RotateZ3D(fAngle);
		
		//ODFiber00Active - physical volumes
		GeoPhysVol* physOD_FiberActive = new GeoPhysVol(logOD_FiberActive);
		pMotherVolume->add(new GeoTransform(TransODFiber));
		pMotherVolume->add(new GeoNameTag(szLabel));
		pMotherVolume->add(physOD_FiberActive);
		
		//ODFiber00Active - set translation vector of the fiber with respect to the main reference point
		HepGeom::Transform3D TransTotal=MotherTransform*TransODCladding*TransODFiber;
		HepGeom::Point3D<float> TransPoint=TransTotal*HepGeom::Point3D<float>(0.0,0.0,0.0);
		HepGeom::Vector3D<float> TransVector=TransTotal*HepGeom::Vector3D<float>(1.0,0.0,0.0);
		float fSlope=tan(TransVector.phi());

		/*
		if(eRPName==ERPN_A7L1U && eFType==EFT_ODFIBERU0){
			TransPoint=MotherTransform*HepGeom::Point3D<float>(0.0*CLHEP::mm,0.0*CLHEP::mm,0.0*CLHEP::mm);
			LogStream<<MSG::INFO<<"(U0) Trans Mother only="<<setprecision(10)<<TransPoint<<endmsg;

			TransPoint=TransODCladding*HepGeom::Point3D<float>(0.0*CLHEP::mm,0.0*CLHEP::mm,0.0*CLHEP::mm);
			LogStream<<MSG::INFO<<"(U0) Trans Cladding only="<<setprecision(10)<<TransPoint.z()<<endmsg;

			TransPoint=MotherTransform*TransODCladding*HepGeom::Point3D<float>(0.0*CLHEP::mm,0.0*CLHEP::mm,0.0*CLHEP::mm);
			LogStream<<MSG::INFO<<"(U0) Trans Mother+Cladding only ="<<setprecision(10)<<TransPoint.z()<<endmsg;

			TransPoint=TransODCladding*TransODFiber*HepGeom::Point3D<float>(0.0*CLHEP::mm,0.0*CLHEP::mm,0.0*CLHEP::mm);
			LogStream<<MSG::INFO<<"(U0) Trans Cladding+fiber only ="<<setprecision(10)<<TransPoint.z()<<endmsg;
		}
		if(eRPName==ERPN_A7L1U && eFType==EFT_ODFIBERV0){
			TransPoint=MotherTransform*HepGeom::Point3D<float>(0.0*CLHEP::mm,0.0*CLHEP::mm,0.0*CLHEP::mm);
			LogStream<<MSG::INFO<<"(V0) Trans Mother only="<<setprecision(10)<<TransPoint<<endmsg;

			TransPoint=TransODCladding*HepGeom::Point3D<float>(0.0*CLHEP::mm,0.0*CLHEP::mm,0.0*CLHEP::mm);
			LogStream<<MSG::INFO<<"(V0) Trans Cladding only="<<setprecision(10)<<TransPoint.z()<<endmsg;

			TransPoint=MotherTransform*TransODCladding*HepGeom::Point3D<float>(0.0*CLHEP::mm,0.0*CLHEP::mm,0.0*CLHEP::mm);
			LogStream<<MSG::INFO<<"(V0) Trans Mother+Cladding only ="<<setprecision(10)<<TransPoint.z()<<endmsg;

			TransPoint=TransODCladding*TransODFiber*HepGeom::Point3D<float>(0.0*CLHEP::mm,0.0*CLHEP::mm,0.0*CLHEP::mm);
			LogStream<<MSG::INFO<<"(V0) Trans Cladding+fiber only="<<setprecision(10)<<TransPoint.z()<<endmsg;
		}*/
		
		if (eFType==EFT_ODFIBERU0)
		{
			m_pGeoReader->SetODFiberPositionToMainReference(eRPName, EFT_ODFIBERU0, iODPlate+1, i+16, TransPoint, fSlope);
		}
		else if (eFType==EFT_ODFIBERV0)
		{
			m_pGeoReader->SetODFiberPositionToMainReference(eRPName, EFT_ODFIBERV0, iODPlate+1, i+1, TransPoint, fSlope);
		}

		// ODFiber00 physical volume
		if (eFType==EFT_ODFIBERU0) sprintf(szLabel,"ODFiberU0[%d][%d][%d]",eRPName,iODPlate+1,i+1);
		else if (eFType==EFT_ODFIBERV0) sprintf(szLabel,"ODFiberV0[%d][%d][%d]",eRPName,iODPlate+1,i+1);
		GeoPhysVol* physOD_Fiber = new GeoPhysVol(logOD_Fiber);
		pMotherVolume->add(new GeoTransform(HepGeom::Translate3D(0*CLHEP::mm,0*CLHEP::mm,0*CLHEP::mm)));
		pMotherVolume->add(new GeoNameTag(szLabel));
		pMotherVolume->add(physOD_Fiber);
	}
}

void ALFA_DetectorFactory::ConstructODFibers01(const eRPotName eRPName, const int iODPlate, eFiberType eFType, GeoFullPhysVol* pMotherVolume, const HepGeom::Transform3D& MotherTransform, const HepGeom::Transform3D& TransODCladding)
{
	int i;
	char szLabel[32];
	HepGeom::Transform3D TransODFiber;
	
	GeoBox* OD_Fiberbox1 = new GeoBox(4.*CLHEP::mm,.24*CLHEP::mm,.24*CLHEP::mm);
	GeoBox* OD_Fiberbox2 = new GeoBox(.24*CLHEP::mm,15*CLHEP::mm,.24*CLHEP::mm);
	GeoBox* OD_Fiberbox3 = new GeoBox(30*CLHEP::mm,50*CLHEP::mm,1*CLHEP::mm);
	GeoBox* OD_Fiberbox4 = new GeoBox(50*CLHEP::mm,30*CLHEP::mm,1*CLHEP::mm);

	for(i=0; i<OD_Nb_Fibers; i++){
		GeoTube* OD_Fibertube1 = new GeoTube((22.51+0.5*i)*CLHEP::mm,(22.99+0.5*i)*CLHEP::mm,0.24*CLHEP::mm);

		HepGeom::Transform3D OD_FiberMove1 = HepGeom::Translate3D(30*CLHEP::mm, 0*CLHEP::mm, 0*CLHEP::mm);
		GeoShapeShift * mowe1 = new GeoShapeShift(OD_Fiberbox3, OD_FiberMove1);
		GeoShapeSubtraction * OD_Fibersemi1 = new GeoShapeSubtraction(OD_Fibertube1, mowe1);

		HepGeom::Transform3D OD_FiberMove2 = HepGeom::Translate3D(0*CLHEP::mm, 30*CLHEP::mm, 0*CLHEP::mm);
		GeoShapeShift * mowe2 = new GeoShapeShift(OD_Fiberbox4, OD_FiberMove2);
		GeoShapeSubtraction * OD_Fibersemi2 = new GeoShapeSubtraction(OD_Fibersemi1, mowe2);

		HepGeom::Transform3D OD_FiberMov4= HepGeom::Translate3D((-22.75-0.5*i)*CLHEP::mm, 15*CLHEP::mm, 0);
		GeoShapeShift * mowe4 = new GeoShapeShift(OD_Fiberbox2, OD_FiberMov4);
		GeoShapeUnion * shapeOD_Fiber = new GeoShapeUnion(OD_Fibersemi2, mowe4);

		GeoLogVol* logOD_Fiber = new GeoLogVol("logOD_Fiber", shapeOD_Fiber, m_MapMaterials[string("Polystyrene")]);
		GeoLogVol* logOD_FiberActive = new GeoLogVol("logOD_FiberActive", OD_Fiberbox1, m_MapMaterials[string("Polystyrene")]);

		double fYPos = 0.0;
		double fAngle = 0.0;
		if (eFType==EFT_ODFIBERU1)
		{
			//1.10.2010 LN: change of indexation scheme
			//fYPos=pGeoReader->GetODFiberCentreYPos(eRPName, EFT_ODFIBERU1, iODPlate+1, i+1);
			//fAngle=pGeoReader->GetODFiberAngle(eRPName, EFT_ODFIBERU1, iODPlate+1, i+1);
			fYPos=m_pGeoReader->GetODFiberCentreYPos(eRPName, EFT_ODFIBERU1, iODPlate+1, i+16);
			fAngle=m_pGeoReader->GetODFiberAngle(eRPName, EFT_ODFIBERU1, iODPlate+1, i+16);

			sprintf(szLabel,"ODFiberU1Active[%d][%d][%d]",eRPName,iODPlate+1,i+1);
		}
		else if (eFType==EFT_ODFIBERV1)
		{
			//1.10.2010 LN: change of indexation scheme
			//(+16 because V1-nFiberID is indexed from 16 to 30)
			//fYPos=pGeoReader->GetODFiberCentreYPos(eRPName, EFT_ODFIBERV1, iODPlate+1, i+16);
			//fAngle=pGeoReader->GetODFiberAngle(eRPName, EFT_ODFIBERV1, iODPlate+1, i+16);
			fYPos=m_pGeoReader->GetODFiberCentreYPos(eRPName, EFT_ODFIBERV1, iODPlate+1, i+1);
			fAngle=m_pGeoReader->GetODFiberAngle(eRPName, EFT_ODFIBERV1, iODPlate+1, i+1);

			sprintf(szLabel,"ODFiberV1Active[%d][%d][%d]",eRPName,iODPlate+1,i+1);
		}
		
		TransODFiber=HepGeom::Translate3D(4*CLHEP::mm, fYPos, 0*CLHEP::mm)*HepGeom::RotateZ3D(fAngle);
		
		//ODFiber01Active - physical volumes
		GeoPhysVol* physOD_FiberActive = new GeoPhysVol(logOD_FiberActive);
		pMotherVolume->add(new GeoTransform(TransODFiber));
		pMotherVolume->add(new GeoNameTag(szLabel));
		pMotherVolume->add(physOD_FiberActive);
		
		//ODFiber01Active - set translation vector of the fiber with respect to the main reference point
		HepGeom::Transform3D TransTotal=MotherTransform*TransODCladding*TransODFiber;
		HepGeom::Point3D<float> TransPoint=TransTotal*HepGeom::Point3D<float>(0.0,0.0,0.0);
		HepGeom::Vector3D<float> TransVector=TransTotal*HepGeom::Vector3D<float>(1.0,0.0,0.0);
		float fSlope=tan(TransVector.phi());
		
		if (eFType==EFT_ODFIBERU1)
		{
			//1.10.2010 LN: change of indexation scheme
			//pGeoReader->SetODFiberPositionToMainReference(eRPName, EFT_ODFIBERU1, iODPlate+1, i+1, TransPoint, fSlope);
			m_pGeoReader->SetODFiberPositionToMainReference(eRPName, EFT_ODFIBERU1, iODPlate+1, i+16, TransPoint, fSlope);
		}
		else if (eFType==EFT_ODFIBERV1)
		{
			//1.10.2010 LN: change of indexation scheme
			//pGeoReader->SetODFiberPositionToMainReference(eRPName, EFT_ODFIBERV1, iODPlate+1, i+16, TransPoint, fSlope);
			m_pGeoReader->SetODFiberPositionToMainReference(eRPName, EFT_ODFIBERV1, iODPlate+1, i+1, TransPoint, fSlope);
		}

		//ODFiber01 physical volume
		if (eFType==EFT_ODFIBERU1) sprintf(szLabel,"ODFiberU1[%d][%d][%d]",eRPName,iODPlate+1,i+1);
		else if (eFType==EFT_ODFIBERV1) sprintf(szLabel,"ODFiberV1[%d][%d][%d]",eRPName,iODPlate+1,i+1);
		GeoPhysVol* physOD_Fiber = new GeoPhysVol(logOD_Fiber);
		pMotherVolume->add(new GeoTransform(HepGeom::Translate3D(0*CLHEP::mm,0*CLHEP::mm,0*CLHEP::mm)));
		pMotherVolume->add(new GeoNameTag(szLabel));
		pMotherVolume->add(physOD_Fiber);
	}
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Fiber & Claddings
 
//void ALFA_DetectorFactory::ConstructUFiberCladdings(const eRPotName eRPName, GeoFullPhysVol* pMotherVolume, const HepGeom::Transform3D& MotherTransform)
void ALFA_DetectorFactory::ConstructUFiberCladdings(const eRPotName eRPName, GeoFullPhysVol* pMotherVolume, const HepGeom::Transform3D& MotherTransform, GeoAlignableTransform* pDetTransform)
{
	char strLabel[32];
	//const double ALFA_stagger[10] = {0.0, 0.283, -0.141, 0.141, -0.283, 0.354, -0.071, 0.212, -0.212, 0.071};
	
	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_DetectorFactory::ConstructUFiberCladdings");
	
	//Claddings
	PLATEPARAMS PlateParams;
	
	GeoBox *ALFA_Cladbox1, *ALFA_Cladbox2;
	GeoShapeSubtraction* shapeALFA_Clad;
	
	//G4RotationMatrix* ALFA_CladdingURot = new G4RotationMatrix;
	//ALFA_CladdingURot -> rotateZ(-45*CLHEP::deg);
	//ALFA_CladdingURot -> rotateY(180*CLHEP::deg);
	//HepGeom::RotateZ3D ALFA_CladdingURot=HepGeom::RotateZ3D(-45.0*CLHEP::deg);

	GeoShapeShift* mowe1;
	GeoLogVol* pLogCladdingU;
	GeoFullPhysVol* physALFA_CladdingU[ALFAPLATESCNT];
	HepGeom::Transform3D TransCladdingU[ALFAPLATESCNT];

	//RPPOSPARAMS RPosParams;
	//m_pGeoReader->GetRPPosParams(&RPosParams,eRPName);
	//StoredPhysVol* pStPhysClad=NULL;
	//char szLabel[32];
		
	for(int i=0; i<ALFAPLATESCNT; i++)
	{
		m_pGeoReader->GetPlateParams(&PlateParams,eRPName,i+1);
		//ALFA_Cladbox1 = new G4Box("ALFA_Cladbox1", 16.*CLHEP::mm,16.*CLHEP::mm,.25*CLHEP::mm);
		ALFA_Cladbox1 = new GeoBox(PlateParams.fUCladdingSizeX/2.0,16.0*CLHEP::mm,0.25*CLHEP::mm);
		ALFA_Cladbox2 = new GeoBox(16.0*CLHEP::mm,16.0*CLHEP::mm,0.5*CLHEP::mm);
	
		//G4RotationMatrix* Clad_Rot1 = new G4RotationMatrix; 
		//Clad_Rot1 -> rotateZ(-45*CLHEP::deg);
		//G4ThreeVector  Clad_Move1(-17.415*CLHEP::mm, -17.415*CLHEP::mm, 0*CLHEP::mm);
		HepGeom::Transform3D Clad_Move1=HepGeom::Translate3D(-17.415*CLHEP::mm, -17.415*CLHEP::mm, 0*CLHEP::mm)*HepGeom::RotateZ3D(-45*CLHEP::deg);
	
		mowe1=new GeoShapeShift(ALFA_Cladbox2,Clad_Move1);
		shapeALFA_Clad = new GeoShapeSubtraction(ALFA_Cladbox1,mowe1);
		pLogCladdingU = new GeoLogVol("logALFA_CladdingU",shapeALFA_Clad, m_MapMaterials[string("PMMA")]);
	
		sprintf(strLabel,"ALFA_CladdingU[%d][%d]",eRPName,i);
		TransCladdingU[i]=HepGeom::Translate3D(ALFA_stagger[i]*CLHEP::mm,-20.897*CLHEP::mm, (-18+i*2)*CLHEP::mm)*HepGeom::RotateZ3D(+45.0*CLHEP::deg);

		physALFA_CladdingU[i] = new GeoFullPhysVol(pLogCladdingU);
		pMotherVolume->add(new GeoNameTag(strLabel));
		pMotherVolume->add(pDetTransform);
		pMotherVolume->add(new GeoTransform(TransCladdingU[i]));
		pMotherVolume->add(physALFA_CladdingU[i]);

		/*
		if(eRPName==ERPN_A7L1U && i==0){
			sprintf(szLabel,"StALFA_CladdingU[03][00]");
			pStPhysClad=new StoredPhysVol(physALFA_CladdingU[i]);
			StatusCode sc=m_pDetectorStore->record(pStPhysClad,szLabel);
			if(!sc.isSuccess()) throw std::runtime_error("Cannot store alignable record");
		}*/

	}
	
	//Fibers
	GeoFullPhysVol* physiFibU;
	GeoLogVol* logALFA_FiberU;
	//GeoLogVol* logALFA_FiberU[ALFAPLATESCNT][ALFAFIBERSCNT];

	GeoBox* CladTrim = new GeoBox(16.*CLHEP::mm,16.*CLHEP::mm,.5*CLHEP::mm);
	GeoBox* Fib1 = new GeoBox(0.24*CLHEP::mm,16.*CLHEP::mm,.24*CLHEP::mm);

	//G4RotationMatrix* FibRotU = new G4RotationMatrix;
	//G4RotationMatrix* pFibRotUSlope = new G4RotationMatrix; 
	//FibRotU -> rotateZ(45*CLHEP::deg);
	HepGeom::Transform3D TransFiber;
		
	for(int j=0; j<ALFAFIBERSCNT; j++)
	{
		//HepGeom::Transform3D FibMove=HepGeom::Translate3D((-33.165+0.5*j)*CLHEP::mm, -17.415*CLHEP::mm, 0*CLHEP::mm)*HepGeom::RotateZ3D(+45.0*CLHEP::deg);
		//const GeoShapeShift* Sh6=new GeoShapeShift(CladTrim,FibMove);
		//const GeoShapeSubtraction* solFibU=new GeoShapeSubtraction(Fib1,Sh6);
		//sprintf(strLabel,"logALFA_FiberU[%d]",j+1);
		//logALFA_FiberU=new GeoLogVol(strLabel,solFibU,MapMaterials[string("Polystyrene")]);

		for(int i=0; i<ALFAPLATESCNT; i++)
		{
			double fXPos=m_pGeoReader->GetUFiberCentreXPos(eRPName, i+1, j+1);
			double fAngle=m_pGeoReader->GetUFiberAngle(eRPName, i+1, j+1);
			double fDx=fXPos-(+15.75-0.5*(j))*CLHEP::mm;

			HepGeom::Transform3D TrimMove=HepGeom::Translate3D((-33.165+0.5*j)*CLHEP::mm-fDx, -17.415*CLHEP::mm, 0*CLHEP::mm)*HepGeom::RotateZ3D(+45.0*CLHEP::deg); //bug fix 23.2.2010
			const GeoShapeShift* Sh6=new GeoShapeShift(CladTrim,TrimMove);
			const GeoShapeSubtraction* solFibU=new GeoShapeSubtraction(Fib1,Sh6);
			sprintf(strLabel,"logALFA_FiberU[%d]",j+1);
			logALFA_FiberU=new GeoLogVol(strLabel,solFibU,m_MapMaterials[string("Polystyrene")]);

			sprintf(strLabel,"ALFA_FiberU[%d][%d][%d]",eRPName,i+1,j+1);
			TransFiber=HepGeom::Translate3D(fXPos, 0*CLHEP::mm, 0*CLHEP::mm)*HepGeom::RotateZ3D(-fAngle);
			physiFibU=new GeoFullPhysVol(logALFA_FiberU);
			physALFA_CladdingU[i]->add(new GeoNameTag(strLabel));
			physALFA_CladdingU[i]->add(new GeoTransform(TransFiber));
			physALFA_CladdingU[i]->add(physiFibU);
			
			//set translation vector of the fiber with respect to the main reference point
			HepGeom::Transform3D TransTotal=MotherTransform*TransCladdingU[i]*TransFiber;
			HepGeom::Point3D<float> TransPoint=TransTotal*HepGeom::Point3D<float>(0.0,0.0,0.0);
			HepGeom::Vector3D<float> TransVector=TransTotal*HepGeom::Vector3D<float>(0.0,1.0,0.0);
			float fSlope=tan(TransVector.phi());
			
			m_pGeoReader->SetUFiberPositionToMainReference(eRPName, i+1, j+1, TransPoint, fSlope);
		}
	}
}

//void ALFA_DetectorFactory::ConstructVFiberCladdings(const eRPotName eRPName, GeoFullPhysVol* pMotherVolume, const HepGeom::Transform3D& MotherTransform)
void ALFA_DetectorFactory::ConstructVFiberCladdings(const eRPotName eRPName, GeoFullPhysVol* pMotherVolume, const HepGeom::Transform3D& MotherTransform, GeoAlignableTransform* pDetTransform)
{
	char strLabel[32];
	//const double ALFA_stagger[10] = {0.0, 0.283, -0.141, 0.141, -0.283, 0.354, -0.071, 0.212, -0.212, 0.071};
	
	//Claddings
	PLATEPARAMS PlateParams;
	
	GeoShapeShift* mowe1;
	GeoBox *ALFA_Cladbox1, *ALFA_Cladbox2;
	GeoShapeSubtraction* shapeALFA_Clad;
	
	//G4RotationMatrix* ALFA_CladdingVRot = new G4RotationMatrix;
	//ALFA_CladdingVRot -> rotateZ(-45*CLHEP::deg);
	//ALFA_CladdingVRot -> rotateY(180*CLHEP::deg);
	//HepGeom::RotateZ3D ALFA_CladdingVRot=HepGeom::RotateZ3D(+45.0*CLHEP::deg);

	GeoLogVol* pLogCladdingV;
	GeoFullPhysVol* physALFA_CladdingV[ALFAPLATESCNT];
	HepGeom::Transform3D TransCladdingV[ALFAPLATESCNT];

	//RPPOSPARAMS RPosParams;
	//m_pGeoReader->GetRPPosParams(&RPosParams,eRPName);
	
	for(int i=0; i<ALFAPLATESCNT; i++)
	{
		m_pGeoReader->GetPlateParams(&PlateParams,eRPName,i+1);
		//ALFA_Cladbox1 = new G4Box("ALFA_Cladbox1", 16.*CLHEP::mm,16.*CLHEP::mm,.25*CLHEP::mm);
		ALFA_Cladbox1 = new GeoBox(PlateParams.fVCladdingSizeX/2.0,16.*CLHEP::mm,.25*CLHEP::mm);
		ALFA_Cladbox2 = new GeoBox(16.0*CLHEP::mm,16.0*CLHEP::mm,0.5*CLHEP::mm);
	
		//G4RotationMatrix* Clad_Rot1 = new G4RotationMatrix; 
		//Clad_Rot1 -> rotateZ(-45*CLHEP::deg);
		//G4ThreeVector  Clad_Move1(-17.415*CLHEP::mm, -17.415*CLHEP::mm, 0*CLHEP::mm);
		HepGeom::Transform3D Clad_Move1=HepGeom::Translate3D(+17.415*CLHEP::mm, -17.415*CLHEP::mm, 0*CLHEP::mm)*HepGeom::RotateZ3D(-45*CLHEP::deg);
	
		mowe1=new GeoShapeShift(ALFA_Cladbox2,Clad_Move1);
		shapeALFA_Clad = new GeoShapeSubtraction(ALFA_Cladbox1,mowe1);
		pLogCladdingV = new GeoLogVol("logALFA_CladdingV",shapeALFA_Clad, m_MapMaterials[string("PMMA")]);
	
		sprintf(strLabel,"ALFA_CladdingV[%d][%d]",eRPName,i);
		TransCladdingV[i]=HepGeom::Translate3D(ALFA_stagger[i]*CLHEP::mm,-20.897*CLHEP::mm, (-17+i*2)*CLHEP::mm)*HepGeom::RotateZ3D(-45.0*CLHEP::deg);

		physALFA_CladdingV[i] = new GeoFullPhysVol(pLogCladdingV);
		pMotherVolume->add(new GeoNameTag(strLabel));
		pMotherVolume->add(pDetTransform);
		pMotherVolume->add(new GeoTransform(TransCladdingV[i]));
		pMotherVolume->add(physALFA_CladdingV[i]);
	}
	
	//Fibers
	GeoFullPhysVol* physiFibV;
	GeoLogVol* logALFA_FiberV;
	//GeoLogVol* logALFA_FiberV[ALFAPLATESCNT][ALFAFIBERSCNT];

	GeoBox* CladTrim = new GeoBox(16.*CLHEP::mm,16.*CLHEP::mm,.5*CLHEP::mm);
	GeoBox* Fib1 = new GeoBox(0.24*CLHEP::mm,16.*CLHEP::mm,.24*CLHEP::mm);

	//G4RotationMatrix* FibRotV = new G4RotationMatrix;
	//G4RotationMatrix* pFibRotVSlope = new G4RotationMatrix; 
	//FibRotV -> rotateZ(45*CLHEP::deg);
	HepGeom::Transform3D TransFiber;
		
	for(int j=0; j<ALFAFIBERSCNT; j++)
	{
		//HepGeom::Transform3D TrimMove=HepGeom::Translate3D((1.635+0.5*j)*CLHEP::mm, -17.415*CLHEP::mm, 0*CLHEP::mm)*HepGeom::RotateZ3D(+45.0*CLHEP::deg); //moved to inner loop
		//const GeoShapeShift* Sh6=new GeoShapeShift(CladTrim,TrimMove);
		//const GeoShapeSubtraction* solFibV=new GeoShapeSubtraction(Fib1,Sh6);
		//sprintf(strLabel,"logALFA_FiberV[%d]",j+1);
		//logALFA_FiberV=new GeoLogVol(strLabel,solFibV,MapMaterials[string("Polystyrene")]);

		for(int i=0; i<ALFAPLATESCNT; i++)
		{
			double fXPos=m_pGeoReader->GetVFiberCentreXPos(eRPName, i+1, j+1);
			double fAngle=m_pGeoReader->GetVFiberAngle(eRPName, i+1, j+1);
			double fDx=fXPos-(+15.75-0.5*(j))*CLHEP::mm;

			HepGeom::Transform3D TrimMove=HepGeom::Translate3D((1.665+0.5*j)*CLHEP::mm-fDx, -17.415*CLHEP::mm, 0*CLHEP::mm)*HepGeom::RotateZ3D(+45.0*CLHEP::deg);
			const GeoShapeShift* Sh6=new GeoShapeShift(CladTrim,TrimMove);
			const GeoShapeSubtraction* solFibV=new GeoShapeSubtraction(Fib1,Sh6);
			sprintf(strLabel,"logALFA_FiberV[%d]",j+1);
			logALFA_FiberV=new GeoLogVol(strLabel,solFibV,m_MapMaterials[string("Polystyrene")]);

			sprintf(strLabel,"ALFA_FiberV[%d][%d][%d]",eRPName,i+1,j+1);
			TransFiber=HepGeom::Translate3D(fXPos, 0*CLHEP::mm, 0*CLHEP::mm)*HepGeom::RotateZ3D(-fAngle);
			
			physiFibV=new GeoFullPhysVol(logALFA_FiberV);
			physALFA_CladdingV[i]->add(new GeoNameTag(strLabel));
			physALFA_CladdingV[i]->add(new GeoTransform(TransFiber));
			physALFA_CladdingV[i]->add(physiFibV);
			
			//set translation vector of the fiber with respect to the main reference point
			HepGeom::Transform3D TransTotal=MotherTransform*TransCladdingV[i]*TransFiber;
			
			HepGeom::Point3D<float> TransPoint=TransTotal*HepGeom::Point3D<float>(0.0,0.0,0.0);
			HepGeom::Vector3D<float> TransVector=TransTotal*HepGeom::Vector3D<float>(0.0,1.0,0.0);
			float fSlope=tan(TransVector.phi());
			m_pGeoReader->SetVFiberPositionToMainReference(eRPName, i+1, j+1, TransPoint, fSlope);
		}
	}
}
/*
void ALFA_DetectorFactory::SetIsTransformInDetector(const vector<bool> &bIsTransformInDetector)
{
	bIsTransformInDetector = bIsTransformInDetector;
}

void ALFA_DetectorFactory::SetIsTransformInStation(const vector<bool> &bIsTransformInStation)
{
	bIsTransformInStation = bIsTransformInStation;
}

void ALFA_DetectorFactory::SetPointTransformInDetector(	const vector<double> &pointTransformInDetectorB7L1U, const vector<double> &pointTransformInDetectorB7L1L,
														const vector<double> &pointTransformInDetectorA7L1U, const vector<double> &pointTransformInDetectorA7L1L,
														const vector<double> &pointTransformInDetectorB7R1U, const vector<double> &pointTransformInDetectorB7R1L,
														const vector<double> &pointTransformInDetectorA7R1U, const vector<double> &pointTransformInDetectorA7R1L)
{
	pointTransformInDetectorB7L1U = pointTransformInDetectorB7L1U;
	pointTransformInDetectorB7L1L = pointTransformInDetectorB7L1L;
	pointTransformInDetectorA7L1U = pointTransformInDetectorA7L1U;
	pointTransformInDetectorA7L1L = pointTransformInDetectorA7L1L;
	pointTransformInDetectorB7R1U = pointTransformInDetectorB7R1U;
	pointTransformInDetectorB7R1L = pointTransformInDetectorB7R1L;
	pointTransformInDetectorA7R1U = pointTransformInDetectorA7R1U;
	pointTransformInDetectorA7R1L = pointTransformInDetectorA7R1L;
}

void ALFA_DetectorFactory::SetVecTransformInDetector(	const vector<double> &vecTransformInDetectorB7L1U, const vector<double> &vecTransformInDetectorB7L1L,
														const vector<double> &vecTransformInDetectorA7L1U, const vector<double> &vecTransformInDetectorA7L1L,
														const vector<double> &vecTransformInDetectorB7R1U, const vector<double> &vecTransformInDetectorB7R1L,
														const vector<double> &vecTransformInDetectorA7R1U, const vector<double> &vecTransformInDetectorA7R1L)
{
	vecTransformInDetectorB7L1U = vecTransformInDetectorB7L1U;
	vecTransformInDetectorB7L1L = vecTransformInDetectorB7L1L;
	vecTransformInDetectorA7L1U = vecTransformInDetectorA7L1U;
	vecTransformInDetectorA7L1L = vecTransformInDetectorA7L1L;
	vecTransformInDetectorB7R1U = vecTransformInDetectorB7R1U;
	vecTransformInDetectorB7R1L = vecTransformInDetectorB7R1L;
	vecTransformInDetectorA7R1U = vecTransformInDetectorA7R1U;
	vecTransformInDetectorA7R1L = vecTransformInDetectorA7R1L;
}

void ALFA_DetectorFactory::SetVecTransformInStation(	const vector<double> &vecTransformInStationB7L1U, const vector<double> &vecTransformInStationB7L1L,
														const vector<double> &vecTransformInStationA7L1U, const vector<double> &vecTransformInStationA7L1L,
														const vector<double> &vecTransformInStationB7R1U, const vector<double> &vecTransformInStationB7R1L,
														const vector<double> &vecTransformInStationA7R1U, const vector<double> &vecTransformInStationA7R1L)
{
	vecTransformInStationB7L1U = vecTransformInStationB7L1U;
	vecTransformInStationB7L1L = vecTransformInStationB7L1L;
	vecTransformInStationA7L1U = vecTransformInStationA7L1U;
	vecTransformInStationA7L1L = vecTransformInStationA7L1L;
	vecTransformInStationB7R1U = vecTransformInStationB7R1U;
	vecTransformInStationB7R1L = vecTransformInStationB7R1L;
	vecTransformInStationA7R1U = vecTransformInStationA7R1U;
	vecTransformInStationA7R1L = vecTransformInStationA7R1L;
}
*/
HepGeom::Transform3D ALFA_DetectorFactory::UserTransformInStation(eRPotName eRPName)
{
	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_DetectorFactory::UserHepGeom::Transform3DInStation(eRPotName eRPName)");

	CLHEP::HepRep3x3 matRotation;
	USERTRANSFORM structUserTransform;

	switch(eRPName)
	{
		case ERPN_B7L1U:
			structUserTransform.fAngle = m_Config.vecTransformInStationB7L1U[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_Config.vecTransformInStationB7L1U[1], m_Config.vecTransformInStationB7L1U[2], m_Config.vecTransformInStationB7L1U[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_Config.vecTransformInStationB7L1U[4], m_Config.vecTransformInStationB7L1U[5], m_Config.vecTransformInStationB7L1U[6]);
			break;
		case ERPN_B7L1L:
			structUserTransform.fAngle = m_Config.vecTransformInStationB7L1L[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_Config.vecTransformInStationB7L1L[1], m_Config.vecTransformInStationB7L1L[2], m_Config.vecTransformInStationB7L1L[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_Config.vecTransformInStationB7L1L[4], m_Config.vecTransformInStationB7L1L[5], m_Config.vecTransformInStationB7L1L[6]);
			break;
		case ERPN_A7L1U:
			structUserTransform.fAngle = m_Config.vecTransformInStationA7L1U[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_Config.vecTransformInStationA7L1U[1], m_Config.vecTransformInStationA7L1U[2], m_Config.vecTransformInStationA7L1U[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_Config.vecTransformInStationA7L1U[4], m_Config.vecTransformInStationA7L1U[5], m_Config.vecTransformInStationA7L1U[6]);
			break;
		case ERPN_A7L1L:
			structUserTransform.fAngle = m_Config.vecTransformInStationA7L1L[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_Config.vecTransformInStationA7L1L[1], m_Config.vecTransformInStationA7L1L[2], m_Config.vecTransformInStationA7L1L[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_Config.vecTransformInStationA7L1L[4], m_Config.vecTransformInStationA7L1L[5], m_Config.vecTransformInStationA7L1L[6]);
			break;
		case ERPN_A7R1U:
			structUserTransform.fAngle = m_Config.vecTransformInStationA7R1U[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_Config.vecTransformInStationA7R1U[1], m_Config.vecTransformInStationA7R1U[2], m_Config.vecTransformInStationA7R1U[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_Config.vecTransformInStationA7R1U[4], m_Config.vecTransformInStationA7R1U[5], m_Config.vecTransformInStationA7R1U[6]);
			break;
		case ERPN_A7R1L:
			structUserTransform.fAngle = m_Config.vecTransformInStationA7R1L[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_Config.vecTransformInStationA7R1L[1], m_Config.vecTransformInStationA7R1L[2], m_Config.vecTransformInStationA7R1L[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_Config.vecTransformInStationA7R1L[4], m_Config.vecTransformInStationA7R1L[5], m_Config.vecTransformInStationA7R1L[6]);
			break;
		case ERPN_B7R1U:
			structUserTransform.fAngle = m_Config.vecTransformInStationB7R1U[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_Config.vecTransformInStationB7R1U[1], m_Config.vecTransformInStationB7R1U[2], m_Config.vecTransformInStationB7R1U[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_Config.vecTransformInStationB7R1U[4], m_Config.vecTransformInStationB7R1U[5], m_Config.vecTransformInStationB7R1U[6]);
			break;
		case ERPN_B7R1L:
			structUserTransform.fAngle = m_Config.vecTransformInStationB7R1L[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_Config.vecTransformInStationB7R1L[1], m_Config.vecTransformInStationB7R1L[2], m_Config.vecTransformInStationB7R1L[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_Config.vecTransformInStationB7R1L[4], m_Config.vecTransformInStationB7R1L[5], m_Config.vecTransformInStationB7R1L[6]);
			break;
		default:
			LogStream << MSG::WARNING << "Unknown Roman pot, station transformation will be set to default (zero) values" << endmsg;
			structUserTransform.fAngle = 0.0;
			structUserTransform.vecRotation = CLHEP::Hep3Vector(0.0, 0.0, 0.0);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(0.0, 0.0, 0.0);
	}

	double fPhi = structUserTransform.fAngle;

	CLHEP::Hep3Vector vRotation = CLHEP::Hep3Vector(structUserTransform.vecRotation);
	CLHEP::Hep3Vector vTranslation = CLHEP::Hep3Vector(structUserTransform.vecTranslation);

	matRotation.xx_ = vRotation.x()*vRotation.x()*(1 - cos(fPhi)) + cos(fPhi);
	matRotation.xy_ = vRotation.x()*vRotation.y()*(1 - cos(fPhi)) - vRotation.z()*sin(fPhi);
	matRotation.xz_ = vRotation.x()*vRotation.z()*(1 - cos(fPhi)) + vRotation.y()*sin(fPhi);

	matRotation.yx_ = vRotation.y()*vRotation.x()*(1 - cos(fPhi)) + vRotation.z()*sin(fPhi);
	matRotation.yy_ = vRotation.y()*vRotation.y()*(1 - cos(fPhi)) + cos(fPhi);
	matRotation.yz_ = vRotation.y()*vRotation.z()*(1 - cos(fPhi)) - vRotation.x()*sin(fPhi);

	matRotation.zx_ = vRotation.z()*vRotation.x()*(1 - cos(fPhi)) - vRotation.y()*sin(fPhi);
	matRotation.zy_ = vRotation.z()*vRotation.y()*(1 - cos(fPhi)) + vRotation.x()*sin(fPhi);
	matRotation.zz_ = vRotation.z()*vRotation.z()*(1 - cos(fPhi)) + cos(fPhi);

	return HepGeom::Transform3D(CLHEP::HepRotation(matRotation), vTranslation);
}

HepGeom::Transform3D ALFA_DetectorFactory::UserTransformInDetector(eRPotName eRPName)
{
	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_DetectorFactory::UserHepGeom::Transform3DInDetector(eRPotName eRPName)");

	CLHEP::HepRep3x3 matRotation;
	USERTRANSFORM structUserTransform;

	switch(eRPName)
	{
		case ERPN_B7L1U:
			structUserTransform.fAngle = m_Config.vecTransformInDetectorB7L1U[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_Config.vecTransformInDetectorB7L1U[1], m_Config.vecTransformInDetectorB7L1U[2], m_Config.vecTransformInDetectorB7L1U[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_Config.vecTransformInDetectorB7L1U[4], m_Config.vecTransformInDetectorB7L1U[5], m_Config.vecTransformInDetectorB7L1U[6]);
			break;
		case ERPN_B7L1L:
			structUserTransform.fAngle = m_Config.vecTransformInDetectorB7L1L[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_Config.vecTransformInDetectorB7L1L[1], m_Config.vecTransformInDetectorB7L1L[2], m_Config.vecTransformInDetectorB7L1L[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_Config.vecTransformInDetectorB7L1L[4], m_Config.vecTransformInDetectorB7L1L[5], m_Config.vecTransformInDetectorB7L1L[6]);
			break;
		case ERPN_A7L1U:
			structUserTransform.fAngle = m_Config.vecTransformInDetectorA7L1U[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_Config.vecTransformInDetectorA7L1U[1], m_Config.vecTransformInDetectorA7L1U[2], m_Config.vecTransformInDetectorA7L1U[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_Config.vecTransformInDetectorA7L1U[4], m_Config.vecTransformInDetectorA7L1U[5], m_Config.vecTransformInDetectorA7L1U[6]);
			break;
		case ERPN_A7L1L:
			structUserTransform.fAngle = m_Config.vecTransformInDetectorA7L1L[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_Config.vecTransformInDetectorA7L1L[1], m_Config.vecTransformInDetectorA7L1L[2], m_Config.vecTransformInDetectorA7L1L[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_Config.vecTransformInDetectorA7L1L[4], m_Config.vecTransformInDetectorA7L1L[5], m_Config.vecTransformInDetectorA7L1L[6]);
			break;
		case ERPN_A7R1U:
			structUserTransform.fAngle = m_Config.vecTransformInDetectorA7R1U[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_Config.vecTransformInDetectorA7R1U[1], m_Config.vecTransformInDetectorA7R1U[2], m_Config.vecTransformInDetectorA7R1U[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_Config.vecTransformInDetectorA7R1U[4], m_Config.vecTransformInDetectorA7R1U[5], m_Config.vecTransformInDetectorA7R1U[6]);
			break;
		case ERPN_A7R1L:
			structUserTransform.fAngle = m_Config.vecTransformInDetectorA7R1L[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_Config.vecTransformInDetectorA7R1L[1], m_Config.vecTransformInDetectorA7R1L[2], m_Config.vecTransformInDetectorA7R1L[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_Config.vecTransformInDetectorA7R1L[4], m_Config.vecTransformInDetectorA7R1L[5], m_Config.vecTransformInDetectorA7R1L[6]);
			break;
		case ERPN_B7R1U:
			structUserTransform.fAngle = m_Config.vecTransformInDetectorB7R1U[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_Config.vecTransformInDetectorB7R1U[1], m_Config.vecTransformInDetectorB7R1U[2], m_Config.vecTransformInDetectorB7R1U[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_Config.vecTransformInDetectorB7R1U[4], m_Config.vecTransformInDetectorB7R1U[5], m_Config.vecTransformInDetectorB7R1U[6]);
			break;
		case ERPN_B7R1L:
			structUserTransform.fAngle = m_Config.vecTransformInDetectorB7R1L[0];
			structUserTransform.vecRotation = CLHEP::Hep3Vector(m_Config.vecTransformInDetectorB7R1L[1], m_Config.vecTransformInDetectorB7R1L[2], m_Config.vecTransformInDetectorB7R1L[3]);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(m_Config.vecTransformInDetectorB7R1L[4], m_Config.vecTransformInDetectorB7R1L[5], m_Config.vecTransformInDetectorB7R1L[6]);
			break;
		default:
			LogStream << MSG::WARNING << "Unknown Roman pot, detector transformation will be set to default (zero) values" << endmsg;
			structUserTransform.fAngle = 0.0;
			structUserTransform.vecRotation = CLHEP::Hep3Vector(0.0, 0.0, 0.0);
			structUserTransform.vecTranslation = CLHEP::Hep3Vector(0.0, 0.0, 0.0);
	}

	double fPhi = structUserTransform.fAngle;

	CLHEP::Hep3Vector vRotation = CLHEP::Hep3Vector(structUserTransform.vecRotation);
	CLHEP::Hep3Vector vTranslation = CLHEP::Hep3Vector(structUserTransform.vecTranslation);

	matRotation.xx_ = vRotation.x()*vRotation.x()*(1 - cos(fPhi)) + cos(fPhi);
	matRotation.xy_ = vRotation.x()*vRotation.y()*(1 - cos(fPhi)) - vRotation.z()*sin(fPhi);
	matRotation.xz_ = vRotation.x()*vRotation.z()*(1 - cos(fPhi)) + vRotation.y()*sin(fPhi);

	matRotation.yx_ = vRotation.y()*vRotation.x()*(1 - cos(fPhi)) + vRotation.z()*sin(fPhi);
	matRotation.yy_ = vRotation.y()*vRotation.y()*(1 - cos(fPhi)) + cos(fPhi);
	matRotation.yz_ = vRotation.y()*vRotation.z()*(1 - cos(fPhi)) - vRotation.x()*sin(fPhi);

	matRotation.zx_ = vRotation.z()*vRotation.x()*(1 - cos(fPhi)) - vRotation.y()*sin(fPhi);
	matRotation.zy_ = vRotation.z()*vRotation.y()*(1 - cos(fPhi)) + vRotation.x()*sin(fPhi);
	matRotation.zz_ = vRotation.z()*vRotation.z()*(1 - cos(fPhi)) + cos(fPhi);

	return HepGeom::Transform3D(CLHEP::HepRotation(matRotation), vTranslation);
}

HepGeom::Point3D<double> ALFA_DetectorFactory::Point3DInDetector(eRPotName eRPName)
{
	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_DetectorFactory::Point3DInDetector(eRPotName eRPName)");
	HepGeom::Point3D<double> Point;

	switch(eRPName)
	{
		case ERPN_B7L1U:
			Point = HepGeom::Point3D<double>(m_Config.pointTransformInDetectorB7L1U[0], m_Config.pointTransformInDetectorB7L1U[1], m_Config.pointTransformInDetectorB7L1U[2]);
			break;
		case ERPN_B7L1L:
			Point = HepGeom::Point3D<double>(m_Config.pointTransformInDetectorB7L1L[0], m_Config.pointTransformInDetectorB7L1L[1], m_Config.pointTransformInDetectorB7L1L[2]);
			break;
		case ERPN_A7L1U:
			Point = HepGeom::Point3D<double>(m_Config.pointTransformInDetectorA7L1U[0], m_Config.pointTransformInDetectorA7L1U[1], m_Config.pointTransformInDetectorA7L1U[2]);
			break;
		case ERPN_A7L1L:
			Point = HepGeom::Point3D<double>(m_Config.pointTransformInDetectorA7L1L[0], m_Config.pointTransformInDetectorA7L1L[1], m_Config.pointTransformInDetectorA7L1L[2]);
			break;
		case ERPN_A7R1U:
			Point = HepGeom::Point3D<double>(m_Config.pointTransformInDetectorA7R1U[0], m_Config.pointTransformInDetectorA7R1U[1], m_Config.pointTransformInDetectorA7R1U[2]);
			break;
		case ERPN_A7R1L:
			Point = HepGeom::Point3D<double>(m_Config.pointTransformInDetectorA7R1L[0], m_Config.pointTransformInDetectorA7R1L[1], m_Config.pointTransformInDetectorA7R1L[2]);
			break;
		case ERPN_B7R1U:
			Point = HepGeom::Point3D<double>(m_Config.pointTransformInDetectorB7R1U[0], m_Config.pointTransformInDetectorB7R1U[1], m_Config.pointTransformInDetectorB7R1U[2]);
			break;
		case ERPN_B7R1L:
			Point = HepGeom::Point3D<double>(m_Config.pointTransformInDetectorB7R1L[0], m_Config.pointTransformInDetectorB7R1L[1], m_Config.pointTransformInDetectorB7R1L[2]);
			break;
		default:
			LogStream << MSG::WARNING << "Unknown Roman pot, transformation point will be set to default (zero) value" << endmsg;
	}

	return Point;
}

void ALFA_DetectorFactory::UpdateTransforms(PALIGNPARAMETERS pAlignParams)
{
	int i;
	StatusCode sc;
	char szLabel[32];
	RPPOSPARAMS RPosParams;
	eRPotName eRPName;
	StoredAlignX* pAlignX;
	GeoAlignableTransform* pAlTrans;
	list<eRPotName>::const_iterator iterRPName;
	MsgStream LogStream(Athena::getMessageSvc(), "ALFA_DetectorFactory::UpdateTransforms");

	if(m_eRequestedMetrologyType==EMT_SWCORRECTIONS){
		for(i=0;i<RPOTSCNT;i++){
			m_Config.GeometryConfig.CfgRPosParams[i].swcorr.fXOffset=pAlignParams->fXOffset[i];
			m_Config.GeometryConfig.CfgRPosParams[i].swcorr.fYOffset=pAlignParams->fYOffset[i];
			m_Config.GeometryConfig.CfgRPosParams[i].swcorr.fTheta=pAlignParams->fTheta[i];
		}

		if(ReadGeometry(true)){
			for(iterRPName=m_ListExistingRPots.begin();iterRPName!=m_ListExistingRPots.end();iterRPName++)
			{
				eRPName=*iterRPName;
				m_pGeoReader->GetRPPosParams(&RPosParams,eRPName);

				sprintf(szLabel,"AlTransRPBox[%02d]",eRPName);
				sc=m_pDetectorStore->retrieve(pAlignX,szLabel);
				if(sc==StatusCode::SUCCESS){
					pAlTrans=pAlignX->getAlignX();
					//pAlTrans->setDelta(RPosParams.RPSWTransformInStation);
					pAlTrans->setDelta(RPosParams.RPTransformInStation);
				}

				sprintf(szLabel,"AlTransDetInRPMainPoint[%02d]",eRPName);
				sc=m_pDetectorStore->retrieve(pAlignX,szLabel);
				if(sc==StatusCode::SUCCESS){
					pAlTrans=pAlignX->getAlignX();
					//pAlTrans->setDelta(RPosParams.DetSWTransformInMainPoint);
					pAlTrans->setDelta(RPosParams.DetTransformInMainPoint);
				}

			}
		}
	}
}
