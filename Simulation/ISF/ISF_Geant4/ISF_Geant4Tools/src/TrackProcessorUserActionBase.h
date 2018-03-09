/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4TOOLS_TRACKPROCESSORUSERACTIONBASE_H
#define ISF_GEANT4TOOLS_TRACKPROCESSORUSERACTIONBASE_H

//#include "ISF_Geant4Interfaces/ITrackProcessorUserAction.h"

#include <string>

#include "G4UserTrackingAction.hh"
#include "G4UserSteppingAction.hh"
#include "G4UserEventAction.hh"

#include "MCTruth/VTrackInformation.h"


// forward declarations
class EventInformation;

namespace HepMC {
  class GenParticle;
}

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

protected:
  EventInformation* m_eventInfo;   //!< event-global G4 UserInformation

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
                          HepMC::GenParticle* truthPrimary,
                          HepMC::GenParticle* truthCurrentlyTraced);

  /// Classify the particle represented by the given set of truth links
  TrackClassification classify(const HepMC::GenParticle* primaryTruthParticle,
                               const HepMC::GenParticle* generationZeroTruthParticle,
                               const HepMC::GenParticle* currentlyTracedHepPart,
                               int regenerationNumber) const;

  /// The most recent ISFParticle ancestor that triggers the currently processed G4Track
  ISF::ISFParticle* m_curBaseISP;

}; // class TrackProcessorUserActionBase

}// iGeant4

}// G4UA

#endif // ISF_GEANT4TOOLS_TRACKPROCESSORUSERACTIONBASE_H
