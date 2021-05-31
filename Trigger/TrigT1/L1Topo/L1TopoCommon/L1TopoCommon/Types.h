// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoCommon_Types
#define L1TopoCommon_Types

#include <string>

namespace TCS {

   enum inputTOBType_t { NONE=0,
                         CLUSTER=0, 
                         JET=1,
                         JJET=2,
                         MET=3,
                         MUON=4, 
                         TAU=5,
			 LATEMUON=6, 
			 MUONNEXTBC=7,
                         EEM=8 }; 

   std::string inputTypeAsString(inputTOBType_t type);

   inputTOBType_t inputType(const std::string& input);

}

#endif
