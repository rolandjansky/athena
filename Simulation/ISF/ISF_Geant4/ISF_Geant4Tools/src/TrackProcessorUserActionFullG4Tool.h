/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4TOOLS_G4UA__TRACKPROCESSORUSERACTIONFULLG4TOOL_H
#define ISF_GEANT4TOOLS_G4UA__TRACKPROCESSORUSERACTIONFULLG4TOOL_H
#include "G4AtlasInterfaces/IG4TrackingActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "TrackProcessorUserActionFullG4.h"

namespace G4UA{

  namespace iGeant4{

    /// @class TrackProcessorUserActionFullG4Tool
    /// @brief Tool which manages the TrackProcessorUserActionFullG4
    ///
    /// @author Andrea Di Simone
    ///


    class TrackProcessorUserActionFullG4Tool:
    public ActionToolBase<TrackProcessorUserActionFullG4>,
      public IG4TrackingActionTool, public IG4SteppingActionTool, public IG4EventActionTool
    {

    public:
      /// Standard constructor
      TrackProcessorUserActionFullG4Tool(const std::string& type, const std::string& name,const IInterface* parent);

      /// Retrieve the tracking action
      virtual G4UserTrackingAction* getTrackingAction() override final
      { return static_cast<G4UserTrackingAction*>( getAction() ); }
      /// Retrieve the stepping action
      virtual G4UserSteppingAction* getSteppingAction() override final
      { return static_cast<G4UserSteppingAction*>( getAction() ); }
      /// Retrieve the event action
      virtual G4UserEventAction* getEventAction() override final
      { return static_cast<G4UserEventAction*>( getAction() ); }

      /// Query interface for gaudi
      virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
    protected:
      /// Create an action for this thread
      virtual std::unique_ptr<TrackProcessorUserActionFullG4> makeAction() override final;
    private:
      /// Configuration parameters
      TrackProcessorUserActionFullG4::Config m_config;
    }; // class TrackProcessorUserActionFullG4Tool

  }// iGeant4

} // namespace G4UA
#endif
