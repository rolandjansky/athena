/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKG4USERACTIONS_G4UA__ENERGYLOSSRECORDERTOOL_H 
#define TRKG4USERACTIONS_G4UA__ENERGYLOSSRECORDERTOOL_H 
#include "G4AtlasInterfaces/IG4RunActionTool.h"
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "TrkG4UserActions/EnergyLossRecorder.h"
namespace Trk {
  class IPositionMomentumWriter;
}

namespace G4UA{ 


  /// @class EnergyLossRecorderTool
  /// @brief A class to manage EnergyLossRecorder actions
  ///
  /// creates one instance of the action per thread
  ///
  /// @author Andrea Di Simone

  class EnergyLossRecorderTool: 

  public ActionToolBase<EnergyLossRecorder>,
    public IG4RunActionTool, public IG4EventActionTool,  public IG4SteppingActionTool
  {
    
  public:
    /// standard tool ctor
    EnergyLossRecorderTool(const std::string& type, const std::string& name,const IInterface* parent);
    /// Athena initialize method
    virtual StatusCode initialize() override final;
    /// gets the run action
    virtual G4UserRunAction* getRunAction() override final 
    { return static_cast<G4UserRunAction*>( getAction() ); }
    /// gets the event action
    virtual G4UserEventAction* getEventAction() override final 
    { return static_cast<G4UserEventAction*>( getAction() ); }
    /// gets the stepping action
    virtual G4UserSteppingAction* getSteppingAction() override final 
    { return static_cast<G4UserSteppingAction*>( getAction() ); }
    /// Gaudi interface manipulation
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
  protected:
    /// creates the instance for this thread
    virtual std::unique_ptr<EnergyLossRecorder> makeAction() override final;
  private:
    /// stores the config obtained from the python side
    EnergyLossRecorder::Config m_config;
    ToolHandle<Trk::IPositionMomentumWriter> m_pmWriter;
  }; // class EnergyLossRecorderTool


} // namespace G4UA 
#endif
