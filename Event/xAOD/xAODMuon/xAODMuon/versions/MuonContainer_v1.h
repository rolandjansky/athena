/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMUON_VERSIONS_MUONCONTAINER_V1_H
#define XAODMUON_VERSIONS_MUONCONTAINER_V1_H
 
#include <stdint.h>
 
// Core include(s):
#include "AthContainers/DataVector.h"
 
// Local include(s):
#include "xAODMuon/versions/Muon_v1.h"
 
namespace xAOD {
   /// The container is a simple typedef for now
   typedef DataVector< xAOD::Muon_v1 > MuonContainer_v1;
}
  
#endif // XAODMUON_VERSIONS_MUONCONTAINER_V1_H
