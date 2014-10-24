// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PhotonAuxContainer_v3.h 619839 2014-10-03 13:38:44Z christos $
#ifndef XAODEGAMMA_VERSIONS_PHOTONAUXCONTAINER_V3_H
#define XAODEGAMMA_VERSIONS_PHOTONAUXCONTAINER_V3_H

// System include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/ElementLink.h"

// xAOD include(s):
#include "xAODCore/AuxContainerBase.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

namespace xAOD {

   /// Auxiliary store for offline photons
   ///
   /// This class is used as the auxiliary store for offline reconstructed
   /// photons.
   ///
   /// @author Christos Anastopoulos
   /// @author Anthony Morley
   ///
   /// $Revision: 619839 $
   /// $Date: 2014-10-03 15:38:44 +0200 (Fri, 03 Oct 2014) $
   ///
   class PhotonAuxContainer_v3 : public AuxContainerBase {

   public:
      /// @name xAOD::PhotonAuxContainer constructors
      PhotonAuxContainer_v3();

   private:
      /// Include the egamma variables:
#define EGAMMA_AUX_VARIABLE( TYPE, NAME ) TYPE NAME
#include "xAODEgamma/versions/EgammaAuxVariables_v3.def"
#undef EGAMMA_AUX_VARIABLE

      ///@name vector of links to vertexes
      /// @{

      std::vector< std::vector< ElementLink< VertexContainer > > > vertexLinks;

      /// @}

      /// @name Track Match details
      /// @{

      /// Difference between the cluster eta and the eta of the first track of
      /// the vertex extrapolated to the second sampling.
      std::vector< float > convMatchDeltaEta1;

      /// Difference between the cluster eta and the eta of the second track of
      /// the vertex extrapolated to the second sampling.
      std::vector< float > convMatchDeltaEta2;

      /// Difference between the cluster phi and the phi of the first track of
      /// the vertex extrapolated to the second sampling.
      std::vector< float > convMatchDeltaPhi1;

      /// Difference between the cluster phi and the phi of the second track of
      /// the vertex extrapolated to the second sampling.
      std::vector< float > convMatchDeltaPhi2;

      ///@}

   }; // class PhotonAuxContainer_v3

} // namespace xAOD

// Set up the StoreGate inheritance for the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::PhotonAuxContainer_v3, xAOD::AuxContainerBase );

#endif // XAODEGAMMA_VERSIONS_PHOTONAUXCONTAINER_V3_H
