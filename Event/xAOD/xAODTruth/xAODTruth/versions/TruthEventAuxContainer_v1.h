// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthEventAuxContainer_v1.h 622193 2014-10-16 16:08:34Z krasznaa $
#ifndef XAODTRUTH_VERSIONS_TRUTHEVENTAUXCONTAINER_V1_H
#define XAODTRUTH_VERSIONS_TRUTHEVENTAUXCONTAINER_V1_H

// System include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "xAODCore/AuxContainerBase.h"

// Local include(s):
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertexContainer.h"

namespace xAOD {

   /// Auxiliary store for the truth vertices
   ///
   /// @author Jovan Mitrevski <Jovan.Mitrevski@cern.ch>
   ///
   /// $Revision: 622193 $
   /// $Date: 2014-10-16 18:08:34 +0200 (Thu, 16 Oct 2014) $
   ///
   class TruthEventAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      TruthEventAuxContainer_v1();

   private:
      /// @name Links to the interaction particles/vertices
      /// @{
      std::vector< ElementLink< TruthVertexContainer > >
      signalProcessVertexLink;
      std::vector< ElementLink< TruthParticleContainer > > beamParticle1Link;
      std::vector< ElementLink< TruthParticleContainer > > beamParticle2Link;
      /// @}

      /// @todo Needs to be a map or similar (perhaps two linked vectors?)
      std::vector< std::vector< float > > weights;

      /// @name Cross sections and errors coming from the generator
      /// @{
      std::vector< float > crossSection;
      std::vector< float > crossSectionError;
      /// @}

      /// @name Links to the generated/simulated particles/vertices
      /// @{
      std::vector< std::vector< ElementLink< TruthParticleContainer > > >
      truthParticleLinks;
      std::vector< std::vector< ElementLink< TruthVertexContainer > > >
      truthVertexLinks;
      /// @}

   }; // class TruthEventAuxContainer_v1

} // namespace xAOD

// Declare the class's inheritance to StoreGate:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TruthEventAuxContainer_v1, xAOD::AuxContainerBase ); 

#endif // XAODTRUTH_VERSIONS_TRUTHEVENTAUXCONTAINER_V1_H
