//
//  ClusterTOBArray.cxx
//  TopoCore
//
//  Created by Joerg Stelzer on 11/17/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.
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
