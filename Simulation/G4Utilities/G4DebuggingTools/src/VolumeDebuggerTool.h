/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4UA__VOLUMEDEBUGGERTOOL_H 
#define G4DEBUGGINGTOOLS_G4UA__VOLUMEDEBUGGERTOOL_H 
#include "G4AtlasInterfaces/IG4RunActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "./VolumeDebugger.h"

namespace G4UA{ 

  /// @class VolumeDebuggerTool
  /// @brief Tool which manages the VolumeDebugger action.
  ///
  /// Create the VolumeDebugger  for each worker thread
  ///
  /// @author Andrea Di Simone
  ///
  
  class VolumeDebuggerTool: 
  public ActionToolBase<VolumeDebugger>,
    public IG4RunActionTool
    {
      
    public:
      /// standard tool constructor
      VolumeDebuggerTool(const std::string& type, const std::string& name,const IInterface* parent);
      /// returns the BoR action 
      virtual G4UserRunAction* getRunAction() override final
      { return static_cast<G4UserRunAction*>( getAction() ); }
      virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
    protected:
      /// builds the action for a thread
      virtual std::unique_ptr<VolumeDebugger> makeAction() override final;
    private:
      /// holds the configuration received from the python side
      VolumeDebugger::Config m_config;
    }; // class VolumeDebuggerTool
  
  
} // namespace G4UA 
#endif
