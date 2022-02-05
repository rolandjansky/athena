/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "sTGCSensitiveDetector.h"
#include "MuonSimEvent/sTgcHitIdHelper.h"
#include "MCTruth/TrackHelper.h"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"

#include "G4Track.hh"

#include "GeoPrimitives/CLHEPtoEigenConverter.h"

#include <string>

// construction/destruction
sTGCSensitiveDetector::sTGCSensitiveDetector(const std::string& name, const std::string& hitCollectionName)
  : G4VSensitiveDetector( name )
  , m_sTGCSimHitCollection( hitCollectionName )
{
  m_muonHelper = sTgcHitIdHelper::GetHelper();
  //m_muonHelper->PrintFields();
}

// Implemenation of memebr functions
void sTGCSensitiveDetector::Initialize(G4HCofThisEvent*)
{
  if (!m_sTGCSimHitCollection.isValid()) m_sTGCSimHitCollection = std::make_unique<sTGCSimHitCollection>();
}

G4bool sTGCSensitiveDetector::ProcessHits(G4Step* aStep,G4TouchableHistory* /*ROHist*/)
{
  G4Track* currentTrack = aStep->GetTrack();
  int charge=currentTrack->GetDefinition()->GetPDGCharge();

  bool geantinoHit = (currentTrack->GetDefinition()==G4Geantino::GeantinoDefinition()) ||
                     (currentTrack->GetDefinition()==G4ChargedGeantino::ChargedGeantinoDefinition());

  if (!charge && (!geantinoHit)) return false;
  //  G4cout << "\t\t sTGCSD: Hit in a sensitive layer!!!!! " << G4endl;
  G4StepPoint* postStep=aStep->GetPostStepPoint();
  const G4Step* post_Step=aStep->GetTrack()->GetStep();

  Amg::Vector3D position = Amg::Hep3VectorToEigen(postStep->GetPosition());
  
  G4StepPoint* preStep = aStep->GetPreStepPoint();
  Amg::Vector3D preposition = Amg::Hep3VectorToEigen(preStep->GetPosition());

  int pdgCode=currentTrack->GetDefinition()->GetPDGEncoding();

  float globalTime=postStep->GetGlobalTime();

  Amg::Vector3D direction= Amg::Hep3VectorToEigen( postStep->GetMomentumDirection() );
  float depositEnergy=post_Step->GetTotalEnergyDeposit();

  if (depositEnergy<0.0001 && (!geantinoHit)) return false;

  G4TouchableHistory* touchHist = (G4TouchableHistory*)aStep->GetPreStepPoint()->GetTouchable();

  // int iDepth=touchHist->GetHistoryDepth();
  //  G4cout << "\t\t\t\t Touchable history dump "<<G4endl;
  int nLayer=touchHist->GetVolume(0)->GetCopyNo();
  std::string chName=touchHist->GetVolume(1)->GetLogicalVolume()->GetName();
  //G4cout << "sTGCSensitiveDetector name: "<<chName<<G4endl;
  std::string subType=chName.substr(chName.find('-')+1);
  //G4cout << "\t\t sType: "<<subType);
  if (subType[0]!='T'&&subType[0]!='Q' ) G4cout << " something is wrong, this is no sTGC!"<<G4endl;
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

  if (mLayer != 1 && mLayer !=2) G4cout << " something is wrong - multilayer index is " << mLayer << G4endl;

  int sTgcId = m_muonHelper->BuildsTgcHitId(subType, iPhi, iRing, mLayer,nLayer, iSide);
  TrackHelper trHelp(aStep->GetTrack());
  m_sTGCSimHitCollection->Emplace(sTgcId,globalTime,position,pdgCode,direction,depositEnergy,
                                  trHelp.GetParticleLink(),
                                  preStep->GetKineticEnergy(),preposition);

  return true;
}

