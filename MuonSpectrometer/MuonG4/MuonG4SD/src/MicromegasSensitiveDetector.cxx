/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonG4SD/MicromegasSensitiveDetector.h"
#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include "MuonSimEvent/MicromegasHitIdHelper.h"
#include "MCTruth/TrackHelper.h"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"

#include "G4Track.hh"

#include "GeoPrimitives/CLHEPtoEigenConverter.h"

#include <string>
#include <iostream>
#include <limits>

// Initialize static data
static FADS::SensitiveDetectorEntryT<MicromegasSensitiveDetector> mmsd("MicromegasSensitiveDetector");



// construction/destruction
MicromegasSensitiveDetector::MicromegasSensitiveDetector(std::string name)
  : FadsSensitiveDetector(name), m_GenericMuonHitCollection(0)
{
  //	ATH_MSG_INFO(" creating a MicromegasSensitiveDetector: "<<name);
  m_muonHelper = MicromegasHitIdHelper::GetHelper();
  m_muonHelper->PrintFields();
}


// Implemenation of memebr functions
void MicromegasSensitiveDetector::Initialize(G4HCofThisEvent*) 
{
  //m_GenericMuonHitCollection=new GenericMuonSimHitCollection("MicromegasSensitiveDetector");
  m_GenericMuonHitCollection = m_hitCollHelp.RetrieveNonconstCollection<GenericMuonSimHitCollection>("MicromegasSensitiveDetector");
}

G4bool MicromegasSensitiveDetector::ProcessHits(G4Step* aStep,G4TouchableHistory* /*ROHist*/) 
{
  G4Track* currentTrack = aStep->GetTrack();
  int charge=currentTrack->GetDefinition()->GetPDGCharge();
  bool geantinoHit = (currentTrack->GetDefinition()==G4Geantino::GeantinoDefinition()) ||
		     (currentTrack->GetDefinition()==G4ChargedGeantino::ChargedGeantinoDefinition());

  if (!charge && (!geantinoHit)) return false;
  //  ATH_MSG_INFO("\t\t MicromegasSD: Hit in a sensitive layer!!!!! ");
  const G4AffineTransform trans = currentTrack->GetTouchable()->GetHistory()->GetTopTransform(); // from global to local
  G4StepPoint* postStep=aStep->GetPostStepPoint();
  G4StepPoint* preStep=aStep->GetPreStepPoint();
  const G4Step* post_Step=aStep->GetTrack()->GetStep();

  Amg::Vector3D position = Amg::Hep3VectorToEigen( postStep->GetPosition() );
  Amg::Vector3D local_position = Amg::Hep3VectorToEigen( trans.TransformPoint( postStep->GetPosition() ) );
  
  Amg::Vector3D preposition = Amg::Hep3VectorToEigen( preStep->GetPosition() );
  Amg::Vector3D local_preposition = Amg::Hep3VectorToEigen( trans.TransformPoint( preStep->GetPosition() ) );
  
  int pdgCode=currentTrack->GetDefinition()->GetPDGEncoding();

  float globalTime=postStep->GetGlobalTime();
  float globalpreTime=preStep->GetGlobalTime();
  float eKin=postStep->GetKineticEnergy();

  Amg::Vector3D direction = Amg::Hep3VectorToEigen( postStep->GetMomentumDirection() );
  float depositEnergy=post_Step->GetTotalEnergyDeposit();
  float StepLength=post_Step->GetStepLength();
  
  if (depositEnergy<0.0001 && (!geantinoHit)) return false;
  
  G4TouchableHistory* touchHist = (G4TouchableHistory*)aStep->GetPreStepPoint()->GetTouchable();
  
  // int iDepth=touchHist->GetHistoryDepth();
  //  ATH_MSG_INFO("\t\t\t\t Touchable history dump ");
  int nLayer=touchHist->GetVolume(0)->GetCopyNo();
  std::string chName=touchHist->GetVolume(1)->GetLogicalVolume()->GetName();
  std::string subType=chName.substr(chName.find('-')+1);
  if (subType[0]!='M') ATH_MSG_INFO(" something is wrong, this is no Micromegas!");
  std::string temp(&subType[1]);
  std::istringstream is(temp);
  int iRing;
  is>>iRing;
  // identifiers have eta naming 0-1, eta encoded in subtype is 1-2
  iRing--;
  // double phiDiff=2*M_PI;

  G4ThreeVector posH=postStep->GetPosition(); //posH is equivalent to position - eigen not used to avoid additional dependence on EventPrimitives
  if (subType[2]=='L') posH.rotateZ(M_PI/8.);
  double phiHit=posH.phi();
  if(phiHit<=0) phiHit+=2.*M_PI;
  int iPhi=1+int(phiHit/(M_PI/4.));
  iPhi*=2;
  if (subType[2]=='L') iPhi-=1;
  
  int iSide=1;
  if (position.z()<0) iSide=-1;
  
  int mLayer= atoi((subType.substr(3,1)).c_str());
  if (mLayer != 1 && mLayer !=2) ATH_MSG_INFO(" something is wrong - multilayer index is " << mLayer );
  
  //  ATH_MSG_INFO("\t\t Chamber "<<chName<<" subType "<<subType<<" layer nr. "<<nLayer<<" ring "<<iRing<<" sector "<<iPhi<<" side "<<iSide);
  int MmId = m_muonHelper->BuildMicromegasHitId(subType, iPhi, iRing, mLayer,nLayer, iSide);
 
  TrackHelper trHelp(aStep->GetTrack());
  int barcode = trHelp.GetBarcode();

  GenericMuonSimHit* aHit=new GenericMuonSimHit(MmId, globalTime,globalpreTime,position,local_position,preposition,local_preposition,pdgCode,eKin,direction,depositEnergy,StepLength,barcode);

  //    ATH_MSG_INFO("MMs "<<m_muonHelper->GetStationName(MmId)
  // 	            << " "<<m_muonHelper->GetFieldValue("PhiSector")
  // 	            << " "<<m_muonHelper->GetFieldValue("ZSector")
  // 	            << " "<<m_muonHelper->GetFieldValue("MultiLayer")
  // 	            << " "<<m_muonHelper->GetFieldValue("Layer")
  // 	            << " "<<m_muonHelper->GetFieldValue("Side"));  	    

  //ATH_MSG_INFO(m_muonHelper->GetStationName(MmId)<<" "<<aHit->print());
  //  ATH_MSG_INFO(aHit->print());
  m_GenericMuonHitCollection->Insert(*aHit);
  delete aHit;

  return true;
}


void MicromegasSensitiveDetector::EndOfEvent(G4HCofThisEvent* /*HCE*/) 
{
  if (!m_allowMods)
    m_hitCollHelp.SetConstCollection<GenericMuonSimHitCollection>(m_GenericMuonHitCollection);
} 


