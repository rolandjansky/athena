/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASSERVICES_StandardFieldSvc_H
#define G4ATLASSERVICES_StandardFieldSvc_H

// STL library
#include <string>

// Geant4
#include "G4MagneticField.hh"

// Gaudi/Athena
#include "GaudiKernel/ServiceHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"

// Base classes
#include "G4MagFieldSvcBase.h"


/// @class AtlasField
/// @brief G4 wrapper around the main ATLAS magnetic field service.
///
/// @todo TODO this should probably be put in a good namespace.
///
class AtlasField : public G4MagneticField
{
  public:

    /// Construct the field object from the IMagFieldSvc
    AtlasField(MagField::IMagFieldSvc* m);

    /// Implementation of G4 method to retrieve field value
    void GetFieldValue(const double *point, double *field) const
    {
      m_magFieldSvc->getField(point, field);
    }

  private:

    /// Pointer to the magnetic field service.
    /// We use a raw pointer here to avoid ServiceHandle overhead.
    MagField::IMagFieldSvc* m_magFieldSvc;
};


/// @class StandardFieldSvc
/// @brief Athena service for constructing the AtlasField object
///
/// @todo TODO this should probably be put in a good namespace.
///
class StandardFieldSvc final : public G4MagFieldSvcBase
{
  public:

    /// Standard constructor
    StandardFieldSvc(const std::string& name, ISvcLocator* pSvcLocator);
    /// Empty destructor
    ~StandardFieldSvc() {}

    /// Athena method. called at initialization time, being customized here.
    StatusCode initialize() override final;

  protected:

    /// Create/retrieve the AtlasField object
    G4MagneticField* makeField() override final;

  private:

    /// Handle to the ATLAS magnetic field service
    ServiceHandle<MagField::IMagFieldSvc> m_magFieldSvc;

};

#endif // G4ATLASSERVICES_StandardFieldSvc_H
