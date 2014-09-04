/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef G4AtlasRunManager_h
#define G4AtlasRunManager_h 1

#include "G4RunManager.hh"
#include "G4VUserPhysicsList.hh"
#include "GaudiKernel/MsgStream.h"

#include <vector>

class StoreGateSvc;
class MsgStream;

class G4AtlasRunManager: public G4RunManager {

  friend class G4AtlasAlg;
  friend class SimControl;

public:

  virtual ~G4AtlasRunManager() { if (m_log) delete m_log; }

  static G4AtlasRunManager* GetG4AtlasRunManager();

  void SetPhysicsList(G4VUserPhysicsList* p) { m_pl = p; }
  G4Event* GenerateEvent(G4int i_event);
  bool SimulateFADSEvent();
  void RunTermination();
  void SetCurrentG4Event(int);

protected:

  void InitializeGeometry();
  void InitializePhysics();

private:

  G4AtlasRunManager();
  void EndEvent();

  void SetStoreGatePtr( StoreGateSvc *sgs) { m_sgSvc = sgs; }
  void SetReleaseGeo(bool b) { m_releaseGeo = b; }
  void SetLogLevel(int i) { log().setLevel(i); }

private:

  G4VUserPhysicsList* m_pl;
  StoreGateSvc* m_sgSvc;
  bool m_releaseGeo;
  MsgStream * m_log;
  MsgStream log();
};


#endif

