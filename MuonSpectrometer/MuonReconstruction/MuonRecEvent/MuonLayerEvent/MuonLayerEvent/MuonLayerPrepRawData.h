/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONLAYERPREPRAWDATA_H
#define MUON_MUONLAYERPREPRAWDATA_H

#include <vector>

#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "MuonPrepRawData/CscPrepDataCollection.h"
#include "MuonPrepRawData/sTgcPrepDataCollection.h"
#include "MuonPrepRawData/MMPrepDataCollection.h"


namespace Muon {
  
  /**
      Struct to hold all PrepRawData collections in a given layer 
   */
  struct MuonLayerPrepRawData {
    std::vector< const MdtPrepDataCollection* >  mdts;
    std::vector< const RpcPrepDataCollection* >  rpcs;
    std::vector< const TgcPrepDataCollection* >  tgcs;
    std::vector< const CscPrepDataCollection* >  cscs;
    std::vector< const sTgcPrepDataCollection* > stgcs;
    std::vector< const MMPrepDataCollection* >   mms;
  };

}

#endif
