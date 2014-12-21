/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMUON_MUONCONTAINER_H
#define XAODMUON_MUONCONTAINER_H
 
// Local include(s):
#include "xAODMuon/Muon.h"
#include "xAODMuon/versions/MuonContainer_v1.h"
 
namespace xAOD {
   /// Definition of the current "Muon container version"
   typedef MuonContainer_v1 MuonContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::MuonContainer, 1178459224, 1 )


#endif // XAODMUON_MUONCONTAINER_H
