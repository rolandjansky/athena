/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoEvent/jLargeRJetTOBArray.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/GenericTOB.h"


TCS::jLargeRJetTOBArray::jLargeRJetTOBArray(const std::string & name, unsigned int reserve) :
   InputTOBArray(name),
   DataArrayImpl<jLargeRJetTOB>(reserve)
{}


TCS::TOBArray
TCS::jLargeRJetTOBArray::asTOBArray() const {
   TOBArray tobarray(name());
   for(const jLargeRJetTOB * jet : m_data)
      tobarray.push_back( GenericTOB(*jet));
   return tobarray;
}

void
TCS::jLargeRJetTOBArray::print(std::ostream &o) const {
   o << name() << std::endl;
   for(const_iterator tob = begin(); tob != end(); ++tob)
      o << **tob << std::endl;
}
