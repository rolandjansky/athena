// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: getIsolationAccessor.h 630627 2014-11-24 14:35:22Z emoyse $
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
  /// object meant to handle one of the isolation values. The function throws
  /// an exception if it is given an enumeration value that's out of bounds.
  ///
  /// @param type The Iso::IsolationType type
  /// @returns A pointer to the Accessor object if successful, or a
  ///          null-pointer if not
  SG::AuxElement::Accessor< float >*
  getIsolationAccessor( Iso::IsolationType type );

} // namespace xAOD

#endif // XAODPRIMITIVES_TOOLS_GETISOLATIONACCESSOR_H
