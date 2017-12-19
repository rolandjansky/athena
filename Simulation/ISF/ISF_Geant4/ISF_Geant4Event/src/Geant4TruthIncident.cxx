/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "ISF_Geant4Event/Geant4TruthIncident.h"

// package includes
#include "ISF_Geant4Event/ISFG4Helper.h"

// Atlas G4 Helpers
#include "MCTruth/EventInformation.h"
#include "MCTruth/TrackBarcodeInfo.h"
#include "MCTruth/TrackHelper.h"
#include "MCTruth/TrackInformation.h"
#include "MCTruth/PrimaryParticleInformation.h"

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

// ISF includes
#include "ISF_Event/ISFParticle.h"

/*
  Comments:
  what about parent particle surviving (e.g. bremstrahlung)


  Existing code:
  Simulation/G4Sim/SimHelpers/src/StepHelper.cxx
  - provids convenient information of a G4Step
  Simulation/G4Utilities/G4TruthStrategies/src/BremsstrahlungStrategy.cxx
  - retrives information from a G4Step (via StepHelper)
  Simulation/G4Sim/MCTruth/MCTruth/TruthStrategy.h
  - common base for different truth strategies
  Simulation/G4Sim/MCTruth/src/EventInformation.cxx
  - stores HepMCevent in G4
  Simulation/G4Sim/MCTruth/src/TrackInformation.cxx
  Simulation/G4Sim/MCTruth/src/TrackHelper.cxx
  - store/manage barcode
*/


iGeant4::Geant4TruthIncident::Geant4TruthIncident( const G4Step *step,
                                               const ISF::ISFParticle& baseISP,
                                               AtlasDetDescr::AtlasRegion geoID,
                                               EventInformation *eventInfo) :
  ITruthIncident(geoID, step->GetSecondaryInCurrentStep()->size()), // switch to G4Step::GetNumberOfSecondariesInCurrentStep() once we're using G4 10.2 or later
  m_positionSet(false),
  m_position(),
  m_step(step),
  m_baseISP(baseISP),
  m_eventInfo(eventInfo),
  m_childrenPrepared(false),
  m_children(),
  m_parentParticleAfterIncident(nullptr)
{
}

const HepMC::FourVector& iGeant4::Geant4TruthIncident::position() const {
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

int iGeant4::Geant4TruthIncident::physicsProcessCategory() const {
  const G4VProcess *process = m_step->GetPostStepPoint()->GetProcessDefinedStep();
  // TODO: need to check that G4ProcessSubTypes Match Barcode::PhysicsProcessCode
  return process->GetProcessType();
}

Barcode::PhysicsProcessCode iGeant4::Geant4TruthIncident::physicsProcessCode() const {
  const G4VProcess *process = m_step->GetPostStepPoint()->GetProcessDefinedStep();
  // TODO: need to check that G4ProcessSubTypes Match Barcode::PhysicsProcessCode
  return process->GetProcessSubType();
}

double iGeant4::Geant4TruthIncident::parentP2() const {
  const G4ThreeVector & mom= m_step->GetPreStepPoint()->GetMomentum();
  return mom.mag2();
}

double iGeant4::Geant4TruthIncident::parentPt2() const {
  const G4ThreeVector & mom= m_step->GetPreStepPoint()->GetMomentum();
  return mom.perp2();
}

double iGeant4::Geant4TruthIncident::parentEkin() const {
  return (m_step->GetPreStepPoint()->GetKineticEnergy()/CLHEP::MeV);
}

int iGeant4::Geant4TruthIncident::parentPdgCode() const {
  return  m_step->GetTrack()->GetDefinition()->GetPDGEncoding();
}

Barcode::ParticleBarcode iGeant4::Geant4TruthIncident::parentBarcode() const {
  auto parent = parentParticle();

  return (parent) ? parent->barcode() : Barcode::fUndefinedBarcode;
}

HepMC::GenParticle* iGeant4::Geant4TruthIncident::parentParticle() const {
  HepMC::GenParticle* hepParticle = m_eventInfo->GetCurrentlyTraced();

  return hepParticle;
}

int iGeant4::Geant4TruthIncident::parentBCID() const { 
  return m_baseISP.getBCID();
}


bool iGeant4::Geant4TruthIncident::parentSurvivesIncident() const { 
  const G4Track *track = m_step->GetTrack();

  // check if particle is a alive in G4 or in ISF
  if ( particleAlive(track) ) {
    return true;
  } else {
    return false;
  }
}

HepMC::GenParticle* iGeant4::Geant4TruthIncident::parentParticleAfterIncident(Barcode::ParticleBarcode newBarcode) {
  const G4Track *track = m_step->GetTrack();

  // check if particle is a alive in G4 or in ISF
  if ( !parentSurvivesIncident() ) {
    return nullptr;
  }

  // internally cache the HepMC representation of the parent particle
  // (to allow multiple calls to this method on the same instance)
  if ( !m_parentParticleAfterIncident ) {
    // create new HepMC particle, using momentum and energy
    // from G4DynamicParticle (which should be equivalent to postStep)
    m_parentParticleAfterIncident = convert(track, newBarcode, false);
    
    m_eventInfo->SetCurrentlyTraced( m_parentParticleAfterIncident );
    
    // store (new) hepmc particle in track's UserInformation
    TrackHelper       tHelper(track);
    TrackInformation *tInfo = tHelper.GetTrackInformation();
    if (tInfo) {
      // do NOT update the TrackInformation for regenerated particles!
      // (most recent truth info is kept in EventInformation)
      //tInfo->SetParticle( m_parentParticleAfterIncident );
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

double iGeant4::Geant4TruthIncident::childP2(unsigned short i) const {
  prepareChildren();
  const G4ThreeVector & mom= m_children[i]->GetMomentum();
  return mom.mag2();
}

const G4ThreeVector iGeant4::Geant4TruthIncident::childP(unsigned short i) const {
  prepareChildren();
  const G4ThreeVector & mom= m_children[i]->GetMomentum();
  return mom;
}

double iGeant4::Geant4TruthIncident::childPt2(unsigned short i) const {
  prepareChildren();
  const G4ThreeVector & mom= m_children[i]->GetMomentum();
  return mom.perp2();
}

double iGeant4::Geant4TruthIncident::childEkin(unsigned short i) const {
  prepareChildren();
  return (m_children[i]->GetKineticEnergy()/CLHEP::MeV);
}

int iGeant4::Geant4TruthIncident::childPdgCode(unsigned short i) const {
  prepareChildren();
  return m_children[i]->GetDefinition()->GetPDGEncoding();
}

void iGeant4::Geant4TruthIncident::setAllChildrenBarcodes(Barcode::ParticleBarcode) {
  G4ExceptionDescription description;
  description << G4String("setAllChildrenBarcodes: ") + "Shared child particle barcodes are not implemented in ISF_Geant4 at this point.";
  G4Exception("iGeant4::Geant4TruthIncident", "NotImplemented", FatalException, description);
}

HepMC::GenParticle* iGeant4::Geant4TruthIncident::childParticle(unsigned short i,
                                                            Barcode::ParticleBarcode newBarcode) const {
  prepareChildren();

  // the G4Track instance for the current child particle
  const G4Track* thisChildTrack = m_children[i];

  // NB: NOT checking if secondary is actually alive. Even with zero momentum,
  //     secondary could decay right away and create further particles which pass the
  //     truth strategies.

  HepMC::GenParticle* hepParticle = convert( thisChildTrack , newBarcode , true );

  TrackHelper tHelper(thisChildTrack);
  TrackInformation *trackInfo = tHelper.GetTrackInformation();

  // needed to make AtlasG4 work with ISF TruthService
  if(trackInfo==nullptr) {
    trackInfo = new TrackInformation( hepParticle );    
    thisChildTrack->SetUserInformation( trackInfo );    
  } 
    
  trackInfo->SetParticle(hepParticle);
  trackInfo->SetClassification(RegisteredSecondary);
  trackInfo->SetRegenerationNr(0);

  return hepParticle;
}


bool iGeant4::Geant4TruthIncident::particleAlive(const G4Track *track) const {
  G4TrackStatus  trackStatus = track->GetTrackStatus();

  if ( trackStatus!=fAlive ) {
    // parent does not exist in G4 anymore after this step

    // check whether the particle was returned to ISF
    auto*    trackInfo = ISFG4Helper::getISFTrackInfo( *track );
    bool returnedToISF = trackInfo ? trackInfo->GetReturnedToISF() : false;
    if ( !returnedToISF ) {
      // particle was not sent to ISF either
      return false;
    }
  }

  return true;
}


HepMC::GenParticle* iGeant4::Geant4TruthIncident::convert(const G4Track *track, const int barcode, const bool secondary) const {

  const G4ThreeVector & mom =  track->GetMomentum();
  const double energy =  track->GetTotalEnergy();
  int pdgCode = track->GetDefinition()->GetPDGEncoding();
  HepMC::FourVector fourMomentum( mom.x(), mom.y(), mom.z(), energy);

  int status = 1; // stable particle not decayed by EventGenerator
  HepMC::GenParticle* newParticle = new HepMC::GenParticle(fourMomentum, pdgCode, status);

  // This should be a *secondary* track.  If it has a primary, it was a decay and 
  //  we are running with quasi-stable particle simulation.  Note that if the primary
  //  track is passed in as a secondary that survived the interaction, then this was
  //  *not* a decay and we should not treat it in this way
  if (secondary &&
      track->GetDynamicParticle() &&
      track->GetDynamicParticle()->GetPrimaryParticle() &&
      track->GetDynamicParticle()->GetPrimaryParticle()->GetUserInformation()){
    // Then the new particle should use the same barcode as the old one!!
    PrimaryParticleInformation* ppi = dynamic_cast<PrimaryParticleInformation*>( track->GetDynamicParticle()->GetPrimaryParticle()->GetUserInformation() );
    newParticle->suggest_barcode( ppi->GetParticleBarcode() );
  } else {
    newParticle->suggest_barcode( barcode );
  }

  return newParticle;
}


void iGeant4::Geant4TruthIncident::prepareChildren() const {

  if (!m_childrenPrepared) {
    unsigned short numChildren = numberOfChildren();
    const auto &tracks = m_step->GetSecondaryInCurrentStep();
    const int iSize=tracks->size();
    const int iLast=iSize-numChildren-1; //NB can be -1.
    for(int i=iSize-1;i>iLast;i--) {
      m_children.push_back((*tracks)[i]);
    }
    m_childrenPrepared  = true;
  }
}
