// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "L1TopoEvent/gMHTTOBArray.h"

TCS::gMHTTOBArray::gMHTTOBArray(const std::string & name, unsigned int reserve) :
   InputTOBArray(name),
   DataArrayImpl<gMHTTOB>(reserve)
{}

void
TCS::gMHTTOBArray::print(std::ostream &o) const {
   o << name() << std::endl;
   for(const_iterator tob = begin(); tob != end(); ++tob)
      o << **tob << std::endl;
}
