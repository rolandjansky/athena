//  TOBArray.cxx
//  TopoCore
//  Created by Joerg Stelzer on 11/17/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/CompositeTOB.h"
#include "L1TopoEvent/InputTOBArray.h"
#include "L1TopoEvent/JetTOBArray.h"
#include "L1TopoEvent/ClusterTOBArray.h"

#include <algorithm>

void TCS::TOBArray::print(std::ostream &o) const {
   for(const_iterator tob = m_data.begin(); tob != m_data.end(); ++tob) {
      if( tob!=begin() ) o << std::endl;
      o << **tob;
   }
}

void
TCS::TOBArray::push_back(const TCS::CompositeTOB& tob) {
   m_data.push_back(CompositeTOB::createOnHeap(tob));
}

void
TCS::TOBArray::sort(sort_fnc fnc) {
   std::sort(m_data.begin(), m_data.end(), fnc);
}
