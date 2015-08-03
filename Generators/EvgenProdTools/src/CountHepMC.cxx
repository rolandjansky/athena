/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#include "EvgenProdTools/CountHepMC.h"
#include "GaudiKernel/IEventProcessor.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ClassID.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/errorcheck.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include <cmath>
#include <cassert>

using namespace std;


CountHepMC::CountHepMC(const std::string& name, ISvcLocator* pSvcLocator) :
  GenBase(name, pSvcLocator),
  m_nPass(0)
{
  declareProperty("RequestedOutput", m_nCount=5000);
  declareProperty("FirstEvent",      m_firstEv=1);
  declareProperty("CorrectHepMC",    m_corHepMC=false);
  declareProperty("CorrectEventID",  m_corEvtID=false);
}


CountHepMC::~CountHepMC() {

}


StatusCode CountHepMC::initialize() {
  CHECK(GenBase::initialize());

  m_nPass = 0;

  return StatusCode::SUCCESS;
}


StatusCode CountHepMC::execute() {

  /// @todo Replace the old event ?
  m_nPass++;
  ATH_MSG_DEBUG("Current count = " << m_nPass);

  // Fix the event number
  int newnum = m_nPass + m_firstEv - 1;

  if (m_corHepMC) {
    // Replace the old event
    HepMC::GenEvent* new_evt = new HepMC::GenEvent(*event());
    new_evt->set_event_number(newnum);
    events()->pop_back();
    events()->push_back(new_evt);
  }

  if (m_corEvtID) {
    // Change the EventID in the eventinfo header
    const EventInfo* pInputEvt(0);
    if (evtStore()->retrieve(pInputEvt).isSuccess()) {
      assert(pInputEvt);
      EventID* eventID = const_cast<EventID*>(pInputEvt->event_ID());
      eventID->set_event_number(newnum);
      ATH_MSG_DEBUG("Set new event number in event_ID");
    } else {
      ATH_MSG_ERROR("No EventInfo object found");
      return StatusCode::SUCCESS;
    }
  }

  if (m_nPass == m_nCount) {
    ATH_MSG_INFO("Stopping the event processing...." << m_nPass << "/" << m_nCount);
    IEventProcessor* apm = 0;
    IService* ssvc = 0;
    static const bool CRE(false);
    StatusCode sc = serviceLocator()->getService("AthenaEventLoopMgr", ssvc, CRE);
    if (sc.isSuccess()) {
      sc = ssvc->queryInterface(IEventProcessor::interfaceID(), pp_cast<void>(&apm));
    } else {
      ATH_MSG_WARNING("No EventLoop Manager found ");
    }
    if (sc.isSuccess()) {
      if ((apm->stopRun()).isFailure()) {
        ATH_MSG_ERROR("StatusCode Failure in stopRun()");
        return StatusCode::FAILURE;
      }
      apm->release();
    } else {
      ATH_MSG_WARNING("No EventLoop Manager specified ");
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode CountHepMC::finalize() {
  ATH_MSG_INFO("Events passing all checks and written = " << m_nPass);
  return StatusCode::SUCCESS;
}

#endif
