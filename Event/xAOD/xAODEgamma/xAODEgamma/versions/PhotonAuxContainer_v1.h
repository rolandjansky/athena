// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PhotonAuxContainer_v1.h 614853 2014-09-03 11:40:46Z krasznaa $
#ifndef XAODEGAMMA_VERSIONS_PHOTONAUXCONTAINER_V1_H
#define XAODEGAMMA_VERSIONS_PHOTONAUXCONTAINER_V1_H

// System include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/ElementLink.h"

// xAOD include(s):
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

// Local include(s):
#include "xAODEgamma/versions/EgammaAuxContainer_v1.h"

namespace xAOD {

  /// Auxiliary store for offline photons
  ///
  /// This class is used as the auxiliary store for offline reconstructed
  /// photons.
  ///
  /// @author Christos Anastopoulos
  /// @author Anthony Morley
  ///
  /// $Revision: 614853 $
  /// $Date: 2014-09-03 13:40:46 +0200 (Wed, 03 Sep 2014) $
  ///
  class PhotonAuxContainer_v1 : public EgammaAuxContainer_v1 {

  public :
    /// @name xAOD::PhotonAuxContainer constructors
    PhotonAuxContainer_v1();

  private:
    ///@name vector of links to vertexes
    /// @{

    std::vector< std::vector< ElementLink< VertexContainer > > > vertexLinks;

    /// @}

    /// @name Track Match details
    /// @{
    
    /// @brief difference between the cluster eta  and
    ///the eta of the first track of the vertex extrapolated to the second sampling.
    std::vector< float > convMatchDeltaEta1;
    
    /// @brief difference between the cluster eta  and
    ///the eta of the second track of the vertex extrapolated to the second sampling.
    std::vector< float > convMatchDeltaEta2;
    
    /// @brief difference between the cluster phi  and
    ///the phi of the first track of the vertex extrapolated to the second sampling.
    std::vector< float > convMatchDeltaPhi1 ;
    
    /// @brief difference between the cluster phi  and
    ///the phi of the second track of the vertex extrapolated to the second sampling.
    std::vector< float > convMatchDeltaPhi2;
    
    ///@}

  }; // class PhotonAuxContainer_v1

} // namespace xAOD

// Set up the StoreGate inheritance for the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::PhotonAuxContainer_v1, xAOD::EgammaAuxContainer_v1 );

#endif // XAODEGAMMA_VERSIONS_PHOTONAUXCONTAINER_V1_H
