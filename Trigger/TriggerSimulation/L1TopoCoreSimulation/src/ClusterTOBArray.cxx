/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  ClusterTOBArray.cxx
//  TopoCore
//
//  Created by Joerg Stelzer on 11/17/12.
//

#include "L1TopoCoreSimulation/ClusterTOBArray.h"

void
TCS::ClusterTOBArray::push_back(const TCS::ClusterTOB& tob) {
   m_data.push_back(ClusterTOB::createOnHeap(tob));
}


void
TCS::ClusterTOBArray::print(std::ostream &o) const {
   for(const_iterator tob = begin(); tob != end(); ++tob) {
      if( tob!=begin() ) o << std::endl;
      o << **tob;
   }
}
