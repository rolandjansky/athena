/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MUONPREPRAWDATA_MMPREPDATACONTAINER_H
#define MUONPREPRAWDATA_MMPREPDATACONTAINER_H

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/MMPrepDataCollection.h"
#include "AthLinks/DeclareIndexingPolicy.h"
#include "CLIDSvc/CLASS_DEF.h"

namespace Muon {
  
/** @brief Muon::MuonPrepDataContainer to hold the Muon::MMPrepDataCollection*/  
typedef MuonPrepDataContainer< MMPrepDataCollection > MMPrepDataContainer;

}

CLASS_DEF(Muon::MMPrepDataContainer,  1115503214, 1)
CONTAINER_IS_IDENTCONT(Muon::MMPrepDataContainer)

#endif
