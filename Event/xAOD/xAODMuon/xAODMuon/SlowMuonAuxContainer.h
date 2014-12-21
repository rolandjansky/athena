/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMUON_SLOWMUONAUXCONTAINER_H
#define XAODMUON_SLOWMUONAUXCONTAINER_H
 
// Local include(s):
#include "xAODMuon/versions/SlowMuonAuxContainer_v1.h"
 
namespace xAOD {
   /// Definition of the current Muon auxiliary container
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::SlowMuonContainer, so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef SlowMuonAuxContainer_v1 SlowMuonAuxContainer;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::SlowMuonAuxContainer, 1161619655, 1 )

#endif // XAODMUON_SLOWMUONAUXCONTAINER_H
