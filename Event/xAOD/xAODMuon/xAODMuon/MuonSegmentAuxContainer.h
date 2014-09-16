/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMUON_MUONSEGMENTAUXCONTAINER_H
#define XAODMUON_MUONSEGMENTAUXCONTAINER_H
 
// Local include(s):
#include "xAODMuon/versions/MuonSegmentAuxContainer_v1.h"
 
namespace xAOD {
   /// Definition of the current Muon auxiliary container
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::MuonSegmentContainer, so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef MuonSegmentAuxContainer_v1 MuonSegmentAuxContainer;
}
 
#endif // XAODMUON_MUONSEGMENTAUXCONTAINER_H
