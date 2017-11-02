//  ClusterTOBArray.cxx
//  TopoCore
//  Created by Joerg Stelzer on 11/17/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#include "L1TopoEvent/ClusterTOBArray.h"

TCS::ClusterTOBArray::ClusterTOBArray(const std::string & name, unsigned int reserve) :
   InputTOBArray(name),
   DataArrayImpl<ClusterTOB>(reserve)
{}

void
TCS::ClusterTOBArray::print(std::ostream &o) const {
   o << name() << std::endl;
   for(const_iterator tob = begin(); tob != end(); ++tob)
      o << **tob << std::endl;
}

