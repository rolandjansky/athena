/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  ~ConstantFieldSvc() = default;

  /** Athena method. called at initialization time, being customized here */
  StatusCode initialize() override final;

 protected:

  /// Create the field object
  G4MagneticField* makeField() override final;

 private:

  /// @name Field vector components - What are the units??
  /// @{
  Gaudi::Property<double> m_fieldX{this, "FieldX", 0., "Field X component"};
  Gaudi::Property<double> m_fieldY{this, "FieldY", 0., "Field Y component"};
  Gaudi::Property<double> m_fieldZ{this, "FieldZ", 0., "Field Z component"};
  /// @}
};

#endif
