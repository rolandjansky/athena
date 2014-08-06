/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigCostD3PDUtils.cxx tamartin $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TrigCostD3PDUtils.h"

// EDM include(s):
#include "TrigMonitoringEvent/TrigMonEvent.h"
#include "TrigMonitoringEvent/TrigMonEventCollection.h"

namespace D3PD {

  const TrigMonEvent* getObjectFromVector(const TrigMonEventCollection& eventCollection, uint32_t objectLocation) {
  
    // Check requesting a valid entry in the DataVector
    if ( objectLocation >= eventCollection.size() ) {
      return 0;
    }

    if( eventCollection.size() == 0 ) {
      return 0;
    }
    
    return eventCollection.at( objectLocation );
  }

}
