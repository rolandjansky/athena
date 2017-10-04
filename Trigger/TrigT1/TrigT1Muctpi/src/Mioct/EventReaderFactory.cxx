/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventReaderFactory.cxx 472835 2011-12-06 21:05:16Z stelzer $

// Local include(s):
#include "EventReaderFactory.h"
#include "EventReader.h"
#include "SectorLogicReader.h"
#include "../Logging/MsgLogger.h"

namespace LVL1MUCTPI {

   EventReader* EventReaderFactory::getEventReader() {

      MsgLogger logger( "EventReaderFactory" );
      REPORT_MSG2(DEBUG, "Using a SectorLogicReader", logger);
      return new SectorLogicReader();
   }

} // namespace LVL1MUCTPI
