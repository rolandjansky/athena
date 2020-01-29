/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_EVENT_COLLECTION_H
#define TRIGMON_EVENT_COLLECTION_H

#include "AthContainers/DataVector.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

class TrigMonEventCollection : public DataVector<TrigMonEvent> {

};

SG_BASE( TrigMonEventCollection, DataVector< TrigMonEvent > );
CLASS_DEF( TrigMonEventCollection , 1271256426 , 1 )

#endif
