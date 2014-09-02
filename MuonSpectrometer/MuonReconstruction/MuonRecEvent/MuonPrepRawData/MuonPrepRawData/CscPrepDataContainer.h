/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MUONPREPRAWDATA_CSCPREPDATACONTAINER_H
#define MUONPREPRAWDATA_CSCPREPDATACONTAINER_H

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/CscPrepDataCollection.h"
#include "AthLinks/DeclareIndexingPolicy.h"
#include "CLIDSvc/CLASS_DEF.h"

namespace Muon {

/** @brief Muon::MuonPrepDataContainer to hold the Muon::CscPrepDataCollection
*/

typedef MuonPrepDataContainer< CscPrepDataCollection > CscPrepDataContainer;

}

CLASS_DEF(Muon::CscPrepDataContainer,  1053454200, 1)
CONTAINER_IS_IDENTCONT(Muon::CscPrepDataContainer)

#endif
