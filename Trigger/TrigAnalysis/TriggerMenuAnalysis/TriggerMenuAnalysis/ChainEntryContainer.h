/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSIS_CHAINENTRYCONTAINER_H
#define TRIGGERMENUANALYSIS_CHAINENTRYCONTAINER_H
/*
  ChainEntryContainer.h
*/
#include "AthContainers/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "TriggerMenuNtuple/ChainEntry.h"

typedef DataVector<ChainEntry> ChainEntryContainer;

//CLASS_DEF( DataVector<ChainEntry> , 103090914 , 1 )
CLASS_DEF( ChainEntryContainer , 1338527445 , 1 )

#endif // TRIGGERMENUANALYSIS_CHAINENTRYCONTAINER_H
