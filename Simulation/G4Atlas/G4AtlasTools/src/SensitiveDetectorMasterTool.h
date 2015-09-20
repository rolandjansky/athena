/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASSERVICES_SENSITIVEDETECTORMASTERTOOL_H
#define G4ATLASSERVICES_SENSITIVEDETECTORMASTERTOOL_H

// Base classes
#include "AthenaBaseComps/AthAlgTool.h"
#include "G4AtlasInterfaces/ISensitiveDetectorMasterTool.h"

// Member
#include "GaudiKernel/ToolHandle.h" // For tool handle array
#include "G4AtlasInterfaces/ISensitiveDetector.h" // What we have a handle on

class SensitiveDetectorMasterTool : public AthAlgTool , public virtual ISensitiveDetectorMasterTool {
 public:
  // Standard constructor and destructor
  SensitiveDetectorMasterTool(const std::string& type, const std::string& name,
                              const IInterface* parent);
  virtual ~SensitiveDetectorMasterTool() {}

  // Gaudi methods
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  static const InterfaceID& interfaceID() { return ISensitiveDetectorMasterTool::interfaceID(); }

  /// Retrieve the SD tools. SD creation is deferred until initializeSDs.
  StatusCode initialize() override final;

  // Base class methods
  StatusCode initializeSDs() override final; ///!< Base class method to initialize all the SDs
  StatusCode BeginOfAthenaEvent() override final; ///!< Base class method that calls setup for all SDs
  StatusCode EndOfAthenaEvent() override final; ///!< Base class method that calls gather for all SDs

 private:
  ToolHandleArray<ISensitiveDetector> m_SenDetList; ///!< Private array of tool handles pointing to all SDs
};

#endif
