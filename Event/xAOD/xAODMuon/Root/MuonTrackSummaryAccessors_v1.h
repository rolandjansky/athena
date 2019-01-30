// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonTrackSummaryAccessors_v1.h 587620 2014-03-14 12:41:33Z krasznaa $
#ifndef XAOD_MUONTRACKSUMMARYACCESSORS_V1_H
#define XAOD_MUONTRACKSUMMARYACCESSORS_V1_H

// EDM include(s):
#include "AthContainers/AuxElement.h"

// Local include(s):
#include "xAODTracking/TrackingPrimitives.h"

namespace xAOD {

   /// Helper function for managing MuonTrackSummary Accessor objects
   ///
   /// This function holds on to Accessor objects that can be used by each
   /// TrackParticle_v1 object at runtime to get/set summary values on themselves.
   ///
   const SG::AuxElement::Accessor< uint8_t >*
   muonTrackSummaryAccessorV1( xAOD::MuonSummaryType type );

} // namespace xAOD

#endif // XAOD_MUONTRACKSUMMARYACCESSORS_V1_H
