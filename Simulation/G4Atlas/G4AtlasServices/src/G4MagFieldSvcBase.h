/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASSERVICES_G4MagFieldSvcBase_H
#define G4ATLASSERVICES_G4MagFieldSvcBase_H

// Geant4 includes
#include "G4SystemOfUnits.hh"

// Framework includes
#include "AthenaBaseComps/AthService.h"

// G4Atlas includes
#include "G4AtlasInterfaces/IG4FieldSvc.h"
#include "G4AtlasTools/ThreadLocalHolder.h"

// Forward declarations
class G4MagneticField;


/** @class G4MagFieldSvcBase G4MagFieldSvcBase.h "G4AtlasServices/G4MagFieldSvcBase.h"
 *
 *  Base class for services that create/manage G4 magnetic fields.
 *
 *  @author Andrea Dell'Acqua
 *  @date   2015-11-17
 */
class G4MagFieldSvcBase : public extends<AthService, IG4FieldSvc> {

 public:
  /// Standard constructor
  G4MagFieldSvcBase( const std::string& name, ISvcLocator* pSvcLocator );

  /// Empty destructor
  virtual ~G4MagFieldSvcBase() {};

  /// @brief Retrieve a G4 magnetic field object.
  /// If one doesn't yet exist in the current thread, it will be created.
  G4MagneticField* getField() override final;

 protected:

  /// This method must be overridden by concrete tools to create a new field.
  virtual G4MagneticField* makeField() = 0;

  /// Thread-local field object
  thread_utils::ThreadLocalOwner<G4MagneticField> m_fieldHolder;

};

#endif // G4ATLASSERVICES_G4AtlasFieldTool_H
