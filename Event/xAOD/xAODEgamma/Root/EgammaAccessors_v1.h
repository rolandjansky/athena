// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EgammaAccessors_v1.h 741466 2016-04-19 20:21:49Z christos $
#ifndef XAOD_EGAMMAACCESSORS_V1_H
#define XAOD_EGAMMAACCESSORS_V1_H

// EDM include(s):
#include "AthContainers/AuxElement.h"
// Local include(s):
#include "xAODEgamma/EgammaEnums.h"

namespace xAOD {

   /// Helper function for managing Egamma Accessor objects
   ///
   /// This function holds on to Accessor objects that can be used by each
   /// Egamma_v1 object at runtime to get/set detail values on themselves.


   const SG::AuxElement::Accessor< float >*
   showerShapeAccessorV1( xAOD::EgammaParameters::ShowerShapeType type );

} // namespace xAOD

#endif // XAOD_EGAMMAACCESSORS_V1_H
