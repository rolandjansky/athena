/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASSERVICES_DETECTORGEOMETRYSVC_H
#define G4ATLASSERVICES_DETECTORGEOMETRYSVC_H

// Base classes
#include "AthenaBaseComps/AthService.h"
#include "G4AtlasInterfaces/IDetectorGeometrySvc.h"
#include "G4AtlasInterfaces/IDetectorConstructionTool.h"
#include "G4AtlasInterfaces/IRegionCreator.h"
#include "G4AtlasInterfaces/IParallelWorldTool.h"

class G4VUserDetectorConstruction;
class G4VNotifier;

// Member
#include "GaudiKernel/ToolHandle.h" // For tool handle array
#include "G4AtlasInterfaces/IDetectorGeometryTool.h" // What we have a handle on

class DetectorGeometrySvc : public AthService , public virtual IDetectorGeometrySvc {
 public:
  // Standard constructor and destructor
  DetectorGeometrySvc( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~DetectorGeometrySvc();

  // Gaudi methods
  StatusCode initialize();
  StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  static const InterfaceID& interfaceID() { return IDetectorGeometrySvc::interfaceID(); }

  G4VUserDetectorConstruction* GetDetectorConstruction() final;
  
  void SetCurrentDetectorName(std::string s) {m_currentDetectorName=s;}
  const std::string GetCurrentDetectorName() const {return m_currentDetectorName;}
  
  bool ParallelWorldsActivated() {return m_activateParallelWorlds;}

  void ActivateParallelWorlds() {m_activateParallelWorlds=true;}

  std::vector<std::string>& GetParallelWorldNames();
  
 private:
  ToolHandle<IDetectorGeometryTool> m_detTool;
  ToolHandle<IDetectorConstructionTool> m_detConstruction;
  ToolHandleArray<IRegionCreator> m_regionCreators;
  ToolHandleArray<IParallelWorldTool> m_parallelWorlds;
  
  std::string m_currentDetectorName;
  
  bool m_activateLVNotifier=false;
  bool m_activatePVNotifier=true;
  G4VNotifier* lvNotifier;
  G4VNotifier* pvNotifier;
  
  bool m_activateParallelWorlds;
  std::vector<std::string> m_parallelWorldNames;
};

#endif
