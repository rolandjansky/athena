/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MCTruthUserAction.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "ISF_Geant4Tools/MCTruthUserAction.h"

// ISF includes
#include "ISF_Geant4Tools/Geant4TruthIncident.h"
#include "ISF_Interfaces/ITruthSvc.h"
#include "ISF_Interfaces/IParticleBroker.h"
#include "ISF_Event/ISFParticle.h"

#include "MCTruth/TrackInformation.h"
#include "MCTruth/EventInformation.h"
#include "MCTruth/TrackHelper.h"

#include "SimHelpers/StepHelper.h"

#include "ISF_Geant4Tools/ISFTrajectory.h"

// Atlas G4 Helpers

iGeant4::MCTruthUserAction::MCTruthUserAction(const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent)
: FADS::UserAction(name),
  AthAlgTool(type,name,parent),
  m_sHelper(FADS::FadsTrackingAction::GetTrackingAction()->GetTrackingManager()),
  m_truthRecordSvc("ISF_TruthRecordSvc", name),
  m_truthRecordSvcQuick(0),
  m_isfParent(0)
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
  m_isfParent = 0; 
    
  G4Track* inT = const_cast<G4Track*> (inTrack);
  TrackHelper trackHelper(inT);
   
  if (trackHelper.IsPrimary() || 
      (trackHelper.IsRegisteredSecondary()&&m_ilevel>1) ||
      (trackHelper.IsSecondary()&&m_ilevel>2)) {
  
    G4Trajectory *temp=new iGeant4::ISFTrajectory(inTrack, m_truthRecordSvcQuick);
    FADS::FadsTrackingAction::GetTrackingAction()->SetTraj(temp);       
  }
  
}

void iGeant4::MCTruthUserAction::PostUserTrackingAction(const G4Track* )
{
  FADS::FadsTrackingAction::GetTrackingAction()->ResetTraj();
  // -- nothing to do --
}

iGeant4::MCTruthUserAction::~MCTruthUserAction()
{}
