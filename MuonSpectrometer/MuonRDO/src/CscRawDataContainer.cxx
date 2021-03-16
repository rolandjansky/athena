/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/CscRawDataContainer.h"
#include "EventContainers/SelectAllObject.h"

// Default constructor.
CscRawDataContainer::CscRawDataContainer()
  : IdentifiableContainer<CscRawDataCollection>(hashFcn().max()) 
{
}

CscRawDataContainer::CscRawDataContainer(unsigned int hashmax)
  : IdentifiableContainer<CscRawDataCollection>(hashmax) 
{
}

CscRawDataContainer::CscRawDataContainer(CscRawDataCollection_Cache* cache)
  : IdentifiableContainer<CscRawDataCollection>(cache) 
{
}

// Destructor.
CscRawDataContainer::~CscRawDataContainer() 
{}

/// Convert identifier to idhash
unsigned int 
CscRawDataContainer::idToHash(unsigned int id) const
{
    return (hashFcn()(id));
}

/// Convert identifier to idhash
const CscRawDataCollectionIdHash& 
CscRawDataContainer::hashFcn()
{

    static const CscRawDataCollectionIdHash hashFcn;
    return (hashFcn);
}

// Class ID
const CLID& CscRawDataContainer::classID()
{
  return ClassID_traits<CscRawDataContainer>::ID();       
}


// Insert a RawData
void CscRawDataContainer::push_back(CscRawData * /*rawData*/)
{
}
