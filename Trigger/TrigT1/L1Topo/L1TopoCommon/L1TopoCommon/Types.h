//  Types.h
//  L1TopoCoreSimulation
//  Created by Joerg Stelzer on 11/22/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#ifndef __L1TopoCoreSimulation__Types__
#define __L1TopoCoreSimulation__Types__

#include <string>

namespace TCS {
   
   enum inputTOBType_t { NONE=0,
                         CLUSTER=0, 
                         JET=1,
                         MET=2,
                         MUON=3, 
                         TAU=4 };
   

   std::string inputTypeAsString(inputTOBType_t type);

   inputTOBType_t inputType(const std::string& input);

}

#endif
