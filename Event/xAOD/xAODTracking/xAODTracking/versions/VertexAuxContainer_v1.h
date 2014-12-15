// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: VertexAuxContainer_v1.h 575751 2013-12-16 16:45:36Z krasznaa $
#ifndef XAODTRACKING_VERSIONS_VERTEXAUXCONTAINER_V1_H
#define XAODTRACKING_VERSIONS_VERTEXAUXCONTAINER_V1_H

// System include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "xAODCore/AuxContainerBase.h"

// xAOD include(s):
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/NeutralParticleContainer.h"

namespace xAOD {

   /// Temporary container used until we have I/O for AuxStoreInternal
   ///
   /// This class is meant to serve as a temporary way to provide an auxiliary
   /// store with Athena I/O capabilities for the TrackParticle EDM. Will be exchanged for
   /// a generic auxiliary container type (AuxStoreInternal) later on.
   ///
   /// @author Edward Moyse
   ///
   class VertexAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      VertexAuxContainer_v1();

   private:
      /// We store Chi squared and the number of degrees of freedom of the vertex fit as float 
      std::vector< float >  chiSquared; 
      std::vector< float >  numberDoF; 
      /// We store the 3-pos
      std::vector< float >  x; 
      std::vector< float >  y; 
      std::vector< float >  z; 
      /// Vertex covariance matrix
      std::vector< std::vector< float > > covariance;
      /// Vertex type
      std::vector< short > vertexType;

      typedef std::vector< ElementLink< TrackParticleContainer > >
         TrackLink_t;
      std::vector< TrackLink_t > trackParticleLinks;
      std::vector< std::vector< float > > trackWeights;

      typedef std::vector< ElementLink< NeutralParticleContainer > >
         NeutralLink_t;
      std::vector< NeutralLink_t > neutralParticleLinks;
      std::vector< std::vector< float > > neutralWeights;

   }; // class VertexAuxContainer_v1

} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::VertexAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRACKING_VERSIONS_VERTEXAUXCONTAINER_V1_H
