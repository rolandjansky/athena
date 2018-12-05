/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H6WarmTCSD.h"
#include "CaloG4Sim/SimulationEnergies.h"

#include "MCTruth/EventInformation.h"

#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4NavigationHistory.hh"
#include "G4VPhysicalVolume.hh"
#include "G4SDManager.hh"
#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4Step.hh"
#include "G4ios.hh"
#include "G4RunManager.hh"
#include <iomanip>

// for position checking
#include "G4Trap.hh"
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"

#include "CxxUtils/make_unique.h" // For make unique

#undef DEBUG_ME

LArG4H6WarmTCSD::LArG4H6WarmTCSD(G4String name, G4String colname)
  : G4VSensitiveDetector(name)
  , m_Collection(colname)
  , m_CalibSD(0)
{
  if(name.find("::Abs")!=std::string::npos){
    m_isABS = true;
    m_isX = false;
  } else {
    if(name.find("::X")!=std::string::npos){
      m_isABS = false;
      m_isX = true;
    } else {
      m_isABS = false;
      m_isX = false;
    }
  }
  if(name.find("::Calib")!=std::string::npos){
    m_isCalib = true;
  } else {
    m_isCalib = false;
  }

#ifdef DEBUG_ME
  std::cout<<"LArG4H6WarmTCSD::LArG4H6WarmTCSD: creating: "<<name<<std::endl;
#endif
  if(m_isCalib) {
    m_CalibSD = ((LArG4CalibSD*)G4SDManager::GetSDMpointer()-> FindSensitiveDetector("TBEndcap::Dead"));
    if(!m_CalibSD) {
      std::cout << "LArG4H6WarmTCSD::LArG4H6WarmTCSD: could not find SD:  TBEndcap::Dead !!!" << std::endl;
      throw;
    }
  }

}

void LArG4H6WarmTCSD::Initialize(G4HCofThisEvent*)
{
#ifdef DEBUG_ME
  std::cout<<"LArG4H6WarmTCSD::LArG4H6WarmTCSD: "<<SensitiveDetectorName<<" initializing coll.: "<<collectionName[0]<<std::endl;
#endif
  if (!m_Collection.isValid()) m_Collection = CxxUtils::make_unique<LArG4H6WarmTCHitCollection>();
#ifdef DEBUG_ME
  std::cout<<"LArG4H6WarmTCSD::LArG4H6WarmTCSD: initialized "<<collectionName[0]<<" with HCID: "<<G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0])<<std::endl;
#endif
}

G4bool LArG4H6WarmTCSD::ProcessHits(G4Step* aStep,G4TouchableHistory* ROhist)
{
  static G4double edep;
  static LArG4H6WarmTCHit* theHit;

#ifdef DEBUG_ME
  std::cout<<"LArG4H6WarmTCSD::LArG4H6WarmTCSD: processing: "<<this->GetName()<<std::endl;
#endif
  edep  = aStep->GetTotalEnergyDeposit() * aStep->GetTrack()->GetWeight();
  if(edep == 0.) {
    if(m_isCalib){
      EventInformation * event_info = dynamic_cast<EventInformation*>(G4RunManager::GetRunManager()->GetCurrentEvent()->GetUserInformation());
      if ( event_info ) {
        // Update the step info
        event_info->SetLastProcessedBarcode( aStep->GetTrack()->GetTrackID() );
        event_info->SetLastProcessedStep( aStep->GetTrack()->GetCurrentStepNumber() );
      }
    }
    return true;
  }


  G4StepPoint* pre_step_point = aStep->GetPreStepPoint();
  G4StepPoint* post_step_point = aStep->GetPostStepPoint();
  G4ThreeVector startPoint = pre_step_point->GetPosition();
  G4ThreeVector endPoint   = post_step_point->GetPosition();
  G4ThreeVector p = (startPoint + endPoint) * 0.5;
  //  Get local coordinates of the step, independently of how it was positioned  in World
  const G4AffineTransform transformation =
    pre_step_point->GetTouchable()->GetHistory()->GetTopTransform();
  G4ThreeVector startPointinLocal = transformation.TransformPoint(startPoint);
  G4ThreeVector   endPointinLocal = transformation.TransformPoint  (endPoint);
  G4ThreeVector          pinLocal =(startPointinLocal+endPointinLocal)*0.5;
  //
  //  G4cout<<"LArTBEndcapWarmTCCalculator::Local point: "<<pinLocal.x()<<" "<<pinLocal.y()<<" "<<pinLocal.z()<<std::endl;

  int sampling;
  int etaBin;
  int phiBin;
  int addr;

  // We can extract our position from the copy number of depth and module
  // First have touchable
  // G4TouchableHistory* theTouchable = (G4TouchableHistory*) (pre_step_point->GetTouchable());
  // Volume name
  G4String hitVolume = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetName();
  // And copy number
  G4int copyModule = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo();

  G4int gran;
  // Sampling Identifier
  if(m_isABS) { sampling = copyModule; gran = 1; }
  else {
    switch(copyModule) {
    case 1:  {
      gran = 1;
      if(m_isX)  sampling = copyModule; else sampling = copyModule + 1;
      break;
    }
    case 2: case 3: {
      gran = 2;
      if(m_isX) sampling = 2*copyModule; else sampling = 2*copyModule - 1;
      break;
    }
    default: { sampling = -1; gran = 0; break; }
    }
  }

  //#include "LArG4TBEndcap/LArTBEndcapWTC.icc"
  double WTC_sci_x = 190.0*CLHEP::mm;
  double WTC_sci_y = 1160.0*CLHEP::mm;
  double x_x = 6 * WTC_sci_x / 2;
  double x_y = WTC_sci_y / 2;

  if(m_isX) {
    etaBin = int((x_y - pinLocal.y()) / (5*gran*WTC_sci_x));
    phiBin = int((x_x - pinLocal.x()) / (gran*WTC_sci_x));
  } else {
    phiBin = int((x_y - pinLocal.y()) / (5*gran*WTC_sci_x));
    etaBin = int((x_x - pinLocal.x()) / (gran*WTC_sci_x));
  }

  addr = 100*sampling+10*etaBin+phiBin;
  if(m_isABS) addr *= -1;

#ifdef DEBUG_ME
  std::cout<<"LArG4H6WarmTCSD::LArG4H6WarmTCSD: processed addr: "<<addr<<" en.: "<<edep<<std::endl;
#endif
  hitIt it;
  if((it = m_hits.find(addr)) == m_hits.end()) { // insert the new hit
    theHit = new LArG4H6WarmTCHit(addr, edep);
    m_hits.insert(hitPair(addr,theHit));
  } else { // Adding the energy
    (it->second)->AddEnergy(edep);
  }
  if(m_isCalib) {
    m_CalibSD->ProcessHits(aStep,ROhist);
  }


  return true;
}

void LArG4H6WarmTCSD::EndOfAthenaEvent()
{
  //G4int HCID;
  G4String cnam;
  hitIt i;
  for (i = m_hits.begin(); i != m_hits.end(); i++ ) {
    //      m_Collection->insert( i->second );
    m_Collection->push_back(i->second);
  }
  m_hits.clear();
}
