/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASALG_G4ATLASWORKERRUNMANAGER_H
#define G4ATLASALG_G4ATLASWORKERRUNMANAGER_H

// Hide multi-threading classes from builds without G4MT
#include "G4Types.hh"
#ifdef G4MULTITHREADED

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "G4WorkerRunManager.hh"
#include "AthenaKernel/MsgStreamMember.h"

#include "G4AtlasInterfaces/IDetectorGeometrySvc.h"
#include "G4AtlasInterfaces/ISensitiveDetectorMasterTool.h"
#include "G4AtlasInterfaces/IFastSimulationMasterTool.h"


/// @brief ATLAS worker run manager for master-slave multi-threading model
class G4AtlasWorkerRunManager : public G4WorkerRunManager {

public:

  /// Get the (pure) singleton instance
  static G4AtlasWorkerRunManager* GetG4AtlasWorkerRunManager();

  /// We cram all of the necessary worker run manager initialization here.
  /// In G4 some of it is called instead under BeamOn
  void Initialize() override final;

  /// Does the work of simulating an ATLAS event
  bool ProcessEvent(G4Event* event);

  /// G4 function called at end of run
  void RunTermination() override final;

  /// @name Methods to pass configuration in from G4AtlasUserWorkerThreadInitialization
  /// @{
  /// Configure the detector geometry service handle
  void SetDetGeoSvc(const std::string& typeAndName) {
    m_detGeoSvc.setTypeAndName(typeAndName);
  }

  /// Configure the Fast Simulation Master Tool handle
  void SetFastSimMasterTool(const std::string& typeAndName) {
    m_fastSimTool.setTypeAndName(typeAndName);
  }
  /// @}

protected:

  /// Initialize the geometry on the worker
  void InitializeGeometry() override final;

  /// Initialize the physics on the worker
  void InitializePhysics() override final;

private:

  /// Pure singleton private constructor
  G4AtlasWorkerRunManager();

  /// This command prints a message about a G4Command depending on its returnCode
  void CommandLog(int returnCode, const std::string& commandString) const;

  /// Log a message using the Athena controlled logging system
  MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }

private:

  /// Private message stream member
  mutable Athena::MsgStreamMember m_msg;

  /// Handle to the detector service
  ServiceHandle<IDetectorGeometrySvc> m_detGeoSvc;

  /// Handle to the FastSim tool
  ToolHandle<IFastSimulationMasterTool> m_fastSimTool;

};

#endif // G4MULTITHREADED

#endif
