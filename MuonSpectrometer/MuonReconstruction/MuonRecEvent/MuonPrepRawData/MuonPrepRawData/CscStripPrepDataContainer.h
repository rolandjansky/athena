/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MUONPREPRAWDATA_CSCSTRIPPREPDATACONTAINER_H
#define MUONPREPRAWDATA_CSCSTRIPPREPDATACONTAINER_H

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/CscStripPrepDataCollection.h"
#include "CLIDSvc/CLASS_DEF.h"
 
namespace Muon {

/** @brief Muon::MuonPrepDataContainer to hold the Muon::CscStripPrepDataCollection*/    
typedef MuonPrepDataContainer< CscStripPrepDataCollection > CscStripPrepDataContainer;

}

CLASS_DEF(Muon::CscStripPrepDataContainer,  1240951609, 1)

#endif
