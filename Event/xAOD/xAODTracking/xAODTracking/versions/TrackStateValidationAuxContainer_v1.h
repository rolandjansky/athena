/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_VERSIONS_MEASUREMENTSTATEONSURFACEAUXCONTAINER_V1_H
#define XAODTRACKING_VERSIONS_MEASUREMENTSTATEONSURFACEAUXCONTAINER_V1_H
 
// System include(s):
#include <vector>

// Core include(s):
#include "xAODCore/AuxContainerBase.h"
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODTracking/TrackMeasurementValidationContainer.h"

namespace xAOD {
 
   /// Temporary container used until we have I/O for AuxStoreInternal
   ///
   /// This class is meant to serve as a temporary way to provide an auxiliary
   /// store with Athena I/O capabilities for the Muon EDM. Will be exchanged for
   /// a generic auxiliary container type (AuxStoreInternal) later on.
   ///
   /// @author Niels Van Eldik <niels.van.eldik@cern.ch>
   /// @author Anthony Morley <anthony.morley@cern.ch>
   /// @author Edward Moyse <Edward.Moyse@cern.ch>
   /// @author Heberth Torres <htorres@cern.ch>
   ///
   class TrackStateValidationAuxContainer_v1 : public AuxContainerBase {
 
   public:
      /// Default constructor
      TrackStateValidationAuxContainer_v1();

   private:
      std::vector< int >       type;
      std::vector< uint64_t >  detElementId;
      std::vector< char >      detType;

      std::vector< float >     localX;
      std::vector< float >     localY;
      std::vector< float >     localTheta;
      std::vector< float >     localPhi;
      std::vector< float >     unbiasedResidualX;
      std::vector< float >     unbiasedResidualY;
      std::vector< float >     unbiasedPullX;
      std::vector< float >     unbiasedPullY;
      std::vector< float >     biasedResidualX;
      std::vector< float >     biasedResidualY;
      std::vector< float >     biasedPullX;
      std::vector< float >     biasedPullY;

      std::vector< ElementLink<xAOD::TrackMeasurementValidationContainer> >  trackMeasurementValidationLink;

   }; // class TrackStateValidationAuxContainer_v1
} // namespace xAOD
 
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TrackStateValidationAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRACKING_VERSIONS_MEASUREMENTSTATEONSURFACEAUXCONTAINER_V1_H
