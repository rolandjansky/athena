/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASSERVICES_ConstantFieldSvc_H
#define G4ATLASSERVICES_ConstantFieldSvc_H

// Base classes
#include "G4MagFieldSvcBase.h"

// Geant4
#include "G4MagneticField.hh"

// STL library
#include <string>

/// @todo Needs documentation
///
class ConstantFieldSvc final : public G4MagFieldSvcBase
{
 public:

  /// Basic constructor and destructor
  ConstantFieldSvc(const std::string& name, ISvcLocator* pSvcLocator);
  ~ConstantFieldSvc() {}

  /** Athena method. called at initialization time, being customized here */
  StatusCode initialize() override final;

 protected:

  /// Create the field object
  G4MagneticField* makeField() override final;

 private:

  /// @name Field vector components
  /// @{
  double m_fieldX;
  double m_fieldY;
  double m_fieldZ;
  /// @}
};

#endif
