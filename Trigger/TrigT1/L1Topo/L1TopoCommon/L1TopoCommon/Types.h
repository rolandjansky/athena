// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoCommon_Types
#define L1TopoCommon_Types

#include <string>

namespace TCS {
   
   enum inputTOBType_t { NONE=0,
                         CLUSTER=0, 
                         JET=1,
                         MET=2,
                         MUON=3, 
                         TAU=4,
			 LATEMUON=5, 
			 MUONNEXTBC=6  };
   

   std::string inputTypeAsString(inputTOBType_t type);

   inputTOBType_t inputType(const std::string& input);

}

#endif
