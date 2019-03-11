/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMUON_MUONAUXCONTAINER_H
#define XAODMUON_MUONAUXCONTAINER_H
 
// Local include(s):
#include "xAODMuon/versions/MuonAuxContainer_v5.h"
 
namespace xAOD {
   /// Definition of the current Muon auxiliary container
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::MuonContainer, so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef MuonAuxContainer_v5 MuonAuxContainer;
}

// Set up a CLID and StoreGate inheritance for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::MuonAuxContainer, 1153381047, 1 )


#endif // XAODMUON_MUONAUXCONTAINER_H
