/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4TOOLS_G4UA__TRACKPROCESSORUSERACTIONPASSBACKTOOL_H
#define ISF_GEANT4TOOLS_G4UA__TRACKPROCESSORUSERACTIONPASSBACKTOOL_H
#include "G4AtlasInterfaces/IPreTrackingActionTool.h"
#include "G4AtlasInterfaces/IPostTrackingActionTool.h"
#include "G4AtlasInterfaces/ISteppingActionTool.h"
#include "G4AtlasInterfaces/IBeginEventActionTool.h"
#include "G4AtlasInterfaces/IEndEventActionTool.h"
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
      public IPreTrackingActionTool,  public IPostTrackingActionTool,  public ISteppingActionTool,  public IBeginEventActionTool,  public IEndEventActionTool
    {

    public:
      /// Standard constructor
      TrackProcessorUserActionPassBackTool(const std::string& type, const std::string& name,const IInterface* parent);

      /// Retrieve the preTracking action
      virtual IPreTrackingAction* getPreTrackingAction() override final
      { return static_cast<IPreTrackingAction*>( getAction() ); }
      /// Retrieve the postTracking action
      virtual IPostTrackingAction* getPostTrackingAction() override final
      { return static_cast<IPostTrackingAction*>( getAction() ); }
       /// Retrieve the stepping action
      virtual ISteppingAction* getSteppingAction() override final
      { return static_cast<ISteppingAction*>( getAction() ); }
       /// Retrieve the BoE action
      virtual IBeginEventAction* getBeginEventAction() override final
      { return static_cast<IBeginEventAction*>( getAction() ); }
       /// Retrieve the EoE action
      virtual IEndEventAction* getEndEventAction() override final
      { return static_cast<IEndEventAction*>( getAction() ); }

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
