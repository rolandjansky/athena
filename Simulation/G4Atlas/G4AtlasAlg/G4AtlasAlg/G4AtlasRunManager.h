/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "G4AtlasInterfaces/IPhysicsListSvc.h"
#include "G4AtlasInterfaces/IDetectorGeometrySvc.h"
#include "G4AtlasInterfaces/IFluxRecorder.h"

/// ATLAS custom singleton run manager.
///
/// This is the run manager used for serial (not-MT) jobs.
/// @todo sync and reduce code duplication with MT run managers.
///

class G4AtlasRunManager: public G4RunManager {

public:

  virtual ~G4AtlasRunManager() {}

  /// Retrieve the singleton instance
  static G4AtlasRunManager* GetG4AtlasRunManager();

  /// Does the work of simulating an ATLAS event
  bool ProcessEvent(G4Event* event);

  /// G4 function called at end of run
  void RunTermination() override final;

  /// Configure the detector geometry service handle
  void SetDetGeoSvc(const std::string& typeAndName) {
    m_detGeoSvc.setTypeAndName(typeAndName);
  }

  /// Configure the Fast Simulation Master Tool handle
  void SetFastSimMasterTool(const std::string& typeAndName) {
    m_fastSimTool.setTypeAndName(typeAndName);
  }

  /// Configure the Physics List Tool handle
  void SetPhysListSvc(const std::string& typeAndName) {
    m_physListSvc.setTypeAndName(typeAndName);
  }

  void SetRecordFlux(bool b, std::unique_ptr<IFluxRecorder> f) { m_recordFlux = b; m_fluxRecorder=std::move(f);}
  void SetLogLevel(int) { /* Not implemented */ }
  /// @}

  void SetVolumeSmartlessLevel(const std::map<std::string,double>& nameAndValue){
    m_volumeSmartlessLevel = nameAndValue;
  }

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

  /// Log a message using the Athena controlled logging system
  MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }

  /// Private message stream member
  mutable Athena::MsgStreamMember m_msg;

  bool m_recordFlux;

  ToolHandle<IFastSimulationMasterTool> m_fastSimTool;
  ServiceHandle<IPhysicsListSvc> m_physListSvc;

  ServiceHandle<IDetectorGeometrySvc> m_detGeoSvc;

  /// Interface to flux recording

  std::unique_ptr<IFluxRecorder> m_fluxRecorder;

  //Property to allow an arbitrary volume (named by string) to have its
  //"smartless" value set
  std::map<std::string, double> m_volumeSmartlessLevel;

};

#endif // G4ATLASALG_G4AtlasRunManager_h
