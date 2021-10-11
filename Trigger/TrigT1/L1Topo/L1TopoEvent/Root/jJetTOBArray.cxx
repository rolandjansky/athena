/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoEvent/jJetTOBArray.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/GenericTOB.h"


TCS::jJetTOBArray::jJetTOBArray(const std::string & name, unsigned int reserve) :
   InputTOBArray(name),
   DataArrayImpl<jJetTOB>(reserve)
{}


TCS::TOBArray
TCS::jJetTOBArray::asTOBArray() const {
   TOBArray tobarray(name());
   for(const jJetTOB * jet : m_data)
      tobarray.push_back( GenericTOB(*jet));
   return tobarray;
}

void
TCS::jJetTOBArray::print(std::ostream &o) const {
   o << name() << std::endl;
   for(const_iterator tob = begin(); tob != end(); ++tob)
      o << **tob << std::endl;
}
