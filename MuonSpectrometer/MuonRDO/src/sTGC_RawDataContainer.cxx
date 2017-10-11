/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/sTGC_RawDataContainer.h"
#include <iostream>

sTGC_RawDataContainer::sTGC_RawDataContainer(unsigned int hashmax)
: IdentifiableContainer<sTGC_RawDataCollection>(hashmax) 
{
}

// Destructor.
sTGC_RawDataContainer::~sTGC_RawDataContainer() {

}

const CLID& sTGC_RawDataContainer::classID()    
{
  return ClassID_traits<sTGC_RawDataContainer>::ID();       
}


// Output stream.
std::ostream& operator<<(std::ostream& lhs, const sTGC_RawDataContainer& rhs) {
  lhs << "sTGC_RawDataContainer has " << rhs.size() << " collections:" << std::endl;
  for (auto col : rhs ){
    lhs << "Collection with hash ["<<col->identifierHash()<<"] : " << std::endl;
    for (auto rdo : *col ){
      lhs << *rdo;
    }
  }
  return lhs;
}

