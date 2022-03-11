// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/jXETOBArray.h"

TCS::jXETOBArray::jXETOBArray(const std::string & name, unsigned int reserve) :
   InputTOBArray(name),
   DataArrayImpl<jXETOB>(reserve)
{}

void
TCS::jXETOBArray::print(std::ostream &o) const {
   o << name() << std::endl;
   for(const_iterator tob = begin(); tob != end(); ++tob)
      o << **tob << std::endl;
}
