/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASSERVICES_SENSITIVEDETECTORSVC_H
#define G4ATLASSERVICES_SENSITIVEDETECTORSVC_H

// Base classes
#include "AthenaBaseComps/AthService.h"
#include "G4AtlasInterfaces/ISensitiveDetectorSvc.h"

// Member
#include "GaudiKernel/ToolHandle.h" // For tool handle array
#include "G4AtlasInterfaces/ISensitiveDetector.h" // What we have a handle on

class SensitiveDetectorSvc : public AthService , public virtual ISensitiveDetectorSvc {
 public:
  // Standard constructor and destructor
  SensitiveDetectorSvc( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~SensitiveDetectorSvc() {}

  // Gaudi methods
  StatusCode initialize();
  StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  static const InterfaceID& interfaceID() { return ISensitiveDetectorSvc::interfaceID(); }

  void EndOfAthenaEvent() override final; ///!< Base class method that calls gather for all SDs

 private:
  ToolHandleArray<ISensitiveDetector> m_SenDetList; ///!< Private array of tool handles pointing to all SDs

};

#endif