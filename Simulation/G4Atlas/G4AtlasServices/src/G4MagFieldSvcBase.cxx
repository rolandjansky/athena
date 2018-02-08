/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4MagFieldSvcBase.h"

// Geant4 includes
#include "G4MagneticField.hh"

//-----------------------------------------------------------------------------
// Implementation file for class : G4MagFieldSvcBase
//
// 2015-11-17: Andrea Dell'Acqua
//-----------------------------------------------------------------------------


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
G4MagFieldSvcBase::G4MagFieldSvcBase(const std::string& name,
                                     ISvcLocator* pSvcLocator)
  : base_class(name, pSvcLocator)
{
}

//=============================================================================
// Retrieve a G4 mag field object
//=============================================================================
G4MagneticField* G4MagFieldSvcBase::getField()
{
  ATH_MSG_DEBUG("G4MagFieldSvcBase::getField");

  // Check if we already have a field setup for current thread
  auto field = m_fieldHolder.get();
  // If not, we create one
  if(!field) {
    field = makeField();
    m_fieldHolder.set(field);
  }

  return field;
}
