// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronAccessors_v1.h 575464 2013-12-13 16:28:44Z christos $
#ifndef XAOD_ELECTRONACCESSORS_V1_H
#define XAOD_ELECTRONACCESSORS_V1_H

// EDM include(s):
#include "AthContainers/AuxElement.h"
// Local include(s):
#include "xAODEgamma/EgammaEnums.h"

namespace xAOD {

   /// Helper function for managing Egamma Accessor objects
   ///
   /// This function holds on to Accessor objects that can be used by each
   /// Electron_v1 object at runtime to get/set detail values on themselves.

   const SG::AuxElement::Accessor< float >*
   trackCaloMatchAccessorV1( xAOD::EgammaParameters::TrackCaloMatchType type );


} // namespace xAOD

#endif // XAOD_ELECTRONACCESSORS_V1_H
