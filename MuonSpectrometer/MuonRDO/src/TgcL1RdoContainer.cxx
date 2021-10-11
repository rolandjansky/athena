/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "MuonRDO/TgcL1RdoContainer.h"
#include "EventContainers/SelectAllObject.h"

// Default constructor.
TgcL1RdoContainer::TgcL1RdoContainer()
  : IdentifiableContainer<TgcL1Rdo>(hashFcn().max()) 
{
}

// Default constructor.
TgcL1RdoContainer::TgcL1RdoContainer(unsigned int hashmax)
  : IdentifiableContainer<TgcL1Rdo>(hashmax) 
{
}

TgcL1RdoContainer::TgcL1RdoContainer(TgcL1Rdo_Cache* cache)
  : IdentifiableContainer<TgcL1Rdo>(cache) 
{
}


// Destructor.
TgcL1RdoContainer::~TgcL1RdoContainer() 
{}

/// Convert identifier to idhash
unsigned int 
TgcL1RdoContainer::idToHash(unsigned int id) const
{
    return (hashFcn()(id));
}

// Class ID
const CLID& TgcL1RdoContainer::classID()
{
  return ClassID_traits<TgcL1RdoContainer>::ID();       
}

const TgcL1RdoIdHash&
TgcL1RdoContainer::hashFcn()
{
    static const TgcL1RdoIdHash hashFcn;
    return(hashFcn);
}

void TgcL1RdoContainer::push_back(TgcL1RawData * /*rawData*/)
{
  // Kept for consistency
}
