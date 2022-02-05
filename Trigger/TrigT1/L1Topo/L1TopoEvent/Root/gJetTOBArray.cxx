/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoEvent/gJetTOBArray.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/GenericTOB.h"


TCS::gJetTOBArray::gJetTOBArray(const std::string & name, unsigned int reserve) :
   InputTOBArray(name),
   DataArrayImpl<gJetTOB>(reserve)
{}


TCS::TOBArray
TCS::gJetTOBArray::asTOBArray() const {
   TOBArray tobarray(name());
   for(const gJetTOB * jet : m_data)
      tobarray.push_back( GenericTOB(*jet));
   return tobarray;
}

void
TCS::gJetTOBArray::print(std::ostream &o) const {
   o << name() << std::endl;
   for(const_iterator tob = begin(); tob != end(); ++tob)
      o << **tob << std::endl;
}
