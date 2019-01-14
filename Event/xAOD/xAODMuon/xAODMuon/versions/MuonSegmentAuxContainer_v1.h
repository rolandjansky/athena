/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMUON_VERSIONS_MUONSEGMENTAUXCONTAINER_V1_H
#define XAODMUON_VERSIONS_MUONSEGMENTAUXCONTAINER_V1_H
 
// System include(s):

// Core include(s):
#include "xAODCore/AuxContainerBase.h"
#include "AthLinks/ElementLink.h"
 
// xAOD include(s):
#ifndef XAOD_ANALYSIS
#ifndef GENERATIONBASE
#include "TrkSegment/SegmentCollection.h"
#endif
#endif

namespace xAOD {
 
   /// Temporary container used until we have I/O for AuxStoreInternal
   ///
   /// This class is meant to serve as a temporary way to provide an auxiliary
   /// store with Athena I/O capabilities for the Muon EDM. Will be exchanged for
   /// a generic auxiliary container type (AuxStoreInternal) later on.
   ///
   /// @author Niels Van Eldik <niels.van.eldik@cern.ch>
   /// @author Heberth Torres <htorres@cern.ch>
   /// @author Edward Moyse <Edward.Moyse@cern.ch>
   ///
   class MuonSegmentAuxContainer_v1 : public AuxContainerBase {
 
   public:
      /// Default constructor
      MuonSegmentAuxContainer_v1();
 
   private:
      std::vector< float >      x;
      std::vector< float >      y;
      std::vector< float >      z;

      std::vector< float >      px;
      std::vector< float >      py;
      std::vector< float >      pz;

      std::vector< float >      t0;
      std::vector< float >      t0error;

      std::vector< float >      chiSquared;
      std::vector< float >      numberDoF;

      std::vector< int   >      sector;
      std::vector< int   >      chamberIndex;
      std::vector< int   >      etaIndex;
      std::vector< int   >      technology;

      std::vector< int   >      nPrecisionHits;
      std::vector< int   >      nPhiLayers;
      std::vector< int   >      nTrigEtaLayers;

   }; // class MuonSegmentAuxContainer_v1
} // namespace xAOD
 
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::MuonSegmentAuxContainer_v1, xAOD::AuxContainerBase ); 

#endif // XAODMUON_VERSIONS_MUONSEGMENTAUXCONTAINER_V1_H
