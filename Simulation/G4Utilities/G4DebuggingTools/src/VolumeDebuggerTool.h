/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4UA__VOLUMEDEBUGGERTOOL_H
#define G4DEBUGGINGTOOLS_G4UA__VOLUMEDEBUGGERTOOL_H

#include "G4AtlasTools/UserActionToolBase.h"
#include "./VolumeDebugger.h"

namespace G4UA
{

  /// @class VolumeDebuggerTool
  /// @brief Tool which manages the VolumeDebugger action.
  ///
  /// Create the VolumeDebugger  for each worker thread
  ///
  /// @author Andrea Di Simone
  ///
  class VolumeDebuggerTool : public UserActionToolBase<VolumeDebugger>
  {
    public:
      /// standard tool constructor
      VolumeDebuggerTool(const std::string& type, const std::string& name,const IInterface* parent);
    protected:
      /// builds the action for a thread
      virtual std::unique_ptr<VolumeDebugger>
      makeAndFillAction(G4AtlasUserActions&) override final;
    private:
      /// holds the configuration received from the python side
      VolumeDebugger::Config m_config;
    }; // class VolumeDebuggerTool

} // namespace G4UA

#endif
