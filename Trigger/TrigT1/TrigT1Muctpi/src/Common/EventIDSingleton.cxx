/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventIDSingleton.cxx 362102 2011-04-28 13:17:28Z krasznaa $

// Local include(s):
#include "EventIDSingleton.h"

/*******************************************************************
 * $Date: 2011-04-28 15:17:28 +0200 (Thu, 28 Apr 2011) $
 *
 * Implementation of class EventIDSingleton
 * @author   Author: Thorsten Wengler
 * @version $Revision: 362102 $
 ******************************************************************/

namespace LVL1MUCTPI {

  EventIDSingleton* EventIDSingleton::m_theOnlyEventID = 0;

  EventIDSingleton::EventIDSingleton()
    : m_eventID( 0 ), m_ECRC( 0 ), m_triggerType( 0 ), m_BCID( 0 ),
      m_runNumber( 0 ), m_monitorEventBit( false ) {

  }

  EventIDSingleton::~EventIDSingleton() {

  }

  EventIDSingleton* EventIDSingleton::instance() {

    if ( m_theOnlyEventID == 0 ) {
      m_theOnlyEventID = new EventIDSingleton();
    }
    return m_theOnlyEventID;

  }

} // namespace LVL1MUCTPI
