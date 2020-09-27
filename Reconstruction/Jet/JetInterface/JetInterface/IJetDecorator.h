/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IJetDecorator_H
#define IJetDecorator_H

// Bill Balunas
// February 2019

/// Interface for adding a decoration to a jet container.
///
/// This is a special case of IJetModifier which only adds new decorations to
/// the jet container and cannot modify any of the existing variables. It can
/// operate on `const xAOD::JetContainer`, which a general `IJetModifier`
/// cannot. An implementation of `modify()` is provided for cases where the
/// const-ness of the container doesn't matter.

#include "JetInterface/IJetModifier.h"

class IJetDecorator : virtual public IJetModifier {

// From AsgTools/AsgToolMacros.h
ASG_TOOL_INTERFACE(IJetDecorator)

public:

  /// Decorate a jet collection without otherwise modifying it.
  /// Returns 0 for success.
  virtual int decorate(const xAOD::JetContainer& jets) const = 0;

  /// Concrete implementation of the function inherited from IJetModifier.
  virtual int modify(xAOD::JetContainer& jets) const override final {return decorate(jets);};

};

#endif
