/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMUON_SLOWMUONCONTAINER_H
#define XAODMUON_SLOWMUONCONTAINER_H
 
// Local include(s):
#include "xAODMuon/SlowMuon.h"
#include "xAODMuon/versions/SlowMuonContainer_v1.h"
 
namespace xAOD {
   /// Definition of the current "SlowMuon container version"
   typedef SlowMuonContainer_v1 SlowMuonContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::SlowMuonContainer, 1143850792, 1 )

#endif // XAODMUON_SLOWMUONCONTAINER_H
