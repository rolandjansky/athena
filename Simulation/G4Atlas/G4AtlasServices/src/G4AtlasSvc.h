/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASSERVICES_G4AtlasSvc_H
#define G4ATLASSERVICES_G4AtlasSvc_H

// Base classes
#include "AthenaBaseComps/AthService.h"
#include "G4AtlasInterfaces/IG4AtlasSvc.h"
#include "G4AtlasInterfaces/IDetectorGeometrySvc.h"
#include "G4AtlasInterfaces/IPhysicsListSvc.h"
#include "G4AtlasInterfaces/IUserLimitsSvc.h"
// Member

#include "GaudiKernel/ServiceHandle.h"

class G4AtlasSvc : public extends<AthService , IG4AtlasSvc> {
 public:
  // Standard constructor and destructor
  G4AtlasSvc( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~G4AtlasSvc() = default;

  // Gaudi methods
  StatusCode initialize() override final;
  StatusCode finalize() override final;

 private:
  ServiceHandle<IDetectorGeometrySvc> m_detGeoSvc{this, "DetectorGeometrySvc", "DetectorGeometrySvc"};
  ServiceHandle<IPhysicsListSvc> m_physicsListSvc{this, "PhysicsListSvc", "PhysicsListSvc"};
  ServiceHandle<IUserLimitsSvc> m_userLimitsSvc{this, "UserLimitsSvc", "UserLimitsSvc"};
  Gaudi::Property<bool> m_isMT{this, "isMT", false};
  Gaudi::Property<bool> m_activateParallelGeometries{this, "ActivateParallelWorlds", false, "Toggle on/off the G4 parallel geometry system"};
};

#endif
