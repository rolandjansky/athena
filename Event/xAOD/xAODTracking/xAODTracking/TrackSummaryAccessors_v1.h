// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrackSummaryAccessors_v1.h 574227 2013-12-06 14:20:39Z emoyse $
#ifndef XAOD_TRACKSUMMARYACCESSORS_V1_H
#define XAOD_TRACKSUMMARYACCESSORS_V1_H

// EDM include(s):
#include "AthContainers/AuxElement.h"

// Local include(s):
#include "xAODTracking/TrackingPrimitives.h"

namespace xAOD {

   /// Helper function for managing TrackSummary Accessor objects
   ///
   /// This function holds on to Accessor objects that can be used by each
   /// TrackParticle_v1 object at runtime to get/set summary values on themselves.
   ///
   template <class T>
   SG::AuxElement::Accessor< T >*
   trackSummaryAccessorV1( xAOD::SummaryType type );

} // namespace xAOD

#endif // XAODCALOEVENT_CALOCLUSTERACCESSORS_V1_H
