/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASSERVICES_DETECTORGEOMETRYSVC_H
#define G4ATLASSERVICES_DETECTORGEOMETRYSVC_H

// Base classes
#include "AthenaBaseComps/AthService.h"
#include "G4AtlasInterfaces/IDetectorGeometrySvc.h"

// Athena headers
#include "G4AtlasInterfaces/IDetectorConstructionTool.h"
#include "G4AtlasInterfaces/IRegionCreator.h"
#include "G4AtlasInterfaces/IParallelWorldTool.h"
#include "G4AtlasInterfaces/IDetectorGeometryTool.h"
#include "G4AtlasInterfaces/IFieldManagerTool.h"
#include "G4AtlasInterfaces/IG4GeometryConfigurationTool.h"

// Gaudi headers
#include "GaudiKernel/ToolHandle.h" // For tool handle array

class G4VUserDetectorConstruction;

class DetectorGeometrySvc : public extends<AthService, IDetectorGeometrySvc> {
public:
  // Standard constructor and destructor
  DetectorGeometrySvc( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~DetectorGeometrySvc() = default;

  // Gaudi methods
  StatusCode initialize() override final;
  StatusCode finalize() override final;

  /// Setup the magnetic field managers for configured volumes
  StatusCode initializeFields() override final;

  G4VUserDetectorConstruction* GetDetectorConstruction() final;

  bool ParallelWorldsActivated() {return m_activateParallelWorlds;}

  void ActivateParallelWorlds() override final {m_activateParallelWorlds=true;}

  std::vector<std::string>& GetParallelWorldNames() override final;

protected:

private:
  ToolHandle<IDetectorGeometryTool> m_detTool{this, "World", "", "Tool handle of the top-of-the-tree detector geometry tool"};
  ToolHandle<IDetectorConstructionTool> m_detConstruction{this, "DetectorConstruction", "", "Tool handle of the DetectorConstruction"};
  ToolHandleArray<IRegionCreator> m_regionCreators;
  ToolHandleArray<IParallelWorldTool> m_parallelWorlds;
  ToolHandleArray<IG4GeometryConfigurationTool> m_configurationTools;
  ToolHandleArray<IFieldManagerTool> m_fieldManagers;
  Gaudi::Property<bool> m_activateParallelWorlds{this, "ActivateParallelWorlds", false, "Toggle on/off the G4 parallel geometry system"};
  std::vector<std::string> m_parallelWorldNames{};
};

#endif
