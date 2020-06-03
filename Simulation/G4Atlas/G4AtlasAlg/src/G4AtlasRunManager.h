/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASALG_G4AtlasRunManager_h
#define G4ATLASALG_G4AtlasRunManager_h

// Base class header
#include "G4RunManager.hh"

// Gaudi headers
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// Athena headers
#include "AthenaKernel/MsgStreamMember.h"
#include "G4AtlasInterfaces/ISensitiveDetectorMasterTool.h"
#include "G4AtlasInterfaces/IFastSimulationMasterTool.h"
#include "G4AtlasInterfaces/IPhysicsListTool.h"
#include "G4AtlasInterfaces/IUserActionSvc.h"
#include "G4AtlasInterfaces/IDetectorGeometrySvc.h"

/// ATLAS custom singleton run manager.
///
/// This is the run manager used for serial (not-MT) jobs.
/// @TODO sync and reduce code duplication with MT run managers.
///

class G4AtlasRunManager: public G4RunManager {

  friend class G4AtlasAlg;   // Is this needed?

public:

  virtual ~G4AtlasRunManager() {}

  /// Retrieve the singleton instance
  static G4AtlasRunManager* GetG4AtlasRunManager();

  /// Does the work of simulating an ATLAS event
  bool ProcessEvent(G4Event* event);

  /// Pass G4Commands for later execution
  void AddG4Command(const std::string& g4Command);

  /// G4 function called at end of run
  void RunTermination() override final;

protected:

  /// @name Overridden G4 init methods for customization
  /// @{
  void Initialize() override final;
  void InitializeGeometry() override final;
  void InitializePhysics() override final;
  /// @}

private:

  /// Pure singleton private constructor
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

  /// This method attempts to execute G4Commands via the G4UImanager
  void AttemptG4Commands(std::vector<std::string>& commands);
  /// This command prints a message about a G4Command depending on its returnCode
  void CommandLog(int returnCode, const std::string& commandString) const;

  /// @name Methods to pass configuration in from G4AtlasAlg
  /// @{
  /// Configure the user action service handle
  void SetUserActionSvc(const std::string& typeAndName) {
    m_userActionSvc.setTypeAndName(typeAndName);
  }

  /// Configure the detector geometry service handle
  void SetDetGeoSvc(const std::string& typeAndName) {
    m_detGeoSvc.setTypeAndName(typeAndName);
  }

  /// Configure the Sensitive Detector Master Tool handle
  void SetSDMasterTool(const std::string& typeAndName) {
    m_senDetTool.setTypeAndName(typeAndName);
  }

  /// Configure the Fast Simulation Master Tool handle
  void SetFastSimMasterTool(const std::string& typeAndName) {
    m_fastSimTool.setTypeAndName(typeAndName);
  }

  /// Configure the Physics List Tool handle
  void SetPhysListTool(const std::string& typeAndName) {
    m_physListTool.setTypeAndName(typeAndName);
  }

  void SetRecordFlux(bool b) { m_recordFlux = b; }
  void SetLogLevel(int) { /* Not implemented */ }
  /// @}

  /// Log a message using the Athena controlled logging system
  MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }

  /// Private message stream member
  mutable Athena::MsgStreamMember m_msg;

  bool m_recordFlux;

  ToolHandle<ISensitiveDetectorMasterTool> m_senDetTool;
  ToolHandle<IFastSimulationMasterTool> m_fastSimTool;
  ToolHandle<IPhysicsListTool> m_physListTool;

  /// Handle to the user action service
  ServiceHandle<G4UA::IUserActionSvc> m_userActionSvc;
  ServiceHandle<IDetectorGeometrySvc> m_detGeoSvc;

  /// Commands to send to the G4 UI
  std::vector<std::string> m_g4commands;
};

#endif // G4ATLASALG_G4AtlasRunManager_h
