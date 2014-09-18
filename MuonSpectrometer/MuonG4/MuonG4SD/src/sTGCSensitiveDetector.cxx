/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonG4SD/sTGCSensitiveDetector.h"
#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include "MuonSimEvent/sTgcHitIdHelper.h"
#include "MCTruth/TrackHelper.h"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"

#include "G4Track.hh"

#include "GeoPrimitives/CLHEPtoEigenConverter.h"

#include <string>
#include <iostream>
#include <limits>

// Initialize static data
static FADS::SensitiveDetectorEntryT<sTGCSensitiveDetector> stgcsd("sTGCSensitiveDetector");



// construction/destruction
sTGCSensitiveDetector::sTGCSensitiveDetector(std::string name)
  : FadsSensitiveDetector(name), m_GenericMuonHitCollection(0)
{
  //	ATH_MSG_INFO(" creating a sTGCSensitiveDetector: "<<name);
  m_muonHelper = sTgcHitIdHelper::GetHelper();
  m_muonHelper->PrintFields();
}


// Implemenation of memebr functions
void sTGCSensitiveDetector::Initialize(G4HCofThisEvent*) 
{
  //m_GenericMuonHitCollection = new GenericMuonSimHitCollection("sTGCSensitiveDetector");
  m_GenericMuonHitCollection = m_hitCollHelp.RetrieveNonconstCollection<GenericMuonSimHitCollection>("sTGCSensitiveDetector");
}

G4bool sTGCSensitiveDetector::ProcessHits(G4Step* aStep,G4TouchableHistory* /*ROHist*/) 
{
  G4Track* currentTrack = aStep->GetTrack();
  int charge=currentTrack->GetDefinition()->GetPDGCharge();

  bool geantinoHit = (currentTrack->GetDefinition()==G4Geantino::GeantinoDefinition()) ||
		     (currentTrack->GetDefinition()==G4ChargedGeantino::ChargedGeantinoDefinition());

  if (!charge && (!geantinoHit)) return false;
  //  ATH_MSG_INFO("\t\t sTGCSD: Hit in a sensitive layer!!!!! ");
  const G4AffineTransform trans = currentTrack->GetTouchable()->GetHistory()->GetTopTransform(); // from global to local
  G4StepPoint* postStep=aStep->GetPostStepPoint();
  G4StepPoint* preStep=aStep->GetPreStepPoint();
  const G4Step* post_Step=aStep->GetTrack()->GetStep();

  Amg::Vector3D position = Amg::Hep3VectorToEigen(postStep->GetPosition());
  Amg::Vector3D local_position = Amg::Hep3VectorToEigen( trans.TransformPoint( postStep->GetPosition() ) );
  
  Amg::Vector3D preposition = Amg::Hep3VectorToEigen( preStep->GetPosition() );
  Amg::Vector3D local_preposition = Amg::Hep3VectorToEigen( trans.TransformPoint( preStep->GetPosition() ) );
  
  int pdgCode=currentTrack->GetDefinition()->GetPDGEncoding();

  float globalTime=postStep->GetGlobalTime();
  float globalpreTime=preStep->GetGlobalTime();
  float eKin=postStep->GetKineticEnergy();

  Amg::Vector3D direction= Amg::Hep3VectorToEigen( postStep->GetMomentumDirection() );
  float depositEnergy=post_Step->GetTotalEnergyDeposit();
  float StepLength=post_Step->GetStepLength();
  
  if (depositEnergy<0.0001 && (!geantinoHit)) return false;
  
  G4TouchableHistory* touchHist = (G4TouchableHistory*)aStep->GetPreStepPoint()->GetTouchable();
   
  // int iDepth=touchHist->GetHistoryDepth();
  //  ATH_MSG_INFO("\t\t\t\t Touchable history dump ");
  int nLayer=touchHist->GetVolume(0)->GetCopyNo();
  std::string chName=touchHist->GetVolume(1)->GetLogicalVolume()->GetName();
  //ATH_MSG_INFO("sTGCSensitiveDetector name: "<<chName);
  std::string subType=chName.substr(chName.find('-')+1);
  //ATH_MSG_INFO("\t\t sType: "<<subType);
  if (subType[0]!='T'&&subType[0]!='Q' ) ATH_MSG_INFO(" something is wrong, this is no sTGC!");
  std::string temp(&subType[2]);
  std::istringstream is(temp);
  int iRing;
  is>>iRing;
  // identifiers have eta naming 0-2, eta encoded in subtype is 1-3
  iRing--;
  // double phiDiff=2*M_PI;
  
  G4ThreeVector posH=postStep->GetPosition(); //posH is equivalent to position - eigen not used to avoid additional dependence on EventPrimitives
  if (subType[1]=='L') posH.rotateZ(M_PI/8.);
  double phiHit=posH.phi();
  if(phiHit<=0) phiHit+=2.*M_PI;
  int iPhi=1+int(phiHit/(M_PI/4.));
  iPhi*=2;
  if (subType[1]=='L') iPhi-=1;                                                                      

  int iSide=1;
  if (position.z()<0) iSide=-1;
  
  int mLayer=0;
  if (subType[1]=='S')
  {
  	if (subType[3]=='C') mLayer=1;
	else if (subType[3]=='P') mLayer=2;
  }
  else if (subType[1]=='L')
  {
  	if (subType[3]=='P') mLayer=1;
	else if (subType[3]=='C') mLayer=2;
  }
  
  if (mLayer != 1 && mLayer !=2) ATH_MSG_INFO(" something is wrong - multilayer index is " << mLayer );
  
  //  ATH_MSG_INFO("\t\t Chamber "<<chName<<" subType "<<subType<<" layer nr. "<<nLayer<<" ring "<<iRing<<" sector "<<iPhi<<" side "<<iSide);
  int sTgcId = m_muonHelper->BuildsTgcHitId(subType, iPhi, iRing, mLayer,nLayer, iSide); 
  TrackHelper trHelp(aStep->GetTrack());
  int barcode = trHelp.GetBarcode();

  GenericMuonSimHit* aHit=new GenericMuonSimHit(sTgcId,globalTime,globalpreTime,position,local_position,preposition,local_preposition,pdgCode,eKin,direction,depositEnergy,StepLength,barcode);

  //    ATH_MSG_INFO("sTGC "<<m_muonHelper->GetStationName(sTgcId)
  // 	            << " "<<m_muonHelper->GetFieldValue("PhiSector")
  // 	            << " "<<m_muonHelper->GetFieldValue("ZSector")
  // 	            << " "<<m_muonHelper->GetFieldValue("MultiLayer")
  // 	            << " "<<m_muonHelper->GetFieldValue("Layer")
  // 	            << " "<<m_muonHelper->GetFieldValue("Side"))  	    

  //ATH_MSG_INFO(<m_muonHelper->GetStationName(sTgcId)<<" "<<aHit->print());
  //  ATH_MSG_INFO(aHit->print());
  m_GenericMuonHitCollection->Insert(*aHit);
  delete aHit;

  return true;
}


void sTGCSensitiveDetector::EndOfEvent(G4HCofThisEvent* /*HCE*/) 
{
  if(!m_allowMods)
    m_hitCollHelp.SetConstCollection(m_GenericMuonHitCollection);
} 


