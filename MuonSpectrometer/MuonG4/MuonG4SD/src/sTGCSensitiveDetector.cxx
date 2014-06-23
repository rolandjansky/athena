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
  //	std::cout<< " creating a sTGCSensitiveDetector: "<<name<<std::endl;
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
  //  std::cout << "\t\t sTGCSD: Hit in a sensitive layer!!!!! "<<std::endl;
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
  //  std::cout<<"\t\t\t\t Touchable history dump "<<std::endl;
  int nLayer=touchHist->GetVolume(0)->GetCopyNo();
  std::string chName=touchHist->GetVolume(1)->GetLogicalVolume()->GetName();
  std::string subType=chName.substr(chName.find('-')+1);
  if (subType[0]!='T') std::cout<<" something is wrong, this is no sTGC!"<<std::endl;
  std::string temp(&subType[1]);
  std::istringstream is(temp);
  int iRing;
  is>>iRing;
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
  if (mLayer != 1 && mLayer !=2) std::cout << " something is wrong - multilayer index is " << mLayer << std::endl;
  
  //  std::cout<<"\t\t Chamber "<<chName<<" subType "<<subType<<" layer nr. "<<nLayer<<" ring "<<iRing<<" sector "<<iPhi<<" side "<<iSide<<std::endl;
  int sTgcId = m_muonHelper->BuildsTgcHitId(subType, iPhi, iRing, mLayer,nLayer, iSide); 
  TrackHelper trHelp(aStep->GetTrack());
  int barcode = trHelp.GetBarcode();

  GenericMuonSimHit* aHit=new GenericMuonSimHit(sTgcId,globalTime,globalpreTime,position,local_position,preposition,local_preposition,pdgCode,eKin,direction,depositEnergy,StepLength,barcode);

  //    std::cout<<"sTGC "<<m_muonHelper->GetStationName(sTgcId);	
  // 	std::cout<<" "<<m_muonHelper->GetFieldValue("PhiSector");
  // 	std::cout<<" "<<m_muonHelper->GetFieldValue("ZSector");
  // 	std::cout<<" "<<m_muonHelper->GetFieldValue("MultiLayer");
  // 	std::cout<<" "<<m_muonHelper->GetFieldValue("Layer");
  // 	std::cout<<" "<<m_muonHelper->GetFieldValue("Side")<<std::endl;  	    

  //std::cout<<m_muonHelper->GetStationName(sTgcId)<<" "<<aHit->print()<<std::endl;
  //  std::cout<<aHit->print()<<std::endl;
  m_GenericMuonHitCollection->Insert(*aHit);
  delete aHit;

  return true;
}


void sTGCSensitiveDetector::EndOfEvent(G4HCofThisEvent* /*HCE*/) 
{
  if(!m_allowMods)
    m_hitCollHelp.SetConstCollection(m_GenericMuonHitCollection);
} 


