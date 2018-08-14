//
//  DataArray.cxx
//  L1TopoCoreSimulation
//
//  Created by Joerg Stelzer on 11/22/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.
//

#include "L1TopoCoreSimulation/DataArray.h"

std::ostream &
operator<< (std::ostream & o, const TCS::DataArray& array) {
   array.print(o);
   return o;
}

