// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODPRIMITIVES_TOOLS_GETISOLATIONACCESSOR_H
#define XAODPRIMITIVES_TOOLS_GETISOLATIONACCESSOR_H

// EDM include(s):
#include "AthContainers/AuxElement.h"

// Local include(s):
#include "xAODPrimitives/IsolationType.h"

namespace xAOD {

  /// Get the Accessor object for a given isolation type
  ///
  /// This function should be used in the EDM code to retrieve the Accessor
  /// object meant to handle one of the isolation values. 
  ///
  /// @param type The Iso::IsolationType type
  /// @returns A pointer to the Accessor object if successful, or a
  ///          null-pointer if not
  const SG::AuxElement::Accessor< float >*
  getIsolationAccessor( Iso::IsolationType type );

} // namespace xAOD

#endif // XAODPRIMITIVES_TOOLS_GETISOLATIONACCESSOR_H
