/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGACCELEVENT_TRIGINDETACCELCODES_H
#define TRIGACCELEVENT_TRIGINDETACCELCODES_H

namespace TrigAccel {
  
  constexpr unsigned int TrigInDetModuleID_CPU  = 0x10000000;

  constexpr unsigned int TrigInDetModuleID_CUDA = 0x10100000;
  
  enum InDetJobControlCode {
    PIXBS              = 11001, 
    SCTBS              = 12001, 
    PIX_CL_EXPORT      = 11010, 
    SCT_CL_EXPORT      = 12010, 
    SPS_EXPORT         = 13010,
    SFS_EXPORT         = 13011,
    TFS_EXPORT         = 13012,
    SEED_IMPORT        = 13100,
    SIL_LAYERS_EXPORT  = 10020,
    PIX_GEO_EXPORT     = 11020, 
    SCT_GEO_EXPORT     = 12020,
    ID_CABLING_EXPORT  = 10000,
    FIND_SEEDS         = 14000,
    MAKE_SEEDS         = 14001, 
    FIND_TRACKS        = 15000 
  };
}

#endif
