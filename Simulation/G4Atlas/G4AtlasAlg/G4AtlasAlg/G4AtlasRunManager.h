/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASALG_G4AtlasRunManager_h
#define G4ATLASALG_G4AtlasRunManager_h

// Base class header
#include "G4RunManager.hh"

// Athena headers
#include "AthenaKernel/MsgStreamMember.h"
#include "G4AtlasInterfaces/ISensitiveDetectorMasterTool.h"
#include "G4AtlasInterfaces/IFastSimulationMasterTool.h"
#include "G4AtlasInterfaces/IPhysicsListTool.h"

// Gaudi headers
#include "GaudiKernel/ToolHandle.h"

class G4AtlasRunManager: public G4RunManager {

  friend class G4AtlasAlg;
  friend class SimControl;

public:

  virtual ~G4AtlasRunManager() {}

  static G4AtlasRunManager* GetG4AtlasRunManager();

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

  bool m_releaseGeo;
  bool m_recordFlux;

  ToolHandle<ISensitiveDetectorMasterTool> m_senDetTool;
  ToolHandle<IFastSimulationMasterTool> m_fastSimTool;
  ToolHandle<IPhysicsListTool> m_physListTool;
};


#endif // G4ATLASALG_G4AtlasRunManager_h

