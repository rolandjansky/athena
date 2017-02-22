// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PhotonTrigAuxContainer_v1.h 707252 2015-11-11 19:27:56Z rwhite $
#ifndef XAODTRIGEGAMMA_VERSIONS_PHOTONTRIGAUXCONTAINER_V1_H
#define XAODTRIGEGAMMA_VERSIONS_PHOTONTRIGAUXCONTAINER_V1_H

// System include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "xAODCore/AuxContainerBase.h"

// xAOD include(s):
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

namespace xAOD {

   /// Auxiliary store for offline photons
   ///
   /// This class is used as the auxiliary store for offline reconstructed
   /// photons.
   /// This is the trigger version to inherit from ByteStreamAuxContainer to persist all dynamic varibles
   /// @author Christos Anastopoulos
   /// @author Anthony Morley
   /// @author Ryan White
   ///
   class PhotonTrigAuxContainer_v1 : public AuxContainerBase {

   public:
      /// @name xAOD::PhotonAuxContainer constructors
      PhotonTrigAuxContainer_v1();

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

      /// @brief ET in a cone with half-opening angle 0.2, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3
      std::vector< float >  etcone20; 
      /// @brief ET in a cone with half-opening angle 0.3, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3
      std::vector< float >  etcone30; 
      /// @brief ET in a cone with half-opening angle 0.4, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3
      std::vector< float >  etcone40; 
      /// @brief summed pt of tracks in a cone with half-opening angle 0.2 (no zvx cut photons, 1mm electrons)
      std::vector< float > ptcone20;
      /// @brief summed pt of tracks in a cone with half-opening angle 0.3
      std::vector< float > ptcone30;
      /// @brief summed pt of tracks in a cone with half-opening angle 0.4
      std::vector< float > ptcone40;
      /// @brief summed pt of tracks in a cone with half-opening angle 0.2 (no zvx cut photons, 1mm electrons)
      std::vector< float > ptvarcone20;
      /// @brief summed pt of tracks in a cone with half-opening angle 0.3
      std::vector< float > ptvarcone30;
      /// @brief summed pt of tracks in a cone with half-opening angle 0.4
      std::vector< float > ptvarcone40;
      //et cone for topo cluster
      /// @brief ET in a cone with half-opening angle 0.20
      std::vector< float >  topoetcone20;
      /// @brief ET in a cone with half-opening angle 0.30
      std::vector< float >  topoetcone30;
      /// @brief ET in a cone with half-opening angle 0.40
      std::vector< float >  topoetcone40;
      /// @brief Pid decision
      std::vector< char > Loose;
      /// @brief Pid decision
      std::vector< char > Medium;
      /// @brief Pid decision
      std::vector< char > Tight;
      /// @brief Pid decision isEM word
      std::vector< unsigned int > isEMLoose;
      /// @brief Pid decision isEM word
      std::vector< unsigned int > isEMMedium;
      /// @brief Pid decision isEM word
      std::vector< unsigned int > isEMTight;

      ///@}

   }; // class PhotonTrigAuxContainer_v1

} // namespace xAOD

// Set up the StoreGate inheritance for the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::PhotonTrigAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRIGEGAMMA_VERSIONS_PHOTONAUXCONTAINER_V3_H
