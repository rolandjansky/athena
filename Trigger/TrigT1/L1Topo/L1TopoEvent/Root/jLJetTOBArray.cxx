/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoEvent/jLJetTOBArray.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/GenericTOB.h"


TCS::jLJetTOBArray::jLJetTOBArray(const std::string & name, unsigned int reserve) :
   InputTOBArray(name),
   DataArrayImpl<jLJetTOB>(reserve)
{}


TCS::TOBArray
TCS::jLJetTOBArray::asTOBArray() const {
   TOBArray tobarray(name());
   for(const jLJetTOB * jet : m_data)
      tobarray.push_back( GenericTOB(*jet));
   return tobarray;
}

void
TCS::jLJetTOBArray::print(std::ostream &o) const {
   o << name() << std::endl;
   for(const_iterator tob = begin(); tob != end(); ++tob)
      o << **tob << std::endl;
}
