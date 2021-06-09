/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AtlasDetectorConstructionTool_H
#define G4AtlasDetectorConstructionTool_H

// Include files
// Gaudi headers
#include "GaudiKernel/ToolHandle.h" // For tool handle array
// Athena headers
#include "AthenaBaseComps/AthAlgTool.h"
#include "G4AtlasInterfaces/IDetectorConstructionTool.h"
#include "G4AtlasInterfaces/IDetectorGeometryTool.h"
#include "G4AtlasInterfaces/IRegionCreator.h"
#include "G4AtlasInterfaces/IParallelWorldTool.h"
#include "G4AtlasInterfaces/IFieldManagerTool.h"
#include "G4AtlasInterfaces/IG4GeometryConfigurationTool.h"
#include "G4AtlasInterfaces/ISensitiveDetectorMasterTool.h"
// Geant4 headers
#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"

/** @class G4AtlasDetectorConstructionTool G4AtlasDetectorConstructionTool.h "G4AtlasTools/G4AtlasDetectorConstructionTool.h"
 *
 *  Tool for the concrete implementation of a G4Atlas-based detector construction
 *
 *  @author Andrea Dell'Acqua
 *  @date   2015-03-06
 */
class G4AtlasDetectorConstructionTool final : public extends<AthAlgTool, IDetectorConstructionTool> {

 public:
  /// Standard constructor
  G4AtlasDetectorConstructionTool( const std::string& type , const std::string& name,
                                   const IInterface* parent ) ;

  /// Destructor
  ~G4AtlasDetectorConstructionTool( ) = default;

  /// Initialize method
  virtual StatusCode initialize( ) override final;

  /** implements the following IDetectorConstructionTool methods */
  virtual G4VUserDetectorConstruction* GetDetectorConstruction() override final {return this;}
  virtual std::vector<std::string>& GetParallelWorldNames() override final; // Called by DetectorGeometrySvc

  /** Implements the following G4VUserDetectorConstruction methods*/
  virtual G4VPhysicalVolume* Construct() override final;
  virtual void ConstructSDandField() override final;

 private:
  ToolHandle<IDetectorGeometryTool> m_detTool{this, "World", "", "Tool handle of the top-of-the-tree detector geometry tool"};
  ToolHandleArray<IRegionCreator> m_regionCreators{this, "RegionCreators", {}, "Tools to define G4 physics regions"};
  ToolHandleArray<IParallelWorldTool> m_parallelWorlds{this, "ParallelWorlds", {} , "Tools to define G4 parallel worlds"};
  ToolHandleArray<IG4GeometryConfigurationTool> m_configurationTools{this, "GeometryConfigurationTools", {}, "Tools for geometry configuration"};
  PublicToolHandle<ISensitiveDetectorMasterTool> m_senDetTool{this, "SenDetMasterTool", "SensitiveDetectorMasterTool", ""};
  ToolHandleArray<IFieldManagerTool> m_fieldManagers{this, "FieldManagers", {}, "field managers used"};
  Gaudi::Property<bool> m_activateParallelWorlds{this, "ActivateParallelWorlds", false, "Toggle on/off the G4 parallel geometry system"};
  std::vector<std::string> m_parallelWorldNames{};

};

#endif
