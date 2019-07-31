/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
  return G4AtlasWorkerRunManager::GetG4AtlasWorkerRunManager();
}

#endif // G4MULTITHREADED
