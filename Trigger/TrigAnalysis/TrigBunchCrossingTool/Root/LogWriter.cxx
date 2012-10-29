/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LogWriter.cxx 457114 2011-09-05 09:35:49Z krasznaa $

// Local include(s):
#include "TrigBunchCrossingTool/LogWriter.h"

namespace Trig {

   LogWriter::LogWriter( const std::string& name )
      : m_logger( name ) {

   }

   MsgLogger& LogWriter::log() const {

      return m_logger;
   }

} // namespace Trig
