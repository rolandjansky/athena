//
//  Parameter.cxx
//  TopoCore
//
//  Created by Joerg Stelzer on 11/18/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.
//

#include "L1TopoCoreSimulation/Parameter.h"

std::ostream &
operator<<(std::ostream &o, const TCS::Parameter & p) {
   o << p.name() << ": " << p.value();

   if(!p.isDefault()) {
      o << " (default: " << p.defaultValue() << ")";
   }

   return o;
}
