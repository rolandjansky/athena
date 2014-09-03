/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"

#include "ISF_Geant4Tools/SDActivateUserAction.h"

#include "FadsSensitiveDetector/FadsSensitiveDetector.h"
#include "FadsSensitiveDetector/SensitiveDetectorCatalog.h"
#include "FadsActions/FadsStackingAction.h"
#include "G4StackManager.hh"
#include "G4UImanager.hh"

#include <iostream>

iGeant4::SDActivateUserAction::SDActivateUserAction(const std::string& type,
						    const std::string& name,
						    const IInterface* parent)
  : ISDActivateUserAction(name)
  , AthAlgTool(type,name,parent)
  , m_stackManager(0)
{
  
  declareInterface<ISDActivateUserAction>(this);
  
}

iGeant4::SDActivateUserAction::~SDActivateUserAction()
{

}

StatusCode iGeant4::SDActivateUserAction::initialize()
{
  ATH_MSG_INFO("Initializing SDActivateUserAction");
  
  return StatusCode::SUCCESS;
}

StatusCode iGeant4::SDActivateUserAction::finalize()
{
  return StatusCode::SUCCESS;
}


void iGeant4::SDActivateUserAction::BeginOfEventAction(const G4Event*)
{
  ATH_MSG_DEBUG("BeginOfEventAction");

  //G4UImanager *ui = G4UImanager::GetUIpointer();
  //ui->ApplyCommand("/event/verbose 2");

  if (!m_stackManager) {
    m_stackManager = FADS::FadsStackingAction::GetStackingAction()->GetStackManager();
    
    FADS::SDStore* sdstore = 
      FADS::SensitiveDetectorCatalog::GetSensitiveDetectorCatalog()->sdStore();
    for (FADS::SDStore::iterator it=sdstore->begin(); it!=sdstore->end(); it++) {
      ATH_MSG_DEBUG("checking "<<(*it).first);    
      if ((*it).second->isActive()) {
	ATH_MSG_DEBUG("setting "<<(*it).first<<" to active");
	m_sdToDeactivate.push_back((*it).second);
      }
    }
  }

  // activate SensitiveDetectors
  activateSD(true);
  
}

void iGeant4::SDActivateUserAction::EndOfEventAction(const G4Event*)
{
  // dummy check to make sure no postponed tracks
  ATH_MSG_DEBUG("EndOfEvent");
  if (m_stackManager->GetNPostponedTrack()>0) 
    ATH_MSG_ERROR("ISF not expecting postponed tracks!  Please fix me!");
  
  //G4UImanager *ui = G4UImanager::GetUIpointer();
  //ui->ApplyCommand("/event/verbose 1");

}

void iGeant4::SDActivateUserAction::PreUserTrackingAction(const G4Track*)
{

  ATH_MSG_DEBUG("PreUserTrackingAction");

  // if sensitive detectors have been deactivated, reactivate
  if (!m_sdActivated) activateSD(true);
  
}

void iGeant4::SDActivateUserAction::PostUserTrackingAction(const G4Track*)

{ 
  ATH_MSG_DEBUG("PostUserTrackingAction");

  ATH_MSG_DEBUG("NTotalTrack: "<<m_stackManager->GetNTotalTrack());
  if (m_stackManager->GetNTotalTrack()==0) activateSD(false);
    
}

void iGeant4::SDActivateUserAction::activateSD(bool active)
{
  if (active) 
    ATH_MSG_DEBUG("setting SensitiveDetectors to active");
  else
    ATH_MSG_DEBUG("setting SensitiveDetectors to inactive");
  
  for (std::vector<FADS::FadsSensitiveDetector*>::iterator it=m_sdToDeactivate.begin();
       it!=m_sdToDeactivate.end();++it) {
    (**it).Activate(active);
  }

  m_sdActivated=active;
  
}
