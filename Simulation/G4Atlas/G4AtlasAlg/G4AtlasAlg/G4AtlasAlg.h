/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASALG_G4AtlasAlg_H
#define G4ATLASALG_G4AtlasAlg_H

// Base class header
#include "AthenaBaseComps/AthAlgorithm.h"

// Athena headers
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "G4AtlasInterfaces/IUserActionSvc.h"
#include "G4AtlasInterfaces/IPhysicsListTool.h"

// Gaudi headers
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// STL headers
#include <map>
#include <string>

//class AthenaStackingAction;
//class AthenaTrackingAction;
class G4AtlasRunManager;

template <class ConcreteAlgorithm> class AlgFactory; //FIXME - safe to remove?


class G4AtlasAlg : public AthAlgorithm {

  friend class AlgFactory<G4AtlasAlg>; //FIXME - safe to remove?

public:

  G4AtlasAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~G4AtlasAlg() {  };

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();

  void initializeG4();

private:

  G4AtlasRunManager* p_runMgr;

  // Properties for the jobOptions
  std::string m_libList;
  std::string m_physList;
  std::string m_generator;
  std::string m_fieldMap;
  std::string m_rndmNumberSource;
  bool m_releaseGeoModel;
  bool m_recordFlux;
  bool m_IncludeParentsInG4Event;
  bool m_killAbortedEvents;
  bool m_flagAbortedEvents;

  std::map<std::string,std::string> m_verbosities;

  // Random number service
  ServiceHandle<IAtRndmGenSvc> m_rndmGenSvc;
  ServiceHandle<IUserActionSvc> m_UASvc;
  ToolHandle<IPhysicsListTool> m_physListTool;
};

#endif // G4ATLASALG_G4AtlasAlg_H
