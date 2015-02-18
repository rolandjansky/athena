/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H6SD/LArHitsEventAction.h"
#include "LArG4H6SD/LArG4H62004MergerFactory.h"
#include "LArG4H6SD/LArG4H6EmecMergerFactory.h"

#include "LArSimEvent/LArHitContainer.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

#include "FadsActions/FadsEventAction.h"
#include "FadsActions/FadsRunAction.h"

#include "G4Run.hh"
#include "G4Event.hh"

#include <iostream>
#include <vector>

#undef DEBUG_HITS

using namespace FADS;

LArTBECHitsEventAction::LArTBECHitsEventAction(): ApplicationAction()
{
	FadsEventAction::GetEventAction()->SetApplicationAction(this);
	FadsRunAction::GetRunAction()->SetApplicationAction(this);
}

LArTBECHitsEventAction::~LArTBECHitsEventAction(){}

void LArTBECHitsEventAction::BeginOfRunAction(const G4Run*)
{
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode status = svcLocator->service("MessageSvc", m_msgSvc);
  MsgStream log(m_msgSvc, "TBECSim::BeginOfRunAction");
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

  m_storedContainers = new TBStoredLArHitContainers();

  status = m_detStore->record(m_storedContainers,"TBLArHitContainers");
  if (status.isFailure()) 
    log << MSG::ERROR << "Unable to record Containers" << endreq;
  else
    log << MSG::DEBUG << "Containers recorded" << endreq;

  // Create Hit Merger Factory and put it to TDS
  LArG4H6EmecMergerFactory* _efactory = new LArG4H6EmecMergerFactory(m_detStore,this,m_timeBinWidth);

  status = m_detStore->record(dynamic_cast<LArVHitMergerFactory*>(_efactory),"LArG4EmecMergerFactory");
  if (status.isFailure()) 
    log << MSG::ERROR << "Unable to record LArG4EmecMergerFactory" << endreq;
  else
    log << MSG::DEBUG << "LArG4EmecMergerFactory recorded" << endreq;



  // Create Hit Merger Factory and put it to TDS
  LArG4H6EmecMergerFactory* _24factory = new LArG4H62004MergerFactory(m_detStore,this,m_timeBinWidth);

  status = m_detStore->record(dynamic_cast<LArVHitMergerFactory*>(_24factory),"LArG42004MergerFactory");
  if (status.isFailure()) 
    log << MSG::ERROR << "Unable to record LArG42004MergerFactory" << endreq;
  else
    log << MSG::DEBUG << "LArG42004MergerFactory recorded" << endreq;

}

static const char *list[7] = {
	"gapadj", "gapold", "gap_e", "gap_s", "gap_se", "chcoll", "ropt"
};

void LArTBECHitsEventAction::BeginOfEventAction(const G4Event*)
{
	for(int i = 0; i < 7; i ++){
		std::string a = "LArHitEMEC_";
		a += list[i];
		m_storedContainers->emecHitCollection[a] = new LArHitContainer(a.c_str());
	}
}

void LArTBECHitsEventAction::EndOfEventAction(const G4Event*)
{
	for(int i = 0; i < 7; i ++){
		std::string a = "LArHitEMEC_";
		a += list[i];
		helper.ExportCollection< LArHitContainer >(m_storedContainers->emecHitCollection[a]);
	}
}
