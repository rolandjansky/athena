/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/MM_RawDataContainer.h"
#include <iostream>
#include "EventContainers/SelectAllObject.h"

Muon::MM_RawDataContainer::MM_RawDataContainer()
: IdentifiableContainer<MM_RawDataCollection>(0) 
{
}

Muon::MM_RawDataContainer::MM_RawDataContainer(unsigned int hashmax)
: IdentifiableContainer<MM_RawDataCollection>(hashmax) 
{
}

Muon::MM_RawDataContainer::MM_RawDataContainer(MM_RawDataCollection_Cache* cache)
: IdentifiableContainer<MM_RawDataCollection>(cache) 
{
}

// Destructor.
Muon::MM_RawDataContainer::~MM_RawDataContainer() {

}

const CLID& Muon::MM_RawDataContainer::classID()    
{
  return ClassID_traits<MM_RawDataContainer>::ID();       
}


// Output stream.
std::ostream& operator<<(std::ostream& lhs, const Muon::MM_RawDataContainer& rhs) {
  lhs << "MM_RawDataContainer has " << rhs.size() << " collections:" << std::endl;
  for (auto col : rhs ){
    lhs << "Collection with hash ["<<col->identifierHash()<<"] : " << std::endl;
    for (auto rdo : *col ){
      lhs << *rdo;
    }
  }
  return lhs;
}

