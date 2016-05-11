/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4AtlasControl_SimControl_H
#define G4AtlasControl_SimControl_H

#include <string>
#include "G4AtlasControl/MCTruthMenu.h"

/// @class SimControl
/// @brief C++ class used for passing configuration
/// from the python layer to C++ layer.
///
/// This code was originally written to allow configuration of G4 from
/// the python layer prior to the introduction of configurable
/// properties in Athena classes. Once the FADS migration
/// (ATLASSIM-2256) is complete all of this code should be obsolete.
class SimControl
{
public:
  /// Empty Constructor
  SimControl();
  /// Empty Destructor
  virtual ~SimControl();
  /// Pass a command to the G4UIManager from the python layer
  void G4Command(const std::string&) const;
  /// Start a UIsession. FIXME not used anywhere - already obsolete? ATLASSIM-2766
  void UIsession() const;
  /// Used to load libraries for G4Extentions using FadsPackageLoader. FIXME Drop this. ATLASSIM-2768
  void load(const std::string&) const;
  /// Used to read XML configuration by FadsXMLParser. FIXME Migrate remaining clients so this can be dropped. ATLASSIM-2769
  void ReadXML(const std::string&) const;
  /// Access the MCTruthMenu. FIXME Migrate TruthStrategy configuration. ATLASSIM-2767
  const MCTruthMenu& mcMenu() const ;
  /// Still used from PyG4Atlas.G4AtlasEngine to initialize Geant4.
  void initializeG4(bool isMT=false) const;
  /// Still used from PyG4Atlas.menu_Visualization used by PyG4Atlas.G4AtlasEngine._init_Graphics. FIXME Drop this - obsolete.
  void initializeGraphics() const;
  /// Currently used to configure TruthStrategies for AtlasG4 jobs. FIXME Migrate TruthStrategy configuration. ATLASSIM-2767
  MCTruthMenu          mctruthMenu;
};

#endif
