// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTruthHelpers.h 668406 2015-05-19 15:32:15Z krasznaa $
#ifndef XAODTRUTH_XAODTRUTHHELPERS_H
#define XAODTRUTH_XAODTRUTHHELPERS_H

// Local include(s):
#include "xAODTruth/TruthParticleFwd.h"

namespace xAOD {

   // Forward declaration(s):
   class IParticle;

   /// Dedicated namespace for the helper functions
   namespace TruthHelpers {

      /// Return the truthParticle associated to the given IParticle (if any)
      const TruthParticle* getTruthParticle( const xAOD::IParticle& p );

      /// Return the particle's truth type (as defined by the MC Truth
      /// Classifier)
      int getParticleTruthType( const xAOD::IParticle& p );

      /// Return the particle's truth origin (as defined by the MC Truth
      /// Classifier)
      int getParticleTruthOrigin( const xAOD::IParticle& p );

   } // namespace TruthHelpers

} // namespace xAOD

#endif // XAODTRUTH_XAODTRUTHHELPERS_H
