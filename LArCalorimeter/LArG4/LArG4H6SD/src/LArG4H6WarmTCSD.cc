/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H6SD/LArG4H6WarmTCSD.h"
#include "CaloG4Sim/SimulationEnergies.h"

#include "G4VSensitiveDetector.hh"
#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ISvcLocator.h"

#include "G4HCofThisEvent.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4NavigationHistory.hh"
#include "G4VPhysicalVolume.hh"
#include "G4SDManager.hh"
#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4Step.hh"
#include "G4ios.hh"
#include <iomanip>

// for position checking
#include "G4Trap.hh"
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"

#undef DEBUG_ME

static FADS::SensitiveDetectorEntryT<LArG4H6WarmTCSD> WarmTC_SDX("LArG4H6WarmTC::X");
static FADS::SensitiveDetectorEntryT<LArG4H6WarmTCSD> WarmTC_SDY("LArG4H6WarmTC::Y");
static FADS::SensitiveDetectorEntryT<LArG4H6WarmTCSD> WarmTC_SDAbs("LArG4H6WarmTC::Abs");


LArG4H6WarmTCSD::LArG4H6WarmTCSD(G4String name) :FADS::FadsSensitiveDetector(name), m_Collection(0)
{
   if(name.find("::Abs")!=std::string::npos){
      collectionName.insert("WarmTCAbsCollection");
      m_isABS = true;
      m_isX = false;
   } else {
       if(name.find("::X")!=std::string::npos){
	  collectionName.insert("WarmTCSciXCollection");
	  m_isABS = false;
	  m_isX = true;
       } else {
	  collectionName.insert("WarmTCSciYCollection");
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
  if(name.find("::Cal")!=std::string::npos) {
     m_CalibSD = ((LArG4::CalibrationSensitiveDetector*)G4SDManager::GetSDMpointer()-> FindSensitiveDetector("TBEndcap::Dead"));
     if(!m_CalibSD) {
	std::cout<<"LArG4H6WarmTCSD::LArG4H6WarmTCSD: could not find SD:  TBEndcap::Dead !!!"<<std::endl;
     }
  }

  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap.h
  StatusCode status = svcLocator->service("MessageSvc", m_msgSvc);
  if(status.isFailure())
     throw std::runtime_error("LArG4H6BeamSD: Unable to retrieve Message Service!");

  MsgStream log(m_msgSvc, "LArG4H6BeamSD");
  status = svcLocator->service("StoreGateSvc", m_storeGate);
  if (status.isFailure()) {
     log << MSG::ERROR << " could not fetch the StoraGateSvc !!!" << endreq;
  }

}

LArG4H6WarmTCSD::~LArG4H6WarmTCSD(){
  // G4 handles the garbage collection
  //   if(m_Collection) delete m_Collection;
  //   if(m_CalibSD) delete m_CalibSD;
}

void LArG4H6WarmTCSD::Initialize(G4HCofThisEvent*)
{
#ifdef DEBUG_ME
   std::cout<<"LArG4H6WarmTCSD::LArG4H6WarmTCSD: "<<SensitiveDetectorName<<" initializing coll.: "<<collectionName[0]<<std::endl;
#endif
   m_Collection = new LArG4H6WarmTCHitCollection();
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
   edep  = aStep->GetTotalEnergyDeposit();
   if(edep == 0.) { 
      if(m_isCalib) CaloG4::SimulationEnergies::SetStepProcessed(); 
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

  int m_sampling;
  int m_etaBin; 
  int m_phiBin; 
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
  if(m_isABS) { m_sampling = copyModule; gran = 1; }
  else {
     switch(copyModule) {
        case 1:  { 
		 gran = 1;
		 if(m_isX)  m_sampling = copyModule; else m_sampling = copyModule + 1;
		 break; 
	         }
        case 2: case 3: { 
	   	    gran = 2; 
	  	    if(m_isX) m_sampling = 2*copyModule; else m_sampling = 2*copyModule - 1; 
		    break; 
		        }
        default: { m_sampling = -1; gran = 0; break; }
     }
  }
  
//#include "LArG4TBEndcap/LArTBEndcapWTC.icc"
  double WTC_sci_x = 190.0*CLHEP::mm;
  double WTC_sci_y = 1160.0*CLHEP::mm;
  double x_x = 6 * WTC_sci_x / 2;
  double x_y = WTC_sci_y / 2;

  if(m_isX) {
   m_etaBin = int((x_y - pinLocal.y()) / (5*gran*WTC_sci_x));
   m_phiBin = int((x_x - pinLocal.x()) / (gran*WTC_sci_x));
  } else {
   m_phiBin = int((x_y - pinLocal.y()) / (5*gran*WTC_sci_x));
   m_etaBin = int((x_x - pinLocal.x()) / (gran*WTC_sci_x));
  }

  addr = 100*m_sampling+10*m_etaBin+m_phiBin;
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

void LArG4H6WarmTCSD::EndOfEvent(G4HCofThisEvent* /*HCE*/)
{
 //G4int HCID;
 G4String cnam;
 /*
 G4SDManager *SDMAN = G4SDManager::GetSDMpointer();
 if(m_isABS)
   HCID = SDMAN->GetCollectionID(cnam="WarmTCAbsCollection");
 else
    if(m_isX) HCID = SDMAN->GetCollectionID(cnam="WarmTCSciXCollection");
    else HCID = SDMAN->GetCollectionID(cnam="WarmTCSciYCollection");
 */
 hitIt i;
 for (i = m_hits.begin(); i != m_hits.end(); i++ ) {
   //      m_Collection->insert( i->second );
    m_Collection->push_back(i->second);
 }
#ifdef DEBUG_ME
  std::cout<<"LArG4H6WarmTCSD::LArG4H6WarmTCSD: exporting: "<<collectionName[0]<<" and size: "<<m_Collection->size()<<std::endl;
#endif
  //HCE->AddHitsCollection(HCID, m_Collection );
  MsgStream log(m_msgSvc, "LArG4H6BeamSD");
   StatusCode status = m_storeGate->record(m_Collection,collectionName[0],false);
  if (status.isFailure()) {
         log << MSG::ERROR << "Failed to record LArG4H6WarmTCHitCollection  in StoreGate!" << endreq;
  }
  status = m_storeGate->setConst(m_Collection);
  if (status.isFailure()) {
         log  << MSG::ERROR << "Failed to lock LArG4H6WarmTCHitCollection  in StoreGate!" << endreq;
  }

 m_hits.clear();
}

