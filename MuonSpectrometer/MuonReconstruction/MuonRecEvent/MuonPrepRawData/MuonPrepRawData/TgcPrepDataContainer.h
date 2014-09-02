/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MUONPREPRAWDATA_TGCPREPDATACONTAINER_H
#define MUONPREPRAWDATA_TGCPREPDATACONTAINER_H

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "AthLinks/DeclareIndexingPolicy.h"
#include "CLIDSvc/CLASS_DEF.h"

namespace Muon {

/** @brief Muon::MuonPrepDataContainer to hold the Muon::TgcPrepDataCollection*/    
typedef MuonPrepDataContainer< TgcPrepDataCollection > TgcPrepDataContainer;

}

CLASS_DEF(Muon::TgcPrepDataContainer,  1353454203, 1)
CONTAINER_IS_IDENTCONT(Muon::TgcPrepDataContainer)

#endif
