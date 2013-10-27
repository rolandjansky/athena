/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class PhantomBarrelSD.
// Sensitive detector for the phantom calorimeter in combined 2004
//
// Author: franck Martin <Franck.Martin@cern.ch>
// december 15th, 2003.
//
//************************************************************

#include "PhantomCalorimeter/PhantomBarrelSD.hh"
#include "PhantomCalorimeter/PhantomBarrelSDMessenger.hh"
#include "PhantomCalorimeter/PhantomBarrelDescription.hh"

#include "TileG4DetDescr/DetectorDescriptionStore.h"
#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include "FadsSensitiveDetector/SensitiveDetectorCatalog.h"

#include "G4HCofThisEvent.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"
#include "G4TransportationManager.hh"
#include "G4Navigator.hh"
#include "G4Trd.hh"

#include "G4UImanager.hh"
#include "G4ios.hh"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "CaloIdentifier/TileTBID.h"
#include "TileSimEvent/TileHitVector.h"

static SensitiveDetectorEntryT<PhantomBarrelSD> phantombarrelSD("PhantomBarrelSD");

PhantomBarrelSD::PhantomBarrelSD(G4String name)   
  : FadsSensitiveDetector(name),
    detectorDescription(0),    
    phantombarrelSDMessenger(0),
    m_iamowner(false)
{
  phantombarrelSDMessenger = new PhantomBarrelSDMessenger(this);
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode status = svcLocator->service("MessageSvc", m_msgSvc);
  MsgStream log(m_msgSvc, "PhantomBarrelSD");

  if(status.isFailure())
    log << MSG::FATAL << "MessageSvc not found!" << endreq;
  else
    log << MSG::DEBUG << "MessageSvc initialized" << endreq;

  status = svcLocator->service("StoreGateSvc", m_sgSvc);
  if(status.isFailure())
    log << MSG::FATAL << "StoreGateSvc not found!" << endreq;
  else
    log << MSG::DEBUG << "StoreGateSvc initialized" << endreq;

  status = svcLocator->service("DetectorStore", m_detStore);
  if(status.isFailure())
    log << MSG::FATAL << "DetectorStoreSvc not found!" << endreq;
  else
    log << MSG::DEBUG << "DetectorStoreSvc initialized" << endreq;

  status = m_detStore->retrieve(m_tileTBID);
  if(status.isFailure())
    log << MSG::FATAL << "No TileTBID helper" << endreq;
  else
    log << MSG::DEBUG << "TileTBID helper retrieved" << endreq;

  int type=TileTBID::ADC_TYPE, module=TileTBID::PHANTOM_CALO;
  for (int channel=0; channel<nCell; ++channel) {
    m_id[channel] = m_tileTBID->channel_id(type,module,channel);
  }
}

PhantomBarrelSD::~PhantomBarrelSD()
{
  delete phantombarrelSDMessenger;
}

void PhantomBarrelSD::Description(std::string descr_name)
{
  MsgStream log(m_msgSvc, "PhantomBarrelSD"); 
  log << MSG::DEBUG << "PhantomBarrelSD, Description" << endreq; 
  DetectorDescriptionStore *st=DetectorDescriptionStore::GetDetectorDescriptionStore();
  DetectorDescription *dd=st->GetDetectorDescription(descr_name);
  PhantomBarrelDescription* i=dynamic_cast<PhantomBarrelDescription *>(dd);
  if (i)
    detectorDescription = i;
  else
    log << MSG::FATAL << "Description" << descr_name << "is not of right type PhantomBarrelDescription!" << endreq;
}

void PhantomBarrelSD::Initialize(G4HCofThisEvent* /* HCE */)
{
  MsgStream log(m_msgSvc, "PhantomBarrelSD"); 
  log << MSG::DEBUG << "Initializing SD" << endreq;

  memset(m_nhits,0,sizeof(m_nhits));
}

G4bool PhantomBarrelSD::ProcessHits(G4Step* aStep,G4TouchableHistory* /* ROhist */)
{
  MsgStream log(m_msgSvc, "PhantomBarrelSD:: ProcessHits");
  MSG::Level logLevel = log.level();
  if (logLevel <= MSG::VERBOSE)
    log << MSG::VERBOSE << "i am in PhantomBarrel process hit" << endreq; 

  G4TouchableHistory* theTouchable = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());
  G4VPhysicalVolume* physVol = theTouchable->GetVolume();
  G4LogicalVolume* logiVol   = physVol->GetLogicalVolume();
  G4String nameLogiVol       = logiVol->GetName();
  G4int nScinti              = physVol->GetCopyNo();

  G4double edep  = aStep->GetTotalEnergyDeposit();
  G4double stepl = 0.;
  G4ThreeVector pStep = aStep->GetDeltaPosition();
  
  if (aStep->GetTrack()->GetDefinition()->GetPDGCharge() != 0.) stepl = aStep->GetStepLength();
    
  if ((edep==0.)&&(stepl==0.)) return false;
  if (logLevel <= MSG::VERBOSE)
    log << MSG::VERBOSE << "Where " << nameLogiVol      << endreq; 

  if(nameLogiVol.find("ScintillatorLayer") !=G4String::npos) 
  {
    int ind;
    G4double m_Scinti = nScinti / 4.;

    if(m_Scinti<=1)                 ind=0;
    if(m_Scinti<=2 && m_Scinti>1)   ind=1;
    if(m_Scinti<=4 && m_Scinti>2)   ind=2;
    if(m_Scinti<=6 && m_Scinti>4)   ind=3;
    if(m_Scinti<=8 && m_Scinti>6)   ind=4;
    if(m_Scinti<=10 && m_Scinti>8)  ind=5;
    if(m_Scinti<=12 && m_Scinti>10) ind=6;
    if(m_Scinti>12)                 ind=7;

    if ( m_nhits[ind] > 0 )
    { 
      if (logLevel <= MSG::VERBOSE)
        log << MSG::VERBOSE << "Additional hit in cell " << ind
            << " ene=" << edep << endreq;
      m_hit[ind]->add(edep,0.0,0.0);
    } else
    {
      // First hit in a cell
      if (logLevel <= MSG::VERBOSE)
        log << MSG::VERBOSE << "First hit in cell " << ind
            << " ene=" << edep << endreq;
      m_hit[ind] = new TileSimHit(m_id[ind],edep,0.0,0.0);  
    }
    ++m_nhits[ind];
  }  
  return true;
}

void PhantomBarrelSD::EndOfEvent(G4HCofThisEvent* /* HCE */)
{
  MsgStream log(m_msgSvc, "PhantomBarrelSD");

  TileHitVector* ExtraHitsCollection;   

  if (! m_iamowner) {
      const TileHitVector* ExtraColl;
      StatusCode status = m_sgSvc->retrieve(ExtraColl,"TileTBHits");
      m_iamowner = (status.isFailure());
      ExtraHitsCollection = (TileHitVector *)ExtraColl;
  }

  if (m_iamowner) {
    ExtraHitsCollection = new TileHitVector("TileTBHits"); 
    StatusCode status = m_sgSvc->record(ExtraHitsCollection,ExtraHitsCollection->Name(),true);
    if (status.isFailure())
      log << MSG::FATAL << "Can't initialize TileTBHits vector" << endreq; 
    else
      log << MSG::DEBUG << "I have initialized TileTBHits vector" << endreq; 
  }
  
  for (int ind=0; ind<nCell; ++ind) {
    int nhit= m_nhits[ind];
    if (nhit>0) {
      log << MSG::DEBUG
          << "Cell id=" << m_tileTBID->to_string(m_id[ind])
          << " nhit=" << nhit << " ene=" << m_hit[ind]->energy()
          << endreq;
      ExtraHitsCollection->Insert(TileHit(m_hit[ind]));
      delete m_hit[ind];
    } else {
      log << MSG::VERBOSE
          << "Cell id=" << m_tileTBID->to_string(m_id[ind])
          << " nhit=0"  << endreq;
    }
  }

  log << MSG::DEBUG << "Total number of hits is " << ExtraHitsCollection->size() << endreq;

  if (m_iamowner) {
    StatusCode status = m_sgSvc->setConst(ExtraHitsCollection);
    if(status.isFailure())
      log << MSG::ERROR << "Can't lock Collection 'TileTBHits'" << endreq;
    else
      log << MSG::DEBUG << "Collection 'TileTBHits' is locked " << endreq;
  }
}

