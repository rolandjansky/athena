/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKG4USERACTIONS_G4UA__ENERGYLOSSRECORDERTOOL_H 
#define TRKG4USERACTIONS_G4UA__ENERGYLOSSRECORDERTOOL_H 
#include "G4AtlasInterfaces/IBeginRunActionTool.h"
#include "G4AtlasInterfaces/IEndRunActionTool.h"
#include "G4AtlasInterfaces/IBeginEventActionTool.h"
#include "G4AtlasInterfaces/IEndEventActionTool.h"
#include "G4AtlasInterfaces/ISteppingActionTool.h"
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
    public IBeginRunActionTool,  public IEndRunActionTool,  public IBeginEventActionTool,  public IEndEventActionTool,  public ISteppingActionTool
  {
    
  public:
    /// standard tool ctor
    EnergyLossRecorderTool(const std::string& type, const std::string& name,const IInterface* parent);
    /// Athena initialize method
    virtual StatusCode initialize() override final;
    /// gets the BoR action
    virtual IBeginRunAction* getBeginRunAction() override final 
    { return static_cast<IBeginRunAction*>( getAction() ); }
    /// gets the EoR action
    virtual IEndRunAction* getEndRunAction() override final 
    { return static_cast<IEndRunAction*>( getAction() ); }
    /// gets the BoE action
    virtual IBeginEventAction* getBeginEventAction() override final 
    { return static_cast<IBeginEventAction*>( getAction() ); }
    /// gets the EoE action
    virtual IEndEventAction* getEndEventAction() override final 
    { return static_cast<IEndEventAction*>( getAction() ); }
    /// gets the stepping action
    virtual ISteppingAction* getSteppingAction() override final 
    { return static_cast<ISteppingAction*>( getAction() ); }
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
