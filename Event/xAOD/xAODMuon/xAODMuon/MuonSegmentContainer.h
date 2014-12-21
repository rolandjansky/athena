/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMUON_MUONSEGMENTCONTAINER_H
#define XAODMUON_MUONSEGMENTCONTAINER_H
 
// Local include(s):
#include "xAODMuon/MuonSegment.h"
#include "xAODMuon/versions/MuonSegmentContainer_v1.h"
 
namespace xAOD {
   /// Definition of the current "MuonSegment container version"
   typedef MuonSegmentContainer_v1 MuonSegmentContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::MuonSegmentContainer, 1129401482, 1 )


#endif // XAODMUON_MUONSEGMENTCONTAINER_H
