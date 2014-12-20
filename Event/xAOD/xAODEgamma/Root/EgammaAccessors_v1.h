// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EgammaAccessors_v1.h 630766 2014-11-25 00:34:26Z christos $
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


   SG::AuxElement::Accessor< float >*
   showerShapeAccessorV1( xAOD::EgammaParameters::ShowerShapeType type );

   SG::AuxElement::Accessor< char >*
   selectionMenuAccessorV1( xAOD::EgammaParameters::SelectionMenu menu ) ;

   SG::AuxElement::Accessor< unsigned int >*
   selectionisEMAccessorV1( xAOD::EgammaParameters::SelectionisEM isEM ) ;


} // namespace xAOD

#endif // XAOD_EGAMMAACCESSORS_V1_H
