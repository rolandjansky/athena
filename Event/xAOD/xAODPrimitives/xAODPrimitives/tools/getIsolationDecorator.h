// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODPRIMITIVES_TOOLS_GETISOLATIONDECORATOR_H
#define XAODPRIMITIVES_TOOLS_GETISOLATIONDECORATOR_H

// EDM include(s):
#include "AthContainers/AuxElement.h"

// Local include(s):
#include "xAODPrimitives/IsolationType.h"

namespace xAOD {

  /// Get the Decorator object for a given isolation type
  ///
  /// This function should be used in the EDM code to retrieve the Decorator
  /// object meant to handle one of the isolation values.
  ///
  /// @param type The Iso::IsolationType type
  /// @returns A pointer to the Decorator object if successful, or a
  ///          null-pointer if not
  const SG::AuxElement::Decorator< float >*
  getIsolationDecorator( Iso::IsolationType type );

} // namespace xAOD

#endif // XAODPRIMITIVES_TOOLS_GETISOLATIONDECORATOR_H
