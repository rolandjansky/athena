/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/STGC_RawDataContainer.h"
#include <iostream>

Muon::STGC_RawDataContainer::STGC_RawDataContainer()
  : IdentifiableContainer<STGC_RawDataCollection>(0) 
{
}

Muon::STGC_RawDataContainer::STGC_RawDataContainer(unsigned int hashmax)
: IdentifiableContainer<STGC_RawDataCollection>(hashmax) 
{
}

Muon::STGC_RawDataContainer::STGC_RawDataContainer(STGC_RawDataCollection_Cache* cache)
: IdentifiableContainer<STGC_RawDataCollection>(cache) 
{
}


// Destructor.
Muon::STGC_RawDataContainer::~STGC_RawDataContainer() {

}

const CLID& Muon::STGC_RawDataContainer::classID()    
{
  return ClassID_traits<STGC_RawDataContainer>::ID();       
}


// Output stream.
std::ostream& operator<<(std::ostream& lhs, const Muon::STGC_RawDataContainer& rhs) {
  lhs << "STGC_RawDataContainer has " << rhs.size() << " collections:" << std::endl;
  for (auto col : rhs ){
    lhs << "Collection with hash ["<<col->identifyHash()<<"] : " << std::endl;
    for (auto rdo : *col ){
      lhs << *rdo;
    }
  }
  return lhs;
}

