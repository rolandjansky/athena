/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_Geant4Interfaces_MCTruthUserAction_H
#define ISF_Geant4Interfaces_MCTruthUserAction_H


//#include "ISF_Geant4Interfaces/IMCTruthUserAction.h"

#include <string>

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

//#include "FadsActions/UserAction.h"
//#include "FadsActions/TrackingAction.h"
#include "G4AtlasInterfaces/IUserActionSvc.h"
#include "G4AtlasTools/UserActionBase.h"
#include "ISF_Interfaces/ITruthSvc.h"

// Atlas G4 Helpers
#include "SimHelpers/SecondaryTracksHelper.h"


/*
  Comments:
     Existing implementation Simulation/G4Sim/MCTruth/MCTruth/MCTruthSteppingAction.h is a 
     G4UserSteppingAction and registers itself to FADS::SteppingAction::SetMCTSteppingAction

  Notes:

     employs ISFTruthService
     needs to know the current geoID, may be obtained from iGeant4::TransportTool, or from current particle in IParticleBroker
     m_particleBroker->current()->passport().nextGeo()
     Thus it is defined as a AlgTool, to get these assigned easily via python.
*/

namespace ISF {
  class IParticleBroker;
  class ISFParticle;
}

namespace iGeant4 {

  class ITransportTool;

  class MCTruthUserAction : public  UserActionBase {

 public:
    MCTruthUserAction(const std::string& type,
		      const std::string& name,
		      const IInterface* parent);
    ~MCTruthUserAction();

    StatusCode initialize();
    StatusCode finalize();

    //void BeginOfEventAction(const G4Event*);
    //void EndOfEventAction(const G4Event*);
    //void BeginOfRunAction(const G4Run*);
    //void EndOfRunAction(const G4Run*);
    void PreTracking(const G4Track* aTrack);
    void PostTracking(const G4Track* aTrack);

    virtual StatusCode queryInterface(const InterfaceID&, void**) override;

  private:

    ServiceHandle<IUserActionSvc>    m_UASvc;
    SecondaryTracksHelper m_sHelper;

    /** the ISF truth service */
    ServiceHandle<ISF::ITruthSvc>    m_truthRecordSvc;
    ISF::ITruthSvc                  *m_truthRecordSvcQuick; //!< used for faster access

    const ISF::ISFParticle* m_isfParent;

    int m_ilevel;                                // secondary saving level

  };

} // namespace iGeant4

#endif // ISF_Geant4Interfaces_MCTruthUserAction_H
