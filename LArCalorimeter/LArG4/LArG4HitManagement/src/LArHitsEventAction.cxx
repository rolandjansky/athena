/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4HitManagement/LArHitsEventAction.h"
#include "LArG4HitManagement/LArHitMergerFactory.h"
#include "LArG4HitManagement/LArCalibHitMergerFactory.h"
#include "LArG4RunControl/LArG4GlobalOptions.h"

#include "LArSimEvent/LArHitContainer.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

#include "FadsActions/FadsEventAction.h"
#include "FadsActions/FadsRunAction.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArMiniFCAL_ID.h"

#include "G4Run.hh"
#include "G4Event.hh"

#include <iostream>
#include <vector>

using namespace FADS;

static LArHitsEventAction applEventAction;

LArHitsEventAction::LArHitsEventAction():
  ApplicationAction(),
  m_storedContainers(0),
  m_storedCalibContainers(0),
  m_timeBinType("Default"),
  m_timeBinWidth(2.5*CLHEP::ns),
  m_doMiniFcal(false),
  m_allowMods(false)
{
  FadsEventAction::GetEventAction()->SetApplicationAction(this);
  FadsRunAction::GetRunAction()->SetApplicationAction(this);
}

LArHitsEventAction::~LArHitsEventAction()
{
}

void LArHitsEventAction::EndOfRunAction(const G4Run*)
{
  delete m_storedContainers;
  delete m_storedCalibContainers;
}

void LArHitsEventAction::BeginOfRunAction(const G4Run*)
{
  // We have to defer the creation of collectionBuilder until
  // well into the application initialization process.  The reason is
  // the "static" declaration above; if we put the following statement
  // in the constructor, then the object will try to create
  // collectionBuilder immediately when the library is loaded.  The
  // problem is that collectionBuilder depends on the detector
  // description, which isn't loaded into memory until later.  The
  // following code attempts to get around that issue.

  // Global containers
  m_storedContainers = new StoredLArHitContainers();
  m_storedCalibContainers = new StoredLArCalibHitContainers();

  // Get the Message Service and the Detector Store
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode status = svcLocator->service("MessageSvc", m_msgSvc);
  MsgStream log(m_msgSvc, "LArHitsEventAction::BeginOfRunAction");
  if (status.isFailure())
  {
    log << MSG::FATAL << "MessageSvc not found!" << endreq;
    return;
  }
  else
    log << MSG::DEBUG << "MessageSvc initialized." << endreq;

  status = svcLocator->service("DetectorStore", m_detStore);
  if (status.isFailure())
  {
    log << MSG::FATAL << "Unable to get pointer to DetectorStore Service" << endreq;
    return;
  }
  else
    log << MSG::DEBUG << "DetectorStore Svc initialized." << endreq;

  // Get Global Options from Run Control - for Time Bin Width
  const LArG4GlobalOptions* glOpt=0;

  if(m_detStore->contains<LArG4GlobalOptions>("LArG4GlobalOptions"))
  {
    status = m_detStore->retrieve(glOpt,"LArG4GlobalOptions");

    if(status.isSuccess())
      log << MSG::DEBUG << "LArG4GlobalOptions retrieved!" << endreq;
    else
      log << MSG::DEBUG << "Unable to retrieve LArG4GlobalOptions! Using default value for TimeBinType = '" << m_timeBinType 
	  << "' and TimeBinWidth = " << m_timeBinWidth << " and AllowHitModification = " << m_allowMods << endreq;

    if(glOpt)
    {
      m_timeBinType = glOpt->TimeBinType();
      m_timeBinWidth = glOpt->TimeBinWidth();
      m_allowMods = glOpt->AllowHitModification();
    }
  }
  else
    log << MSG::DEBUG << "No LArG4GlobalOptions in Detector Store! Using default value for TimeBinType = '" << m_timeBinType 
	<< "' and TimeBinWidth = " << m_timeBinWidth <<  endreq;

  // Create Hit Merger Factories for regular and calibration hits and put them to TDS
  LArHitMergerFactory* _factory = new LArHitMergerFactory(m_detStore,this,m_timeBinType,m_timeBinWidth);

  status = m_detStore->record(dynamic_cast<LArVHitMergerFactory*>(_factory),"LArHitMergerFactory");
  if (status.isFailure()) 
    log << MSG::ERROR << "Unable to record HitMergerFactory" << endreq;
  else
    log << MSG::DEBUG << "HitMergerFactory recorded" << endreq;

  LArCalibHitMergerFactory* _calib_factory = new LArCalibHitMergerFactory(m_detStore,this);

  status = m_detStore->record(dynamic_cast<LArVCalibHitMergerFactory*>(_calib_factory),"LArCalibHitMergerFactory");
  if (status.isFailure()) {
    log << MSG::ERROR << "Unable to record CalibHitMergerFactory" << endreq;
    return;
  }
  else {
    log << MSG::DEBUG << "CalibHitMergerFactory recorded" << endreq;
  }

  // Check whether or not we need to write out Mini FCAL hits
  const CaloIdManager* caloIdManager;
  status = m_detStore->retrieve(caloIdManager);
  if(!status.isSuccess()) {
    log << MSG::ERROR << "Unable to retrieve CaloIdManager. Mini FCAL hits will not be persistified" << endreq;
    return;
  }

  const LArMiniFCAL_ID* larMiniFcalID = caloIdManager->getMiniFCAL_ID();
  m_doMiniFcal = (larMiniFcalID && larMiniFcalID->is_initialized());
  
}

void LArHitsEventAction::BeginOfEventAction(const G4Event*)
{

  m_storedContainers->embHitCollection = helper.RetrieveNonconstCollection<LArHitContainer>("LArHitEMB");
  m_storedContainers->emecHitCollection = helper.RetrieveNonconstCollection<LArHitContainer>("LArHitEMEC");
  m_storedContainers->fcalHitCollection = helper.RetrieveNonconstCollection<LArHitContainer>("LArHitFCAL");
  m_storedContainers->hecHitCollection = helper.RetrieveNonconstCollection<LArHitContainer>("LArHitHEC");
  if(m_doMiniFcal)
    m_storedContainers->miniFcalHitCollection = helper.RetrieveNonconstCollection<LArHitContainer>("LArHitMiniFCAL");
  
  m_storedCalibContainers->activeHitCollection = helper.RetrieveNonconstCollection<CaloCalibrationHitContainer>("LArCalibrationHitActive");
  m_storedCalibContainers->inactiveHitCollection = helper.RetrieveNonconstCollection<CaloCalibrationHitContainer>("LArCalibrationHitInactive");
  m_storedCalibContainers->deadHitCollection = helper.RetrieveNonconstCollection<CaloCalibrationHitContainer>("LArCalibrationHitDeadMaterial");

  /*
  m_storedContainers->embHitCollection = new LArHitContainer("LArHitEMB");
  m_storedContainers->emecHitCollection = new LArHitContainer("LArHitEMEC");
  m_storedContainers->fcalHitCollection = new LArHitContainer("LArHitFCAL");
  m_storedContainers->hecHitCollection = new LArHitContainer("LArHitHEC");
  if(m_doMiniFcal)
    m_storedContainers->miniFcalHitCollection = new LArHitContainer("LArHitMiniFCAL");

  m_storedCalibContainers->activeHitCollection = new CaloCalibrationHitContainer("LArCalibrationHitActive");
  m_storedCalibContainers->inactiveHitCollection = new CaloCalibrationHitContainer("LArCalibrationHitInactive");
  m_storedCalibContainers->deadHitCollection = new CaloCalibrationHitContainer("LArCalibrationHitDeadMaterial");
  */

}

void LArHitsEventAction::EndOfEventAction(const G4Event*)
{
  if (!m_allowMods) {
    helper.SetConstCollection< LArHitContainer >(m_storedContainers->embHitCollection);
    helper.SetConstCollection< LArHitContainer >(m_storedContainers->emecHitCollection);
    helper.SetConstCollection< LArHitContainer >(m_storedContainers->fcalHitCollection);
    helper.SetConstCollection< LArHitContainer >(m_storedContainers->hecHitCollection);
    if(m_doMiniFcal)
      helper.SetConstCollection< LArHitContainer >(m_storedContainers->miniFcalHitCollection);
    
    helper.SetConstCollection< CaloCalibrationHitContainer >(m_storedCalibContainers->activeHitCollection);
    helper.SetConstCollection< CaloCalibrationHitContainer >(m_storedCalibContainers->inactiveHitCollection);
    helper.SetConstCollection< CaloCalibrationHitContainer >(m_storedCalibContainers->deadHitCollection);    
  }
  
  /*
  helper.ExportCollection< LArHitContainer >(m_storedContainers->embHitCollection);
  helper.ExportCollection< LArHitContainer >(m_storedContainers->emecHitCollection);
  helper.ExportCollection< LArHitContainer >(m_storedContainers->fcalHitCollection);
  helper.ExportCollection< LArHitContainer >(m_storedContainers->hecHitCollection);
  if(m_doMiniFcal)
    helper.ExportCollection< LArHitContainer >(m_storedContainers->miniFcalHitCollection);

  helper.ExportCollection< CaloCalibrationHitContainer >(m_storedCalibContainers->activeHitCollection);
  helper.ExportCollection< CaloCalibrationHitContainer >(m_storedCalibContainers->inactiveHitCollection);
  helper.ExportCollection< CaloCalibrationHitContainer >(m_storedCalibContainers->deadHitCollection);
  */
}
