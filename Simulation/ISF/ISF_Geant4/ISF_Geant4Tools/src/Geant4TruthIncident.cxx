/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Geant4TruthIncident.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "Geant4TruthIncident.h"

// Atlas G4 Helpers
#include "MCTruth/EventInformation.h"
#include "MCTruth/TrackBarcodeInfo.h"
#include "MCTruth/TrackHelper.h"
#include "MCTruth/TrackInformation.h"
#include "SimHelpers/SecondaryTracksHelper.h"

// Units
#include "GaudiKernel/PhysicalConstants.h"

// HepMC includes
#include "HepMC/GenParticle.h"

// Geant4 includes
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ThreeVector.hh"
#include "G4VProcess.hh"

#include "G4TrackStatus.hh"
#include "G4ProcessType.hh"
#include "G4EmProcessSubType.hh"
#include "G4DynamicParticle.hh"
#include "G4PrimaryParticle.hh"

#include "G4EventManager.hh"
#include "G4Event.hh"

/*
  Comments:
  what about parent particle surviving (e.g. bremstrahlung)


  Existing code:
  Simulation/G4Sim/SimHelpers/src/StepHelper.cxx
  - provids convenient information of a G4Step
  Simulation/G4Sim/SimHelpers/src/SecondaryTracksHelper.cxx
  - provids convenient information about secondaries produced in (last) G4Step
  Simulation/G4Utilities/G4TruthStrategies/src/BremsstrahlungStrategy.cxx
  - retrives information from a G4Step (via StepHelper)
  Simulation/G4Sim/MCTruth/MCTruth/TruthStrategy.h
  - common base for different truth strategies
  Simulation/G4Sim/MCTruth/src/EventInformation.cxx
  - stores HepMCevent in G4
  Simulation/G4Sim/MCTruth/src/TrackInformation.cxx
  Simulation/G4Sim/MCTruth/src/TrackHelper.cxx
  - store/manage barcode

  Simulation/G4Sim/MCTruth/src/AtlasTrajectory.cxx
  - setup SecondaryTrackHelper to provide secondaries to truth
*/


ISF::Geant4TruthIncident::Geant4TruthIncident(const G4Step *step, AtlasDetDescr::AtlasRegion geoID, int numChildren, SecondaryTracksHelper &sHelper) :
  ITruthIncident(geoID, numChildren),
  m_positionSet(false),
  m_position(),
  m_step(step),
  m_sHelper( sHelper),
  m_childrenPrepared(false),
  m_children(),
  m_parentParticleAfterIncident(0)
{
}

const HepMC::FourVector& ISF::Geant4TruthIncident::position() const {
  if (!m_positionSet) {
    // post step processes:
    const G4StepPoint *postStepPoint = m_step->GetPostStepPoint();
    const G4ThreeVector         &pos = postStepPoint->GetPosition();
    const G4double              time = postStepPoint->GetGlobalTime()*Gaudi::Units::c_light;
    m_position.set( pos.x(), pos.y(), pos.z(), time );
    m_positionSet = true;
  }

  return m_position;
}

int ISF::Geant4TruthIncident::physicsProcessCategory() const {
  const G4VProcess *process = m_step->GetPostStepPoint()->GetProcessDefinedStep();
  // TODO: need to check that G4ProcessSubTypes Match Barcode::PhysicsProcessCode
  return process->GetProcessType();
}

Barcode::PhysicsProcessCode ISF::Geant4TruthIncident::physicsProcessCode() const {
  const G4VProcess *process = m_step->GetPostStepPoint()->GetProcessDefinedStep();
  // TODO: need to check that G4ProcessSubTypes Match Barcode::PhysicsProcessCode
  return process->GetProcessSubType();
}

double ISF::Geant4TruthIncident::parentP2() const {
  const G4ThreeVector & mom= m_step->GetPreStepPoint()->GetMomentum();
  return mom.mag2();
}

double ISF::Geant4TruthIncident::parentPt2() const {
  const G4ThreeVector & mom= m_step->GetPreStepPoint()->GetMomentum();
  return mom.perp2();
}

double ISF::Geant4TruthIncident::parentEkin() const {
  return (m_step->GetPreStepPoint()->GetKineticEnergy()/CLHEP::MeV);
}

int ISF::Geant4TruthIncident::parentPdgCode() const {
  return  m_step->GetTrack()->GetDefinition()->GetPDGEncoding();
}

Barcode::ParticleBarcode ISF::Geant4TruthIncident::parentBarcode() const {
  const G4Track *track = m_step->GetTrack();
  VTrackInformation *trackInfo=static_cast<VTrackInformation *>(track->GetUserInformation());

  if (trackInfo && (trackInfo->GetHepMCParticle() || dynamic_cast<TrackBarcodeInfo*>(trackInfo))) {
    return trackInfo->GetParticleBarcode();
  }
  return Barcode::fUndefinedBarcode;
}

HepMC::GenParticle* ISF::Geant4TruthIncident::parentParticle(bool /*setPersistent*/) const {
  
  EventInformation* evtInfo = static_cast<EventInformation*>
    (G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetUserInformation());
  
  HepMC::GenParticle* hepParticle = evtInfo->GetCurrentlyTraced();

  return hepParticle;
}

bool ISF::Geant4TruthIncident::parentSurvivesIncident() const { 
  const G4Track *track = m_step->GetTrack();

  // check if particle is a alive in G4 or in ISF
  if ( particleAlive(track) ) {
    return true;
  } else {
    return false;
  }
}

HepMC::GenParticle* ISF::Geant4TruthIncident::parentParticleAfterIncident(Barcode::ParticleBarcode newBarcode,
                                                                           bool /*setPersistent*/ ) {
  const G4Track *track = m_step->GetTrack();

  // check if particle is a alive in G4 or in ISF
  if ( !parentSurvivesIncident() ) {
      return 0;
  }

  // internally cache the HepMC representation of the parent particle
  // (to allow multiple calls to this method on the same instance)
  if ( !m_parentParticleAfterIncident ) {
    // create new HepMC particle, using momentum and energy
    // from G4DynamicParticle (which should be equivalent to postStep)
    m_parentParticleAfterIncident = convert(track);
    
    EventInformation* evtInfo = static_cast<EventInformation*>
      (G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetUserInformation());
    
    evtInfo->SetCurrentlyTraced( m_parentParticleAfterIncident );
    
    m_parentParticleAfterIncident->suggest_barcode( newBarcode );

    // store (new) hepmc particle in track's UserInformation
    TrackHelper       tHelper(track);
    TrackInformation *tInfo = tHelper.GetTrackInformation();
    if (tInfo) {
      tInfo->SetParticle( m_parentParticleAfterIncident );
      int regenerationNr = tInfo->GetRegenerationNr();
      regenerationNr++;
      tInfo->SetRegenerationNr(regenerationNr);
      if ( tHelper.IsPrimary() ) { 
        tInfo->SetClassification(RegeneratedPrimary);
      }
    }

  } // <-- end if m_parentParticleAfterIncident is not filled


  // return the HepMC particle representation of the parent
  return m_parentParticleAfterIncident;
}

double ISF::Geant4TruthIncident::childP2(unsigned short i) const {
  prepareChildren();
  const G4ThreeVector & mom= m_children[i]->GetMomentum();
  return mom.mag2();
}

const G4ThreeVector ISF::Geant4TruthIncident::childP(unsigned short i) const {
  prepareChildren();
  const G4ThreeVector & mom= m_children[i]->GetMomentum();
  return mom;
}

double ISF::Geant4TruthIncident::childPt2(unsigned short i) const {
  prepareChildren();
  const G4ThreeVector & mom= m_children[i]->GetMomentum();
  return mom.perp2();
}

double ISF::Geant4TruthIncident::childEkin(unsigned short i) const {
  prepareChildren();
  return (m_children[i]->GetKineticEnergy()/CLHEP::MeV);
}

int ISF::Geant4TruthIncident::childPdgCode(unsigned short i) const {
  prepareChildren();
  return m_children[i]->GetDefinition()->GetPDGEncoding();
}

void ISF::Geant4TruthIncident::setAllChildrenBarcodes(Barcode::ParticleBarcode newBarcode) {

  prepareChildren();

  unsigned short numChildren = numberOfChildren();
  for (unsigned short i=0; i<numChildren; i++) {

    G4Track *curSecondary = m_children[i];

    // get parent if it exists in user info
    VTrackInformation *trackInfo=static_cast<VTrackInformation*>( curSecondary->GetUserInformation() );
    const ISF::ISFParticle* parent = (trackInfo) ? trackInfo->GetISFParticle() : 0;

    // assume these G4Track don't have an information yet.
    TrackBarcodeInfo * bi = new TrackBarcodeInfo(newBarcode,parent);

    // FIXME: doesn't that cause a memory leak if UserInformation already existed before?
    curSecondary->SetUserInformation(bi);
  }

  return;
}

HepMC::GenParticle* ISF::Geant4TruthIncident::childParticle(unsigned short i,
                                                            Barcode::ParticleBarcode newBarcode,
                                                            bool /*setPersistent*/) const {
  prepareChildren();

  // return value
  HepMC::GenParticle *hepParticle = 0;

  // the G4Track instance for the current child particle
  G4Track          *thisChild = m_children[i];

  // NB: NOT checking if secondary is actually alive. Even with zero momentum,
  //     secondary could decay right away and create further particles which pass the
  //     truth strategies.

  hepParticle = convert( thisChild );

  // Normal situation - no parent particle
  hepParticle->suggest_barcode( newBarcode );

  // get last (parent) ISFParticle link
  const G4Track *track = m_step->GetTrack();
  VTrackInformation *trackInfo = static_cast<VTrackInformation*>( track->GetUserInformation() );
  const ISF::ISFParticle* parentISP = (trackInfo) ? trackInfo->GetISFParticle() : 0;
 
  // create new TrackInformation (with link to hepParticle and ISFParticle)
  // and attach it to G4Track
  TrackInformation *ti = new TrackInformation(hepParticle, parentISP);
  ti->SetRegenerationNr(0);
  ti->SetClassification(RegisteredSecondary);

  thisChild->SetUserInformation(ti);

  return hepParticle;
}


bool ISF::Geant4TruthIncident::particleAlive(const G4Track *track) const {
  G4TrackStatus  trackStatus = track->GetTrackStatus();

  if ( trackStatus!=fAlive ) {
    // parent does not exist in G4 anymore after this step

    // check whether the particle was returned to ISF
    TrackInformation* trackInfo = dynamic_cast<TrackInformation*>( track->GetUserInformation() );
    bool          returnedToISF = trackInfo ? trackInfo->GetReturnedToISF() : false;
    if ( !returnedToISF ) {
      // return, since parent does not exist in ISF either after this step
      return false;
    }
  }

  return true;
}


HepMC::GenParticle* ISF::Geant4TruthIncident::convert(const G4Track *track) const {

  const G4ThreeVector & mom =  track->GetMomentum();
  const double energy =  track->GetTotalEnergy();
  int pdgCode = track->GetDefinition()->GetPDGEncoding();
  HepMC::FourVector fourMomentum( mom.x(), mom.y(), mom.z(), energy);

  int status = 1; // stable particle not decayed by EventGenerator
  HepMC::GenParticle* newParticle = new HepMC::GenParticle(fourMomentum, pdgCode, status);

  return newParticle;
}


void ISF::Geant4TruthIncident::prepareChildren() const {

  if (!m_childrenPrepared) {
    // NB: m_children is *not* a pointer, therefore this calls
    //     the assignment operator of std::vector<T> (C++11 should
    //     be using fast move semantics here)
    unsigned short numChildren = numberOfChildren();
    m_children = m_sHelper.GetSecondaries( numChildren );

    m_childrenPrepared  = true;
  }
}
