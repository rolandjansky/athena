/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASALG_G4ATLASMTRUNMANAGER_H
#define G4ATLASALG_G4ATLASMTRUNMANAGER_H

#include "G4MTRunManager.hh"
#include "AthenaKernel/MsgStreamMember.h"

//#include "G4AtlasInterfaces/ISensitiveDetectorSvc.h"
//#include "G4AtlasInterfaces/IFastSimulationSvc.h"


/// @brief ATLAS master run manager for master-slave multi-threading model
class G4AtlasMTRunManager: public G4MTRunManager {

public:

  /// Get the (pure) singleton instance
  static G4AtlasMTRunManager* GetG4AtlasMTRunManager();

  /// G4 function called at end of run
  void RunTermination();

  /// We cram all of the initialization of the run manager stuff in here.
  /// This then includes some of the things that in normal G4 are called
  /// immediately before the event loop.
  void Initialize();

  /// Disable G4's barrier synchronization by implementing these methods
  /// and leaving them empty
  virtual void ThisWorkerReady() {};
  virtual void ThisWorkerEndEventLoop() {};

protected:

  /// Initialize the G4 geometry on the master
  void InitializeGeometry();

  // Initialize the physics list on the master
  void InitializePhysics();

  /// Disable G4's barrier synchronization by implementing these methods
  /// and leaving them empty
  virtual void WaitForReadyWorkers() {};
  virtual void WaitForEndEventLoopWorkers() {};

private:

  /// Pure singleton private constructor
  G4AtlasMTRunManager();

  /// Log a message using the Athena controlled logging system
  MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }

private:

  /// Private message stream member
  mutable Athena::MsgStreamMember m_msg;

  //ISensitiveDetectorSvc* m_senDetSvc;

};


#endif

