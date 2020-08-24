/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ConstantFieldSvc.h"

// Geant4 includes
#include "G4UniformMagField.hh"

ConstantFieldSvc::ConstantFieldSvc(const std::string& name, ISvcLocator* pSvcLocator)
  : G4MagFieldSvcBase(name, pSvcLocator)
{
}

StatusCode ConstantFieldSvc::initialize()
{
  ATH_MSG_DEBUG( "Initializing " << name() );
  return StatusCode::SUCCESS;
}

G4MagneticField* ConstantFieldSvc::makeField()
{
  ATH_MSG_VERBOSE( "ConstantFieldSvc::makeField" );
  const G4ThreeVector fieldVec(m_fieldX, m_fieldY, m_fieldZ);
  return new G4UniformMagField(fieldVec);
}
