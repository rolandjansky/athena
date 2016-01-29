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
: FADS::UserAction(name),
  AthAlgTool(type,name,parent),
  m_sHelper(FADS::FadsTrackingAction::GetTrackingAction()->GetTrackingManager()),
  m_truthRecordSvc("ISF_TruthRecordSvc", name),
  m_truthRecordSvcQuick(0)
{

  declareInterface<IMCTruthUserAction>(this);

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
  // store a pointer directly to the truth service class
  // by doing so, the Gaudi overhead can be minimized
  m_truthRecordSvcQuick = &(*m_truthRecordSvc);

  return StatusCode::SUCCESS;
}

StatusCode iGeant4::MCTruthUserAction::finalize()
{
  return StatusCode::SUCCESS;
}

void iGeant4::MCTruthUserAction::PreUserTrackingAction(const G4Track* inTrack)
{

  //std::cout<<"in MCTruthUserAction::PreUserTrackingAction, m_ilevel="<<m_ilevel<<std::endl;

  //m_sHelper.ResetNrOfSecondaries();
    
  G4Track* inT = const_cast<G4Track*> (inTrack);
  TrackHelper trackHelper(inT);
   
  if (trackHelper.IsPrimary() || 
      (trackHelper.IsRegisteredSecondary()&&m_ilevel>1) ||
      (trackHelper.IsSecondary()&&m_ilevel>2)) {
  
    G4Trajectory *temp=new iGeant4::ISFTrajectory(inTrack, m_truthRecordSvcQuick);
    FADS::FadsTrackingAction::GetTrackingAction()->SetTraj(temp);       
    // TODO: check that the 'temp' obeject is actually deleted by the G4TrackingManager
    //       after FADS::FadsTrackingAction::GetTrackingAction()->ResetTraj() is executed
  }
  
}

void iGeant4::MCTruthUserAction::PostUserTrackingAction(const G4Track* )
{
  FADS::FadsTrackingAction::GetTrackingAction()->ResetTraj();
  // -- nothing to do --
}

iGeant4::MCTruthUserAction::~MCTruthUserAction()
{}
