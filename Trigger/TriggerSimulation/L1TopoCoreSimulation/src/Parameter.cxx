/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  Parameter.cxx
//  TopoCore
//
//  Created by Joerg Stelzer on 11/18/12.
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
