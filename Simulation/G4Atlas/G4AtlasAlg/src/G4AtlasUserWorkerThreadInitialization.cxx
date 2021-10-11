/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Hide multi-threading classes from builds without G4MT
#include "G4Types.hh"
#ifdef G4MULTITHREADED

#include "G4AtlasAlg/G4AtlasUserWorkerThreadInitialization.h"
#include "G4AtlasAlg/G4AtlasWorkerRunManager.h"

G4WorkerRunManager* G4AtlasUserWorkerThreadInitialization::
CreateWorkerRunManager() const
{
  // TODO: maybe better to just use the pseudo-singleton mechanism
  auto* workerRunManager = G4AtlasWorkerRunManager::GetG4AtlasWorkerRunManager();
  workerRunManager->SetDetGeoSvc(m_detGeoSvcName);
  workerRunManager->SetFastSimMasterTool(m_fastSimToolName);
  return workerRunManager;
}

#endif // G4MULTITHREADED
