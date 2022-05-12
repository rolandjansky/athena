// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoCommon_Types
#define L1TopoCommon_Types

#include <string>

namespace TCS {

   enum inputTOBType_t { NONE=0,
                         CLUSTER=0, 
                         JET=1,
                         JTAU=2,
                         JLJET=3,
                         JJET=4,
                         MET=5,
                         MUON=6, 
                         TAU=7,
                         LATEMUON=8, 
                         MUONNEXTBC=9,
                         EEM=10,
                         ETAU=11,
                         JEM=12,
                         GLJET=13,
                         GJET=14,
                         CTAU=15,
                         JXE=16,
                         JXEC=17,
                         JXEPERF=18,
                         JTE=19,
                         JTEC=20,
                         JTEFWD=21,
                         JTEFWDA=22,
                         JTEFWDC=23,
                         GXEJWOJ=24,
                         GXENC=25,
                         GXERHO=26,
                         GMHT=27,
                         GTE=28
   }; 

  enum outputTopoType_t { UNDEF=999,
                          LEGACYTOPO0=0,
                          LEGACYTOPO1=1,
                          TOPO2EL=2,
                          TOPO3EL=3,
                          TOPO1OPT0=4,
                          TOPO1OPT1=5,
                          TOPO1OPT2=6,
                          TOPO1OPT3=7
   }; 

   std::string inputTypeAsString(inputTOBType_t type);

   inputTOBType_t inputType(const std::string& input);
  
   outputTopoType_t outputType(const std::string& output);

}

#endif
