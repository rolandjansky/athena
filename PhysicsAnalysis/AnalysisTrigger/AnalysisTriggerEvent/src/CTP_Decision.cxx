/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CTP_Decision.cxx,v 1.2 2008-05-08 15:00:11 krasznaa Exp $

// STL include(s):
#include <sstream>
#include <iomanip>

// Local include(s):
#include "AnalysisTriggerEvent/CTP_Decision.h"

const std::string CTP_Decision::dump() const {

   std::ostringstream s;

   for( size_t i( 0 ); i < m_CTPResultWord.size(); ++i ) {
      s << std::setw(2) << i 
        << " 0x" << std::hex << std::setw(8) << std::setfill('0') << m_CTPResultWord[i] 
        << std::dec << std::setfill(' ') << std::endl;
   }
   s << m_triggerTypeWord << std::endl;
    
   for( std::vector<std::string>::const_iterator item = m_items.begin();
        item != m_items.end(); ++item ) {
      s << *item << " ";
   }

   return s.str();
}
