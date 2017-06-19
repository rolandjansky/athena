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
#include "G4AtlasInterfaces/IUserActionSvc.h"

// Gaudi headers
#include "GaudiKernel/ToolHandle.h"


/// ATLAS custom singleton run manager.
///
/// This is the run manager used for serial (not-MT) jobs.
/// @todo sync and reduce code duplication with MT run managers.
///
class G4AtlasRunManager: public G4RunManager {

  // Is this needed?
  friend class G4AtlasAlg;
  // Is this needed?
  friend class SimControl;

public:

  virtual ~G4AtlasRunManager() {}

  /// Retrieve the singleton instance
  static G4AtlasRunManager* GetG4AtlasRunManager();

  G4Event* GenerateEvent(G4int i_event) override final;

  /// Run the simulation for one event.
  /// @todo rename method.
  bool SimulateFADSEvent();

  void RunTermination() override final;
  void SetCurrentG4Event(int);

protected:

  /// @name Overridden G4 init methods for customization
  /// @{
  void Initialize() override final;
  void InitializeGeometry() override final;
  void InitializePhysics() override final;
  /// @}

private:

  /// Private constructor
  G4AtlasRunManager();

  void EndEvent();
  /// @name Methods related to flux recording
  /// @{
  /// Initialize flux recording
  void InitializeFluxRecording();
  /// Record fluxes from current event
  void RecordFlux();
  /// Dump flux information to text files
  void WriteFluxInformation();
  /// @}

  /// Configure the user action service handle
  void SetUserActionSvc(const std::string& typeAndName) {
    m_userActionSvc.setTypeAndName(typeAndName);
  }

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

  /// Handle to the user action service
  ServiceHandle<G4UA::IUserActionSvc> m_userActionSvc;
};


#endif // G4ATLASALG_G4AtlasRunManager_h

