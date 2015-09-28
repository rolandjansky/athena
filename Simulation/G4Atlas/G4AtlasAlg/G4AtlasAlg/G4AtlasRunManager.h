/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AtlasRunManager_h
#define G4AtlasRunManager_h

#include "G4RunManager.hh"
#include "G4VUserPhysicsList.hh"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/MsgStreamMember.h"
#include <vector>

class StoreGateSvc;
#include "G4AtlasInterfaces/ISensitiveDetectorMasterTool.h"
#include "G4AtlasInterfaces/IFastSimulationMasterTool.h"


class G4AtlasRunManager: public G4RunManager {

  friend class G4AtlasAlg;
  friend class SimControl;

public:

  virtual ~G4AtlasRunManager() {}

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
  void SetRecordFlux(bool b) { m_recordFlux = b; }
  void SetLogLevel(int) { /* Not implemented */ }

  /// Log a message using the Athena controlled logging system
  MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }

private:
  /// Private message stream member
  mutable Athena::MsgStreamMember m_msg;

  G4VUserPhysicsList* m_pl;
  StoreGateSvc* m_sgSvc;
  bool m_releaseGeo;
  bool m_recordFlux;

  ToolHandle<ISensitiveDetectorMasterTool> m_senDetTool;
  ToolHandle<IFastSimulationMasterTool> m_fastSimTool;
};


#endif

