/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_CONFIG_COLLECTION_H
#define TRIGMON_CONFIG_COLLECTION_H

#include "AthContainers/DataVector.h"
#include "TrigMonitoringEvent/TrigMonConfig.h"

class TrigMonConfigCollection : public DataVector<TrigMonConfig> {

};

SG_BASE( TrigMonConfigCollection, DataVector< TrigMonConfig > );
CLASS_DEF( TrigMonConfigCollection , 1230728709 , 1 )

#endif
