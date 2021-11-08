/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoEvent/gLargeRJetTOBArray.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/GenericTOB.h"


TCS::gLargeRJetTOBArray::gLargeRJetTOBArray(const std::string & name, unsigned int reserve) :
   InputTOBArray(name),
   DataArrayImpl<gLargeRJetTOB>(reserve)
{}


TCS::TOBArray
TCS::gLargeRJetTOBArray::asTOBArray() const {
   TOBArray tobarray(name());
   for(const gLargeRJetTOB * jet : m_data)
      tobarray.push_back( GenericTOB(*jet));
   return tobarray;
}

void
TCS::gLargeRJetTOBArray::print(std::ostream &o) const {
   o << name() << std::endl;
   for(const_iterator tob = begin(); tob != end(); ++tob)
      o << **tob << std::endl;
}
