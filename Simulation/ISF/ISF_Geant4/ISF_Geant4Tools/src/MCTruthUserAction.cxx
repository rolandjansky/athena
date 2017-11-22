/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MCTruthUserAction.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "MCTruthUserAction.h"

// package includes
#include "ISFTrajectory.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"

#include "ISF_Interfaces/ITruthSvc.h"
#include "ISF_Interfaces/IParticleBroker.h"

// ISF Geant4 includes
#include "ISF_Geant4Event/Geant4TruthIncident.h"

// Athena includes
#include "MCTruth/TrackInformation.h"
#include "MCTruth/EventInformation.h"
#include "MCTruth/TrackHelper.h"

#include "SimHelpers/StepHelper.h"

#include "G4EventManager.hh"

// Atlas G4 Helpers


namespace G4UA{

  namespace iGeant4{

    MCTruthUserAction::MCTruthUserAction(const Config& config)
      : m_config(config)
      , m_truthRecordSvcQuick(nullptr)
    {

      if(4<m_config.verboseLevel)
        {
          G4cout << "create MCTruthUserAction" << G4endl;
        }

      if (m_config.truthRecordSvc.retrieve().isFailure()){
        G4ExceptionDescription description;
        description << G4String("MCTruthUserAction: ") + "Could not retrieve " << m_config.truthRecordSvc;
        G4Exception("G4UA::iGeant4::MCTruthUserAction", "NoTruthSvc", FatalException, description);
        return; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
      }

      // store a pointer directly to the truth service class
      // by doing so, the Gaudi overhead can be minimized
      m_truthRecordSvcQuick = &(*(m_config.truthRecordSvc));
    }

    void MCTruthUserAction::PreUserTrackingAction(const G4Track* inTrack){
      //ATH_MSG_DEBUG("Starting to track a new particle");

      G4Track* inT = const_cast<G4Track*> (inTrack);
      TrackHelper trackHelper(inT);

      if (trackHelper.IsPrimary() ||
          (trackHelper.IsRegisteredSecondary()&&m_config.ilevel>1) ||
          (trackHelper.IsSecondary()&&m_config.ilevel>2)) {

        auto trkMgr = G4EventManager::GetEventManager()->GetTrackingManager();
        G4Trajectory *temp=new ::iGeant4::ISFTrajectory(inTrack, m_truthRecordSvcQuick);
        trkMgr->SetStoreTrajectory(true);
        trkMgr->SetTrajectory(temp);

        // TODO: check that the 'temp' obeject is actually deleted by the G4TrackingManager
        //       after FADS::FadsTrackingAction::GetTrackingAction()->ResetTraj() is executed
      }

    }

    void MCTruthUserAction::PostUserTrackingAction(const G4Track*){
      //ATH_MSG_DEBUG("Finished tracking a particle");
      G4EventManager::GetEventManager()->GetTrackingManager()->SetStoreTrajectory(false);
    }
  } // namespace iGeant4

} // namespace G4UA
