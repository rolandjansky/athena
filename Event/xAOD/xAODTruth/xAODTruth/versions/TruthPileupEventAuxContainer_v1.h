// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRUTH_VERSIONS_TRUTHPILEUPEVENTAUXCONTAINER_V1_H
#define XAODTRUTH_VERSIONS_TRUTHPILEUPEVENTAUXCONTAINER_V1_H

#include <vector>

#include "AthLinks/ElementLink.h"
#include "xAODCore/AuxContainerBase.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertexContainer.h"

namespace xAOD {


   /// Auxiliary store for the truth pile-up events
   ///
   /// @author Jovan Mitrevski <Jovan.Mitrevski@cern.ch>
   /// @author Andy Buckley <Andy.Buckley@cern.ch>
   ///
   class TruthPileupEventAuxContainer_v1 : public AuxContainerBase {
   public:

      /// Default constructor
      TruthPileupEventAuxContainer_v1();

   private:

     std::vector< std::vector< ElementLink< TruthParticleContainer > > > truthParticleLinks;
     std::vector< std::vector< ElementLink< TruthVertexContainer > > > truthVertexLinks;

   }; // class TruthPileupEventAuxContainer_v1


} // namespace xAOD


// StoreGate registration
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TruthPileupEventAuxContainer_v1, xAOD::AuxContainerBase );


#endif // XAODTRUTH_VERSIONS_TRUTHPILEUPEVENTAUXCONTAINER_V1_H
