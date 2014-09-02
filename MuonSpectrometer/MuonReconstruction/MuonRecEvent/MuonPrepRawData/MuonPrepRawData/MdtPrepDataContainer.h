/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MUONPREPRAWDATA_MDTPREPDATACONTAINER_H
#define MUONPREPRAWDATA_MDTPREPDATACONTAINER_H

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "AthLinks/DeclareIndexingPolicy.h"
#include "CLIDSvc/CLASS_DEF.h"

namespace Muon {

/** @brief MdtPrepDataContainer Muon::MuonPrepDataContainer to hold the Muon::MdtPrepDataCollection*/    
typedef MuonPrepDataContainer< MdtPrepDataCollection > MdtPrepDataContainer;

}

CLASS_DEF(Muon::MdtPrepDataContainer,  1153454201, 1)
CONTAINER_IS_IDENTCONT(Muon::MdtPrepDataContainer)

#endif
