/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODASSOCIATIONS_VERSIONS_TRACKPARTICLECLUSTERASSOCIATIONAUXCONTAINER_V1_H
#define XAODASSOCIATIONS_VERSIONS_TRACKPARTICLECLUSTERASSOCIATIONAUXCONTAINER_V1_H

// Core include(s):
#include "xAODCore/AuxContainerBase.h"
#include "AthLinks/ElementLink.h"

// xAOD include(s):
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

namespace xAOD {
 
   /// Temporary container used until we have I/O for AuxStoreInternal
   ///
   /// This class is meant to serve as a temporary way to provide an auxiliary
   /// store with Athena I/O capabilities for the Muon EDM. Will be exchanged for
   /// a generic auxiliary container type (AuxStoreInternal) later on.
   ///
   /// @author Niels Van Eldik <niels.van.eldik@cern.ch>
   /// @author Edward Moyse <Edward.Moyse@cern.ch>
   /// @author Heberth Torres <htorres@cern.ch>
   ///
   class TrackParticleClusterAssociationAuxContainer_v1 : public AuxContainerBase {
 
   public:
      /// Default constructor
      TrackParticleClusterAssociationAuxContainer_v1();
      
   private:
      std::vector<ElementLink< TrackParticleContainer > >              trackParticleLink;
      std::vector<std::vector<ElementLink< CaloClusterContainer > > >  caloClusterLinks;

   }; // class TrackParticleClusterAssociationAuxContainer_v1
} // namespace xAOD
 
// Set up a StoreGate inheritance for the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TrackParticleClusterAssociationAuxContainer_v1, xAOD::AuxContainerBase ); 

#endif // XAODASSOCIATIONS_VERSIONS_TRACKPARTICLECLUSTERASSOCIATIONAUXCONTAINER_V1_H
