/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventID.cxx 362102 2011-04-28 13:17:28Z krasznaa $

// STL include(s):
#include <cassert>
#include <iostream>

// Local include(s):
#include "EventID.h"
#include "EventIDSingleton.h"

/*******************************************************************
 * $Date: 2011-04-28 15:17:28 +0200 (Thu, 28 Apr 2011) $
 *
 * Implementation of class EventID
 * @author   Author: Thorsten Wengler
 * @version $Revision: 362102 $
 ******************************************************************/

namespace LVL1MUCTPI {

  EventID::EventID() {

    // get the pointer to the EventID Singleton
    m_eventIDKeeper = EventIDSingleton::instance();
    if( !m_eventIDKeeper ) {
      std::cout << "EventIDSingleton.Instance did not return a pointer" << std::endl;
      assert( 0 ) ;
    }

  }

  EventID::~EventID() {

  }

  void EventID::resetBCID() {

    m_eventIDKeeper->resetBCID();
    return;

  }

  void EventID::BCID( unsigned int newBCID ) {

    m_eventIDKeeper->BCID( newBCID );
    return;

  }

  unsigned int EventID::BCID() {

    return m_eventIDKeeper->BCID();

  }

  void EventID::resetEvID() {

    m_eventIDKeeper->resetEvID();
    return;

  }

  void EventID::setEvID( unsigned int newID ) {

    m_eventIDKeeper->setEvID( newID );
    return;

  }

  void EventID::incrementEvID() {

    m_eventIDKeeper->incrementEvID();
    return;

  }

  unsigned int EventID::getEvID() {

    return m_eventIDKeeper->getEvID();

  }

  unsigned int EventID::getTriggerType() {

    return m_eventIDKeeper->getTriggerType();

  }

  void EventID::setTriggerType( unsigned int theType ) {

    m_eventIDKeeper->setTriggerType( theType );
    return;

  }

  bool EventID::getMonitorEvent() {

    return m_eventIDKeeper->getMonitorEvent();

  }

  void EventID::setMonitorEvent( bool doMonitor ) {

    m_eventIDKeeper->setMonitorEvent( doMonitor );
    return;

  }

  unsigned int EventID::getRunNumber() {

    return m_eventIDKeeper->getRunNumber();

  }

  void EventID::setRunNumber( unsigned int runNumber ) {

    m_eventIDKeeper->setRunNumber( runNumber );
    return;

  }

  unsigned int EventID::getECRC() {

    return m_eventIDKeeper->getECRC();

  }

  void EventID::setECRC( unsigned int ECRC ) {

    m_eventIDKeeper->setECRC( ECRC );
    return;

  }

} // namespace LVL1MUCTPI
