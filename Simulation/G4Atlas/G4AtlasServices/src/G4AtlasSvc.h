/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASSERVICES_G4AtlasSvc_H
#define G4ATLASSERVICES_G4AtlasSvc_H

// Base classes
#include "AthenaBaseComps/AthService.h"
#include "G4AtlasInterfaces/IG4AtlasSvc.h"
#include "G4AtlasInterfaces/IDetectorGeometrySvc.h"
#include "G4AtlasInterfaces/IPhysicsListTool.h"
// Member

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

class G4AtlasSvc : public AthService , public virtual IG4AtlasSvc {
 public:
  // Standard constructor and destructor
  G4AtlasSvc( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~G4AtlasSvc();

  // Gaudi methods
  StatusCode initialize() override final;
  StatusCode finalize() override final;
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface ) override final;
  static const InterfaceID& interfaceID() { return IG4AtlasSvc::interfaceID(); }

 private:

  ServiceHandle<IDetectorGeometrySvc> m_detGeoSvc;

  ToolHandle<IPhysicsListTool> m_physicsListTool;

  bool m_activateParallelGeometries;
};

#endif
