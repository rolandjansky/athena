/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4TOOLS_TRACKPROCESSORUSERACTIONBASE_H
#define ISF_GEANT4TOOLS_TRACKPROCESSORUSERACTIONBASE_H

//#include "ISF_Geant4Interfaces/ITrackProcessorUserAction.h"

#include <string>

// forward declarations
class EventInformation;

namespace HepMC {
  class GenParticle;
}

namespace ISF {
  class ISFParticle;
}


#include "G4AtlasInterfaces/IPreTrackingAction.h"
#include "G4AtlasInterfaces/ISteppingAction.h"
#include "G4AtlasInterfaces/IBeginEventAction.h"
#include "G4AtlasInterfaces/IEndEventAction.h"
#include "G4AtlasInterfaces/IPostTrackingAction.h"


namespace G4UA{

  namespace iGeant4 {

    class TrackProcessorUserActionBase:
      public IPreTrackingAction,  public ISteppingAction,  public IBeginEventAction, public IEndEventAction, public IPostTrackingAction
    {

    public:

      TrackProcessorUserActionBase();
      virtual void beginOfEvent(const G4Event*) override final;
      virtual void endOfEvent(const G4Event*) override final;

      virtual void preTracking(const G4Track*) override;
      virtual void postTracking(const G4Track*) override final;

      virtual void processStep(const G4Step*) override final;

  protected:
      EventInformation                   *m_eventInfo;   //!< event-global G4 UserInformation

      /** This method is called by TrackProcessorUserActionBase after the
       *  G4Track->ISFParticle association has been established for the current G4Track */
      virtual void ISFSteppingAction(const G4Step*, ISF::ISFParticle*) = 0;

    private:
      /** for keeping track of the currently processed G4Track and its corresponding ISFParticle */
      ISF::ISFParticle                   *m_curBaseISP;      //!< the corresponding ISFParticle to this G4Track
    }; // class TrackProcessorUserActionBase




  }// iGeant4

}// G4UA

#endif // ISF_GEANT4TOOLS_TRACKPROCESSORUSERACTIONBASE_H
