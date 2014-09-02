/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MUONPREPRAWDATA_STGCPREPDATACONTAINER_H
#define MUONPREPRAWDATA_STGCPREPDATACONTAINER_H

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/sTgcPrepDataCollection.h"
#include "AthLinks/DeclareIndexingPolicy.h"
#include "CLIDSvc/CLASS_DEF.h"

namespace Muon {
  
/** @brief Muon::MuonPrepDataContainer to hold the Muon::sTgcPrepDataCollection*/  
typedef MuonPrepDataContainer< sTgcPrepDataCollection > sTgcPrepDataContainer;

}

CLASS_DEF(Muon::sTgcPrepDataContainer,  1265704919, 1)
CONTAINER_IS_IDENTCONT(Muon::sTgcPrepDataContainer)

#endif
