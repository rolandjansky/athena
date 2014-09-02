/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MUONPREPRAWDATA_RPCPREPDATACONTAINER_H
#define MUONPREPRAWDATA_RPCPREPDATACONTAINER_H

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "AthLinks/DeclareIndexingPolicy.h"
#include "CLIDSvc/CLASS_DEF.h"

namespace Muon {
  
/** @brief Muon::MuonPrepDataContainer to hold the Muon::RpcPrepDataCollection*/  
typedef MuonPrepDataContainer< RpcPrepDataCollection > RpcPrepDataContainer;

}

CLASS_DEF(Muon::RpcPrepDataContainer,  1253454202, 1)
CONTAINER_IS_IDENTCONT(Muon::RpcPrepDataContainer)

#endif
