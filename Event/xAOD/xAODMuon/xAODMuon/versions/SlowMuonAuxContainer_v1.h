/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMUON_VERSIONS_SLOWMUONAUXCONTAINER_V1_H
#define XAODMUON_VERSIONS_SLOWMUONAUXCONTAINER_V1_H

// Core include(s):
#include "xAODCore/AuxContainerBase.h"
#include "AthLinks/ElementLink.h"

// xAOD include(s):
#include "xAODMuon/MuonContainer.h"
 
namespace xAOD {
 
   /// Temporary container used until we have I/O for AuxStoreInternal
   ///
   /// This class is meant to serve as a temporary way to provide an auxiliary
   /// store with Athena I/O capabilities for the Muon EDM. Will be exchanged for
   /// a generic auxiliary container type (AuxStoreInternal) later on.
   ///
   /// @author Susan Cheatham <Susan.Cheatham@cern.ch>
   /// @author Niels Van Eldik <niels.van.eldik@cern.ch>
   /// @author Edward Moyse <Edward.Moyse@cern.ch>
   /// @author Shlomit Tarem <shlomit.tarem@cern.ch>
   /// @author Heberth Torres <htorres@cern.ch>
   ///
   class SlowMuonAuxContainer_v1 : public AuxContainerBase {
 
   public:
      /// Default constructor
      SlowMuonAuxContainer_v1();
 
   private:
      std::vector< float >    beta;
      std::vector< float >    betaT;
      std::vector< float >    ann;
      
      std::vector< int >      nRpcHits;
      std::vector< int >      nTileCells;

      std::vector< float >    rpcBetaAvg;
      std::vector< float >    rpcBetaRms;
      std::vector< float >    rpcBetaChi2;
      std::vector< int >      rpcBetaDof;

      std::vector< float >    mdtBetaAvg;
      std::vector< float >    mdtBetaRms;
      std::vector< float >    mdtBetaChi2;
      std::vector< int >      mdtBetaDof;

      std::vector< float >    caloBetaAvg;
      std::vector< float >    caloBetaRms;
      std::vector< float >    caloBetaChi2;
      std::vector< int >      caloBetaDof;

      std::vector< float >    dEdxPixel;
      std::vector< float >    dEdxCalo;
      std::vector< int >      dEdxNClusters;

      std::vector<ElementLink< MuonContainer > >   muonLink;

   }; // class SlowMuonAuxContainer_v1
} // namespace xAOD
 
// Set up a StoreGate inheritance for the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::SlowMuonAuxContainer_v1, xAOD::AuxContainerBase ); 

#endif // XAODMUON_VERSIONS_SLOWMUONAUXCONTAINER_V1_H
