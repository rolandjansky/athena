/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4TOOLS_TRACKPROCESSORUSERACTIONBASE_H
#define ISF_GEANT4TOOLS_TRACKPROCESSORUSERACTIONBASE_H

#include <string>

#include "G4UserTrackingAction.hh"
#include "G4UserSteppingAction.hh"
#include "G4UserEventAction.hh"

#include "MCTruth/VTrackInformation.h"

#include "ISF_Event/ISFParticleContainer.h"

// forward declarations
class AtlasG4EventUserInfo;

#include "AtlasHepMC/GenParticle_fwd.h"

namespace ISF {
  class ISFParticle;
}


namespace G4UA{

namespace iGeant4 {

class TrackProcessorUserActionBase: public G4UserTrackingAction,
                                    public G4UserSteppingAction,
                                    public G4UserEventAction
{

public:
  TrackProcessorUserActionBase();
  virtual void BeginOfEventAction(const G4Event*) override final;
  virtual void EndOfEventAction(const G4Event*) override final;

  virtual void PreUserTrackingAction(const G4Track*) override;
  virtual void PostUserTrackingAction(const G4Track*) override final;

  virtual void UserSteppingAction(const G4Step*) override final;

  ISF::ISFParticleContainer ReturnSecondaries(ISF::ISFParticle const* parent);

protected:
  AtlasG4EventUserInfo* m_atlasG4EvtUserInfo;   //!< event-global G4 UserInformation

  ISF::ISFParticleContainer m_storedSecondaries;

  /** This method is called by TrackProcessorUserActionBase after the
   *  G4Track->ISFParticle association has been established for the current G4Track */
  virtual void ISFSteppingAction(const G4Step*, ISF::ISFParticle*) = 0;

private:
  /// Setup the given G4Track as the current primary particle which we'll process
  void setupPrimary(G4Track&);

  /// Setup the given G4Track as the current secondary particle which we'll process
  void setupSecondary(const G4Track&);

  /// Set the following information as the currently traced particle
  void setCurrentParticle(ISF::ISFParticle* baseISFParticle,
                          HepMC::GenParticlePtr truthPrimary,
                          HepMC::GenParticlePtr truthCurrentlyTraced);

  /// Classify the particle represented by the given set of truth links
  TrackClassification classify(HepMC::ConstGenParticlePtr primaryTruthParticle,
                               HepMC::ConstGenParticlePtr generationZeroTruthParticle,
                               HepMC::ConstGenParticlePtr currentlyTracedHepPart,
                               int regenerationNumber) const;

  /// The most recent ISFParticle ancestor that triggers the currently processed G4Track
  ISF::ISFParticle* m_curBaseISP;

}; // class TrackProcessorUserActionBase

}// iGeant4

}// G4UA

#endif // ISF_GEANT4TOOLS_TRACKPROCESSORUSERACTIONBASE_H
