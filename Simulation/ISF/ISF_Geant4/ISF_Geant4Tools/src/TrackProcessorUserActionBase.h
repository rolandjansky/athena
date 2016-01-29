/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4TOOLS_TRACKPROCESSORUSERACTIONBASE_H
#define ISF_GEANT4TOOLS_TRACKPROCESSORUSERACTIONBASE_H

#include "ISF_Geant4Interfaces/ITrackProcessorUserAction.h"

#include <string>

// forward declarations
namespace HepMC {
  class GenParticle;
}

namespace ISF {
  class ISFParticle;
}

namespace iGeant4 {

  class TrackProcessorUserActionBase: virtual public ITrackProcessorUserAction {

  public:
    TrackProcessorUserActionBase(const std::string& name);
    virtual ~TrackProcessorUserActionBase() {}

    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);
    void BeginOfRunAction(const G4Run*);
    void EndOfRunAction(const G4Run*);

    void SteppingAction(const G4Step*);

    void PreUserTrackingAction(const G4Track* aTrack);
    void PostUserTrackingAction(const G4Track* aTrack);

    /** This method is called by TrackProcessorUserActionBase after the
     *  G4Track->ISFParticle association has been established for the current G4Track */
    virtual void ISFSteppingAction(const G4Step*, ISF::ISFParticle *curISP) = 0;

  private:
    /** for keeping track of the currently processed G4Track and its corresponding ISFParticle */
    int                                 m_curTrackID;  //!< the TrackID of the currently processed G4Track
    const HepMC::GenParticle           *m_curPrimary;  //!< the primary particle causing the currently processed G4Track
    ISF::ISFParticle                   *m_curISP;      //!< the corresponding ISFParticle to this G4Track

  };

}

#endif // ISF_GEANT4TOOLS_TRACKPROCESSORUSERACTIONBASE_H
