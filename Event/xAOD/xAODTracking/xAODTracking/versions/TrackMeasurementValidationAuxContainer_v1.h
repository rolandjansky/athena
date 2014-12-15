/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_VERSIONS_TRACKMEASUREMENTVALIDATIONAUXCONTAINER_V1_H
#define XAODTRACKING_VERSIONS_TRACKMEASUREMENTVALIDATIONAUXCONTAINER_V1_H
 
// System include(s):
#include <vector>

// Core include(s):
#include "xAODCore/AuxContainerBase.h"
#include "AthLinks/ElementLink.h"

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
   class TrackMeasurementValidationAuxContainer_v1 : public AuxContainerBase {
 
   public:
      /// Default constructor
      TrackMeasurementValidationAuxContainer_v1();
 
   private:
      std::vector< uint64_t >      identifier;
      std::vector< std::vector< uint64_t > > rdoIdentifierList;

      std::vector< float    >      localX;
      std::vector< float    >      localY;
      std::vector< float    >      localXError;
      std::vector< float    >      localYError;
      std::vector< float    >      localXYCorrelation;

      std::vector< float    >      globalX;
      std::vector< float    >      globalY;
      std::vector< float    >      globalZ;

   }; // class TrackMeasurementValidationAuxContainer_v1
} // namespace xAOD
 
// Set up a CLID and StoreGate inheritance for the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TrackMeasurementValidationAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRACKING_VERSIONS_TRACKMEASUREMENTVALIDATIONAUXCONTAINER_V1_H
