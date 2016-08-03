/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/SegMemSvc.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentListener.h"

using namespace std;


//
///////////////////////////////////////////////////////////////////////////
//

SegMemSvc::SegMemSvc( const std::string& name, ISvcLocator* svc )
  : Service( name, svc ), p_incSvc("IncidentSvc",name),
    m_log(msgSvc(), name), 
    m_arena_job("sms_job",&m_ahead_job), 
    m_arena_evt("sms_evt",&m_ahead_evt), 
    m_arena_inc("sms_inc",&m_ahead_inc) 
{

}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

SegMemSvc::~SegMemSvc() {

}


StatusCode SegMemSvc::queryInterface( const InterfaceID& riid, 
                                      void** ppvInterface ) {
  StatusCode sc = StatusCode::FAILURE;
  if ( ppvInterface ) {
    *ppvInterface = 0;
    
    if ( SegMemSvc::interfaceID().versionMatch(riid) )    {
      *ppvInterface = static_cast<SegMemSvc*>(this);
      sc = StatusCode::SUCCESS;
      addRef();
    }
    else
      sc = Service::queryInterface( riid, ppvInterface );    
  }
  return sc;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode
SegMemSvc::initialize() {

  SG::Arena::Push push(m_arena_job);
  SG::Arena::Push push2(m_arena_evt);
  SG::Arena::Push push3(m_arena_inc);

  //   m_ahead.addArena(&m_arena);

  
  p_incSvc->addListener( this, "EndEvent" );
  p_incSvc->addListener( this, "DefragMemory" );

  return StatusCode::SUCCESS;

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode
SegMemSvc::reinitialize() {

  return StatusCode::SUCCESS;

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode
SegMemSvc::finalize() {

  return StatusCode::SUCCESS;

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void 
SegMemSvc::handle(const Incident& inc) {

  if (inc.type() == "EndEvent") {
    if (m_log.level() <= MSG::DEBUG) {
      m_log << MSG::DEBUG << "Running report for " << m_arena_evt.name() 
            << std::endl;
      std::ostringstream ost;
      m_ahead_evt.report(ost);
      m_log << MSG::DEBUG << ost.str() << endmsg;
      //       m_log << MSG::DEBUG << "header: " << m_ahead.reportStr() << endmsg;
      
    }

    m_log << MSG::DEBUG << "freeing all memory allocated for Event"
          << endmsg;
    m_arena_evt.reset();
    m_ahead_evt.reset();

  } else if ( inc.type() == "DefragMemory") {

    if (m_log.level() <= MSG::DEBUG) {
      m_log << MSG::DEBUG << "defragmenting memory"
            << endmsg;
    }
    m_log << MSG::DEBUG << "freeing all memory allocated associated with "
          << "DefragMemory incident"
          << endmsg;
    m_arena_inc.reset();
    m_ahead_inc.reset();
  }

}    
    
