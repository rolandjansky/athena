/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASSERVICES_SENSITIVEDETECTORMASTERTOOL_H
#define G4ATLASSERVICES_SENSITIVEDETECTORMASTERTOOL_H

// Base classes
#include "AthenaBaseComps/AthAlgTool.h"
#include "G4AtlasInterfaces/ISensitiveDetectorMasterTool.h"

// Member
#include "GaudiKernel/ToolHandle.h" // For tool handle array
#include "G4AtlasInterfaces/ISensitiveDetector.h" // What we have a handle on

/// @class SensitiveDetectorMasterTool
/// @brief Public tool which manages all the sensitive detector tools.
///
/// This tool provides functionality to trigger creation of SDs for the current
/// worker thread and to invoke special SD methods at the beginning and end of
/// an Athena event.
///
/// This will probably be changed (back) to a service in the near future.
///
class SensitiveDetectorMasterTool : public extends<AthAlgTool, ISensitiveDetectorMasterTool>
{
 public:
  /// Standard constructor
  SensitiveDetectorMasterTool(const std::string& type, const std::string& name,
                              const IInterface* parent);
  /// Empty virtual destructor
  virtual ~SensitiveDetectorMasterTool() {}

  /// Retrieve the SD tools. SD creation is deferred until initializeSDs.
  StatusCode initialize() override final;

  /// Calls initializeSD on each SD tool to create the SDs for the current worker thread.
  StatusCode initializeSDs() override final;
  /// Calls SetupEvent on each SD tool
  StatusCode BeginOfAthenaEvent() override final;
  /// Calls Gather on each SD tool
  StatusCode EndOfAthenaEvent() override final;

 private:
  /// Private array of tool handles pointing to all SD tools.
  ToolHandleArray<ISensitiveDetector> m_senDetTools;
};

#endif
