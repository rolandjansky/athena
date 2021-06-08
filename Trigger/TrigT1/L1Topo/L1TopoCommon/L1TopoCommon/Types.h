// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoCommon_Types
#define L1TopoCommon_Types

#include <string>

namespace TCS {

   enum inputTOBType_t { NONE=0,
                         CLUSTER=0, 
                         JET=1,
                         JTAU=2,
                         JLARGERJET=3,
                         JJET=4,
                         MET=5,
                         MUON=6, 
                         TAU=7,
			 LATEMUON=8, 
			 MUONNEXTBC=9,
                         EEM=10 }; 

   std::string inputTypeAsString(inputTOBType_t type);

   inputTOBType_t inputType(const std::string& input);

}

#endif
