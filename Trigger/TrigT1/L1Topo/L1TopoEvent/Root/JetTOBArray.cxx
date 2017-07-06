//  JetTOBArray.cxx
//  TopoCore
//  Created by Joerg Stelzer on 11/17/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.
// cw added

#include "L1TopoEvent/JetTOBArray.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/GenericTOB.h"


TCS::JetTOBArray::JetTOBArray(const std::string & name, unsigned int reserve) :
   InputTOBArray(name),
   DataArrayImpl<JetTOB>(reserve)
{}


TCS::TOBArray
TCS::JetTOBArray::asTOBArray(JetTOB::JetSize jetsize) const {
   TOBArray tobarray(name());
   for(const JetTOB * jet : m_data)
      tobarray.push_back( GenericTOB(*jet, jetsize));
   return tobarray;
}

void
TCS::JetTOBArray::print(std::ostream &o) const {
   o << name() << std::endl;
   for(const_iterator tob = begin(); tob != end(); ++tob)
      o << **tob << std::endl;
}
