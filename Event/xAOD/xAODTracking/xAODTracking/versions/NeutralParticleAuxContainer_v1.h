// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODTRACKING_VERSIONS_NEUTRALPARTICLEAUXCONTAINER_V1_H
#define XAODTRACKING_VERSIONS_NEUTRALPARTICLEAUXCONTAINER_V1_H

// System include(s):
#include <stdint.h>
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

   /// Temporary container used until we have I/O for AuxStoreInternal
   ///
   /// This class is meant to serve as a temporary way to provide an auxiliary
   /// store with Athena I/O capabilities for the NeutralParticle EDM. Will be exchanged for
   /// a generic auxiliary container type (AuxStoreInternal) later on.
   ///
   /// @author Andreas Salzburger
   ///
   class NeutralParticleAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      NeutralParticleAuxContainer_v1();

   private:
      /// Dumps contents (for debugging)
      void dump() const;

      /// @name Defining parameters (perigee)
      /// @{
      std::vector< float >                d0;
      std::vector< float >                z0;
      std::vector< float >                phi;
      std::vector< float >                theta;
      std::vector< float >                oneOverP;

      std::vector< std::vector<float> >   definingParametersCovMatrix;

      std::vector< float >                vx;
      std::vector< float >                vy;
      std::vector< float >                vz;

   }; // class NeutralParticleAuxContainer_v1

} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::NeutralParticleAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODJET_VERSIONS_JETAUXCONTAINER_V1_H
