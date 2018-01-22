//  MuonNextBCTOBArray.cxx
//  TopoCore
//  Based on MuonTOBArray Created by Joerg Stelzer on 11/17/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#include "L1TopoEvent/MuonNextBCTOBArray.h"

TCS::MuonNextBCTOBArray::MuonNextBCTOBArray(const std::string & name, unsigned int reserve) :
   InputTOBArray(name),
   DataArrayImpl<MuonNextBCTOB>(reserve)
{}

void
TCS::MuonNextBCTOBArray::print(std::ostream &o) const {
   o << name() << std::endl;
   for(const_iterator tob = begin(); tob != end(); ++tob)
      o << **tob << std::endl;
}
