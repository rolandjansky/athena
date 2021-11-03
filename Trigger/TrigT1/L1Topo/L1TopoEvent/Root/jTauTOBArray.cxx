// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/jTauTOBArray.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/GenericTOB.h"

TCS::jTauTOBArray::jTauTOBArray(const std::string & name, unsigned int reserve) :
   InputTOBArray(name),
   DataArrayImpl<jTauTOB>(reserve)
{}


TCS::TOBArray
TCS::jTauTOBArray::asTOBArray() const {
  TOBArray tobarray(name());
  for(const jTauTOB * jet : m_data)
    tobarray.push_back( GenericTOB(*jet));
  return tobarray;
}

void
TCS::jTauTOBArray::print(std::ostream &o) const {
   o << name() << std::endl;
   for(const_iterator tob = begin(); tob != end(); ++tob)
      o << **tob << std::endl;
}

