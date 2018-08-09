/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  DataArray.cxx
//  L1TopoEvent
//  Created by Joerg Stelzer on 11/22/12.

#include "L1TopoEvent/DataArray.h"


namespace TCS {


std::ostream &
operator<< (std::ostream & o, const TCS::DataArray& array) {
   array.print(o);
   return o;
}



}
