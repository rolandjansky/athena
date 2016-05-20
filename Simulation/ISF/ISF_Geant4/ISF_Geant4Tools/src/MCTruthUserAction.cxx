/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MCTruthUserAction.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "MCTruthUserAction.h"

// package includes
#include "Geant4TruthIncident.h"
#include "ISFTrajectory.h"

// ISF includes
#include "ISF_Interfaces/ITruthSvc.h"
#include "ISF_Interfaces/IParticleBroker.h"

#include "ISF_Event/ISFParticle.h"

// Athena includes
#include "MCTruth/TrackInformation.h"
#include "MCTruth/EventInformation.h"
#include "MCTruth/TrackHelper.h"

#include "SimHelpers/StepHelper.h"


// Atlas G4 Helpers

iGeant4::MCTruthUserAction::MCTruthUserAction(const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent)
  : UserActionBase(type,name,parent),
    m_sHelper(),
    m_truthRecordSvc("ISF_TruthRecordSvc", name),
    m_truthRecordSvcQuick(nullptr)
{

  //declareInterface<IUserAction>(this);

  declareProperty("TruthRecordSvc", m_truthRecordSvc, "ISF Particle Truth Svc");

  declareProperty("SecondarySavingLevel", m_ilevel=2);

}

StatusCode iGeant4::MCTruthUserAction::initialize()
{
  ATH_MSG_INFO("Initializing iGeant4::MCTruthUserAction");

  if (m_truthRecordSvc.retrieve().isFailure()){
    ATH_MSG_FATAL( "Could not retrieve " << m_truthRecordSvc );
    return StatusCode::FAILURE;
  }

  m_sHelper=SecondaryTracksHelper(G4EventManager::GetEventManager()->GetTrackingManager());

  // store a pointer directly to the truth service class
  // by doing so, the Gaudi overhead can be minimized
  m_truthRecordSvcQuick = &(*m_truthRecordSvc);

  return StatusCode::SUCCESS;
}

StatusCode iGeant4::MCTruthUserAction::finalize()
{
  return StatusCode::SUCCESS;
}

void iGeant4::MCTruthUserAction::PreTracking(const G4Track* inTrack)
{

  //std::cout<<"in MCTruthUserAction::PreUserTrackingAction, m_ilevel="<<m_ilevel<<std::endl;

  //m_sHelper.ResetNrOfSecondaries();

  G4Track* inT = const_cast<G4Track*> (inTrack);
  TrackHelper trackHelper(inT);

  if (trackHelper.IsPrimary() ||
      (trackHelper.IsRegisteredSecondary()&&m_ilevel>1) ||
      (trackHelper.IsSecondary()&&m_ilevel>2)) {

    G4Trajectory *temp=new iGeant4::ISFTrajectory(inTrack, m_truthRecordSvcQuick);
    m_fpTrackingManager->SetStoreTrajectory(true);
    m_fpTrackingManager->SetTrajectory(temp);
    // @TODO check that the G4TrackingManager deletes the 'temp'
    //       object after G4TrackingManager::SetStoreTrajectory(false)
    //       is executed.
  }

}

void iGeant4::MCTruthUserAction::PostTracking(const G4Track* )
{
  // We are done tracking this particle, so reset the trajectory.
  m_fpTrackingManager->SetStoreTrajectory(false);

}

iGeant4::MCTruthUserAction::~MCTruthUserAction()
{}



StatusCode iGeant4::MCTruthUserAction::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IUserAction::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IUserAction*>(this);
    addRef();
  } else {
    // Interface is not directly available : try out a base class
    return UserActionBase::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}


namespace G4UA{

  namespace iGeant4{

    MCTruthUserAction::MCTruthUserAction(const Config& config):
      m_config(config){

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

      m_sHelper=SecondaryTracksHelper(G4EventManager::GetEventManager()->GetTrackingManager());

      // store a pointer directly to the truth service class
      // by doing so, the Gaudi overhead can be minimized
      m_truthRecordSvcQuick = &(*(m_config.truthRecordSvc));
    }

    void MCTruthUserAction::preTracking(const G4Track* inTrack){;
      //ATH_MSG_DEBUG("Starting to track a new particle");

      //m_sHelper.ResetNrOfSecondaries();
 
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

    void MCTruthUserAction::postTracking(const G4Track*){
      //ATH_MSG_DEBUG("Finished tracking a particle");
      G4EventManager::GetEventManager()->GetTrackingManager()->SetStoreTrajectory(false);
    }
  } // namespace iGeant4

} // namespace G4UA
