/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Geant4TruthIncident.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "Geant4TruthIncident.h"

// ISF includes
//#include "ISF_Event/ISFParticle.h"

// Units
#include "GaudiKernel/PhysicalConstants.h"

// HepMC includes
#include "HepMC/GenParticle.h"

// Atlas G4 Helpers
#include "SimHelpers/SecondaryTracksHelper.h"
#include "MCTruth/TrackInformation.h"
#include "MCTruth/TrackBarcodeInfo.h"
#include "MCTruth/TrackHelper.h"

#include "MCTruth/EventInformation.h"
#include "FadsActions/FadsTrackingAction.h"


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

/*
  Comments:
  what about primary particle surviving (e.g. bremstrahlung)


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


ISF::Geant4TruthIncident::Geant4TruthIncident(const G4Step *step, AtlasDetDescr::AtlasRegion geoID, int numSecondaries, SecondaryTracksHelper &sHelper) :
  ITruthIncident(geoID),
  m_positionSet(false),
  m_position(),
  m_step(step),
  m_sHelper( sHelper),
  m_secondariesNum( numSecondaries ),
  m_secondariesPrepared(false),
  m_secondaries(),
  m_checkLastSecondaryOnly(false),
  m_passedFilters(numSecondaries, false),
  m_primaryParticleAfterIncident(0)
{
  // only intialisation
  //  static SecondaryTracksHelper sHelper(FADS::FadsTrackingAction::GetTrackingAction()->GetTrackingManager());

  Barcode::PhysicsProcessCode proc = physicsProcessCode();
  if (proc==fIonisation || proc==fPhotoElectricEffect ||
      proc==fBremsstrahlung ||
      proc==fComptonScattering) m_checkLastSecondaryOnly=true;

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

Barcode::PhysicsProcessCode ISF::Geant4TruthIncident::physicsProcessCode() const {
  const G4VProcess * process = m_step->GetPostStepPoint()->GetProcessDefinedStep();

  // TODO: need to check that G4ProcessSubTypes Match Barcode::PhysicsProcessCode
  return process->GetProcessSubType();
}

double ISF::Geant4TruthIncident::primaryP2() const {
  const G4ThreeVector & mom= m_step->GetPreStepPoint()->GetMomentum();
  return mom.mag2();
}

double ISF::Geant4TruthIncident::primaryPt2() const {
  const G4ThreeVector & mom= m_step->GetPreStepPoint()->GetMomentum();
  return mom.perp2();
}

double ISF::Geant4TruthIncident::primaryEkin() const {
  return (m_step->GetPreStepPoint()->GetKineticEnergy()/CLHEP::MeV);
}

int ISF::Geant4TruthIncident::primaryPdgCode() const {
  return  m_step->GetTrack()->GetDefinition()->GetPDGEncoding();
}

Barcode::ParticleBarcode ISF::Geant4TruthIncident::primaryBarcode() const {
  const G4Track *track = m_step->GetTrack();
  VTrackInformation *trackInfo=static_cast<VTrackInformation *>(track->GetUserInformation());

  if (trackInfo && (trackInfo->GetHepMCParticle() || dynamic_cast<TrackBarcodeInfo*>(trackInfo))) {
    return trackInfo->GetParticleBarcode();
  }
  return Barcode::fUndefinedBarcode;
}

HepMC::GenParticle* ISF::Geant4TruthIncident::primaryParticle(bool /*setPersistent*/) const {
  
  EventInformation* evtInfo = static_cast<EventInformation*>
    (G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetUserInformation());
  
  HepMC::GenParticle* hepParticle = evtInfo->GetCurrentlyTraced();

  return hepParticle;
}

HepMC::GenParticle* ISF::Geant4TruthIncident::primaryParticleAfterIncident(Barcode::ParticleBarcode newBarcode,
                                                                           bool /*setPersistent*/ ) {
  const G4Track *track = m_step->GetTrack();

  // check if particle is a alive in G4 or in ISF
  if ( !particleAlive(track) ) {
    return 0;
  }

  // internally cache the HepMC representation of the primary particle
  // (to allow multiple calls to this method on the same instance)
  if ( !m_primaryParticleAfterIncident ) {
    // create new HepMC particle, using momentum and energy
    // from G4DynamicParticle (which should be equivalent to postStep)
    m_primaryParticleAfterIncident = convert(track);
    
    EventInformation* evtInfo = static_cast<EventInformation*>
      (G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetUserInformation());
    
    evtInfo->SetCurrentlyTraced( m_primaryParticleAfterIncident );
    
    m_primaryParticleAfterIncident->suggest_barcode( newBarcode );

    // store (new) hepmc particle in track's UserInformation
    TrackHelper       tHelper(track);
    TrackInformation *tInfo = tHelper.GetTrackInformation();
    if (tInfo) {
      int regenerationNr = tInfo->GetRegenerationNr();
      regenerationNr++;
      tInfo->SetRegenerationNr(regenerationNr);
      if ( tHelper.IsPrimary() ) { 
        tInfo->SetClassification(RegeneratedPrimary);
      }
    }

  } // <-- end if m_primaryParticleAfterIncident is not filled


  // return the HepMC particle representation of the primary
  return m_primaryParticleAfterIncident;
}

unsigned short ISF::Geant4TruthIncident::numberOfSecondaries() const {

  return m_secondariesNum;
}

double ISF::Geant4TruthIncident::secondaryP2(unsigned short i) const {
  prepareSecondaries();
  const G4ThreeVector & mom= m_secondaries[i]->GetMomentum();
  return mom.mag2();
}

const G4ThreeVector ISF::Geant4TruthIncident::secondaryP(unsigned short i) const {
  prepareSecondaries();
  const G4ThreeVector & mom= m_secondaries[i]->GetMomentum();
  return mom;
}

double ISF::Geant4TruthIncident::secondaryPt2(unsigned short i) const {
  prepareSecondaries();
  const G4ThreeVector & mom= m_secondaries[i]->GetMomentum();
  return mom.perp2();
}

double ISF::Geant4TruthIncident::secondaryEkin(unsigned short i) const {
  prepareSecondaries();
  return (m_secondaries[i]->GetKineticEnergy()/CLHEP::MeV);
}

int ISF::Geant4TruthIncident::secondaryPdgCode(unsigned short i) const {
  prepareSecondaries();
  return m_secondaries[i]->GetDefinition()->GetPDGEncoding();
}

void ISF::Geant4TruthIncident::setAllSecondaryBarcodes(Barcode::ParticleBarcode newBarcode) {

  prepareSecondaries();

  for (unsigned short i=0; i<m_secondariesNum; i++) {

    G4Track *curSecondary = m_secondaries[i];

    // get parent if it exists in user info
    VTrackInformation *trackInfo=static_cast<VTrackInformation*>( curSecondary->GetUserInformation() );
    const ISF::ISFParticle* parent = (trackInfo) ? trackInfo->GetISFParticle() : NULL;

    // assume these G4Track don't have an information yet.
    TrackBarcodeInfo * bi = new TrackBarcodeInfo(newBarcode,parent);

    curSecondary->SetUserInformation(bi);
  }

  return;
}

HepMC::GenParticle* ISF::Geant4TruthIncident::secondaryParticle(unsigned short i,
                                                                Barcode::ParticleBarcode newBarcode,
                                                                bool /*setPersistent*/) const {
  prepareSecondaries();

  // return value
  HepMC::GenParticle *hepParticle = 0;

  // the G4Track instance for the current secondary
  G4Track          *thisSecondary = m_secondaries[i];

  // NB: NOT checking if secondary is actually alive. Even with zero momentum,
  //     secondary could decay right away and create further particles which pass the
  //     truth strategies.

  hepParticle = convert( thisSecondary );

  const G4DynamicParticle *g4DynParticle  = thisSecondary->GetDynamicParticle();
  G4PrimaryParticle       *g4PrimParticle = g4DynParticle ? g4DynParticle->GetPrimaryParticle() : 0;

  if ( g4PrimParticle ) {
    // This is a secondary that came from a primary particle
    //  It seems like a good idea to get back at the primary particle
    //  and from there use the gen particle.  But in the step *right* 
    //  before this, the gen particles are all deleted, so this info   
    //  is not actually available at this point.  If only.

    // See if it should be stable
    if ( g4PrimParticle->GetDaughter() ){
      hepParticle->set_status(2);
    }
    // Now we know that we have to deal with the barcode specially
    hepParticle->suggest_barcode( g4PrimParticle->GetTrackID() );

  } else {
    // Normal situation - no primary particle
    hepParticle->suggest_barcode( newBarcode );
  }
  
  // create new TrackInformation (with link to hepParticle)
  // and attach it to G4Track
  TrackInformation *ti = new TrackInformation(hepParticle);
  ti->SetRegenerationNr(0);
  ti->SetClassification(RegisteredSecondary);

  thisSecondary->SetUserInformation(ti);

  return hepParticle;
}

bool ISF::Geant4TruthIncident::particleAlive(const G4Track *track) const {
  G4TrackStatus  trackStatus = track->GetTrackStatus();

  if ( trackStatus!=fAlive ) {
    // primary does not exist in G4 anymore after this step

    // check whether the particle was returned to ISF
    TrackInformation* trackInfo = dynamic_cast<TrackInformation*>( track->GetUserInformation() );
    bool          returnedToISF = trackInfo ? trackInfo->GetReturnedToISF() : false;
    if ( !returnedToISF ) {
      // return, since primary does not exist in ISF either after this step
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


void ISF::Geant4TruthIncident::prepareSecondaries() const {

  if (!m_secondariesPrepared) {
    // NB: m_secondaries is *not* a pointer, therefore this calls
    //     the assignment operator of std::vector<T> (C++11 should
    //     be using fast move semantics here)
    m_secondaries = m_sHelper.GetSecondaries( m_secondariesNum );

    m_secondariesPrepared  = true;
  }
}
