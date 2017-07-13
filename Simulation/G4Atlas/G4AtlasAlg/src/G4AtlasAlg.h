/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASALG_G4AtlasAlg_H
#define G4ATLASALG_G4AtlasAlg_H

// Base class header
#include "AthenaBaseComps/AthAlgorithm.h"

// STL headers
#include <map>
#include <string>

// Gaudi headers
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// Athena headers
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "G4AtlasInterfaces/IUserActionSvc.h"
#include "G4AtlasInterfaces/IDetectorGeometrySvc.h"
#include "G4AtlasInterfaces/ISensitiveDetectorMasterTool.h"
#include "G4AtlasInterfaces/IFastSimulationMasterTool.h"
#include "G4AtlasInterfaces/IPhysicsListTool.h"
#include "G4AtlasInterfaces/IG4AtlasSvc.h"
#include "GeneratorObjects/McEventCollection.h"

// ISF includes
#include "ISF_Interfaces/ITruthSvc.h"
#include "ISF_Interfaces/IGeoIDSvc.h"
#include "ISF_Interfaces/IInputConverter.h"

/// @class G4AtlasAlg
/// @brief Primary Athena algorithm for ATLAS simulation.
///
/// During initialization, this class sets up several things, including:
/// - the ATLAS (master) run manager
/// - physics list assignment to G4
/// - detector construction (currently FADS::FadsDetectorConstruction)
///
/// During the event loop, it handles processing of the event by
/// invoking the (worker) run manager.
///
class G4AtlasAlg : public AthAlgorithm
{

public:

  /// Standard algorithm constructor
  G4AtlasAlg(const std::string& name, ISvcLocator* pSvcLocator);

  /// Virtual destructor
  virtual ~G4AtlasAlg() {  };

  /// this Alg is Clonable (for AthenaMT)
  bool isClonable() const override { return true; }

  /// @brief Initialize the algorithm.
  ///
  /// Here we setup several things for simulation, including:
  /// - force intialization of the UserActionSvc
  /// - apply custom G4 UI commands (like custom physics list)
  /// - configure the particle generator and random generator svc
  StatusCode initialize() override;

  /// Finalize the algorithm and invoke G4 run termination.
  StatusCode finalize() override;

  /// @brief Simulate one Athena event.
  StatusCode execute() override;

  /// Poorly named possibly unused method which sets some verbosities.
  void initializeG4();

  /// G4 initialization called only by the first alg instance.
  /// This is done (for now) because we get multiple alg instances in hive.
  void initializeOnce();

  /// G4 finalization called only by the first alg instance.
  /// This is done (for now) because we get multiple alg instances in hive.
  void finalizeOnce();

private:

  /// Releases the GeoModel geometry from memory once it has been used
  /// to build the G4 geometry and is no-longer required
  StatusCode releaseGeoModel();

  /// Properties for the jobOptions
  std::string m_libList;
  std::string m_physList;
  std::string m_fieldMap;
  std::string m_rndmGen;
  bool m_releaseGeoModel;
  bool m_recordFlux;
  bool m_killAbortedEvents;
  bool m_flagAbortedEvents;
  SG::ReadHandle<McEventCollection>    m_inputTruthCollection; //!< input hard scatter collection
  SG::WriteHandle<McEventCollection>   m_outputTruthCollection;//!< output hard scatter truth collection
  /// Verbosity settings for Geant4
  std::map<std::string,std::string> m_verbosities;

  /// Commands to send to the G4 UI
  std::vector<std::string> m_g4commands;

  /// Activate multi-threading configuration
  bool m_useMT;

  /// Random number Service
  ServiceHandle<IAtRndmGenSvc> m_rndmGenSvc;
  /// G4Atlas Service - handles G4 initialization
  ServiceHandle<IG4AtlasSvc> m_g4atlasSvc;
  /// User Action Service
  ServiceHandle<G4UA::IUserActionSvc> m_userActionSvc;
  /// Detector Geometry Service (builds G4 Geometry)
  ServiceHandle<IDetectorGeometrySvc> m_detGeoSvc;
  /// Service to convert ISF_Particles into a G4Event
  ServiceHandle<ISF::IInputConverter> m_inputConverter;
  /// Central Truth Service
  ServiceHandle<ISF::ITruthSvc> m_truthRecordSvc;
  /// Geo ID Service
  ServiceHandle<ISF::IGeoIDSvc> m_geoIDSvc;
  /// Physics List Tool
  ToolHandle<IPhysicsListTool> m_physListTool;
  /// Sensitive Detector Master Tool
  ToolHandle<ISensitiveDetectorMasterTool> m_senDetTool;
  /// Fast Simulation Master Tool
  ToolHandle<IFastSimulationMasterTool> m_fastSimTool;

};

#endif// G4ATLASALG_G4AtlasAlg_H
