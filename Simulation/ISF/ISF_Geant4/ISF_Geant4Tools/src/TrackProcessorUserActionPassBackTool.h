/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4TOOLS_G4UA__TRACKPROCESSORUSERACTIONPASSBACKTOOL_H 
#define ISF_GEANT4TOOLS_G4UA__TRACKPROCESSORUSERACTIONPASSBACKTOOL_H 
#include "G4AtlasInterfaces/IBeginRunActionTool.h"
#include "G4AtlasInterfaces/IPreTrackingActionTool.h"
#include "G4AtlasInterfaces/ISteppingActionTool.h"
#include "G4AtlasInterfaces/IBeginEventActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "TrackProcessorUserActionPassBack.h"

namespace G4UA{ 

  namespace iGeant4{
    
    /// @class TrackProcessorUserActionPassBackTool
    /// @brief Tool which manages the TrackProcessorUserActionPassBack
    ///
    /// @author Andrea Di Simone
    ///

    class TrackProcessorUserActionPassBackTool: 
    public ActionToolBase<TrackProcessorUserActionPassBack>,
      public IPreTrackingActionTool,  public ISteppingActionTool,  public IBeginEventActionTool,
      public IBeginRunActionTool
      {
	
      public:
	/// Standard constructor
	TrackProcessorUserActionPassBackTool(const std::string& type, const std::string& name,const IInterface* parent);
	/// Retrieve the BoR action
	virtual IBeginRunAction* getBeginRunAction() override final 
	{ return static_cast<IBeginRunAction*>( getAction() ); }
	/// retrieve preTracking action
	virtual IPreTrackingAction* getPreTrackingAction() override final
	{ return static_cast<IPreTrackingAction*>( getAction() ); }
	/// retrieve stepping action
	virtual ISteppingAction* getSteppingAction() override final
	{ return static_cast<ISteppingAction*>( getAction() ); }
	/// retrieve BoE action
	virtual IBeginEventAction* getBeginEventAction() override final
	{ return static_cast<IBeginEventAction*>( getAction() ); }
	
	/// Query interface for gaudi
	virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
      protected:
	/// Create an action for this thread
	virtual std::unique_ptr<TrackProcessorUserActionPassBack> makeAction() override final;
      private:
	/// Configuration parameters
	TrackProcessorUserActionPassBack::Config m_config;
      }; // class TrackProcessorUserActionPassBackTool
    
  }  // iGeant4
  
} // namespace G4UA 
#endif
