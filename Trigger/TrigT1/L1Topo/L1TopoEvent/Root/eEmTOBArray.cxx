// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/eEmTOBArray.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/GenericTOB.h"

TCS::eEmTOBArray::eEmTOBArray(const std::string & name, unsigned int reserve) :
   InputTOBArray(name),
   DataArrayImpl<eEmTOB>(reserve)
{}

void
TCS::eEmTOBArray::print(std::ostream &o) const {
   o << name() << std::endl;
   for(const_iterator tob = begin(); tob != end(); ++tob)
      o << **tob << std::endl;
}

