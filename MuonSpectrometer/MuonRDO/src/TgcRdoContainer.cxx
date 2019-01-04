/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "MuonRDO/TgcRdoContainer.h"
#include "EventContainers/SelectAllObject.h"

// Default constructor.
TgcRdoContainer::TgcRdoContainer()
  : IdentifiableContainer<TgcRdo>(hashFcn().max()) 
{
}

// Default constructor.
TgcRdoContainer::TgcRdoContainer(unsigned int hashmax)
  : IdentifiableContainer<TgcRdo>(hashmax) 
{
}

TgcRdoContainer::TgcRdoContainer(TgcRdo_Cache* cache)
  : IdentifiableContainer<TgcRdo>(cache) 
{
}


// Destructor.
TgcRdoContainer::~TgcRdoContainer() 
{}

/// Convert identifier to idhash
unsigned int 
TgcRdoContainer::idToHash(unsigned int id) const
{
    return (hashFcn()(id));
}

// Class ID
const CLID& TgcRdoContainer::classID()
{
  return ClassID_traits<TgcRdoContainer>::ID();       
}

const TgcRdoIdHash&
TgcRdoContainer::hashFcn()
{
    static const TgcRdoIdHash hashFcn;
    return(hashFcn);
}


// Insert a RawData
void TgcRdoContainer::push_back(TgcRawData * /*rawData*/)
{
  // uint16_t rawDataId = TgcRdo::identifyRawData(*rawData);
  // 
  // // find TgcRdo
  // MyBase::const_iterator it = MyBase::indexFind(rawDataId);
  // if(it!=MyBase::end())
  //   {
  //     const TgcRdo * const_rdo = *it;
  //     TgcRdo * rdo = const_cast<TgcRdo *>(const_rdo);
  //     rdo->push_back(rawData); 
  //   } 
  // 
  // return ; 
}
