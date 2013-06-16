/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventReader.cxx 364083 2011-05-06 09:09:55Z krasznaa $

// Local include(s):
#include "EventReader.h"

namespace LVL1MUCTPI {

   EventReader::EventReader()
      : m_logger( "EventReader" ),
        m_infoString( "member function isValid() not yet called" ) {

   }

   EventReader::~EventReader() {

   }

} // namespace LVL1MUCTPI
