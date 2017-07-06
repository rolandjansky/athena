//  LateMuonTOBArray.cxx
//  TopoCore
//  Based on MuonTOBArray created by Joerg Stelzer on 11/17/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#include "L1TopoEvent/LateMuonTOBArray.h"

TCS::LateMuonTOBArray::LateMuonTOBArray(const std::string & name, unsigned int reserve) :
   InputTOBArray(name),
   DataArrayImpl<LateMuonTOB>(reserve)
{}

void
TCS::LateMuonTOBArray::print(std::ostream &o) const {
   o << name() << std::endl;
   for(const_iterator tob = begin(); tob != end(); ++tob)
      o << **tob << std::endl;
}
