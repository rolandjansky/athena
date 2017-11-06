/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4UA__VERBOSESELECTORTOOL_H 
#define G4DEBUGGINGTOOLS_G4UA__VERBOSESELECTORTOOL_H 

#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasInterfaces/IG4TrackingActionTool.h"
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "VerboseSelector.h"

namespace G4UA{ 

  class VerboseSelectorTool : public ActionToolBase<VerboseSelector>,
                              public IG4EventActionTool,
                              public IG4SteppingActionTool,
                              public IG4TrackingActionTool
  {
    
  public:
    VerboseSelectorTool(const std::string& type, const std::string& name,const IInterface* parent);
    virtual G4UserSteppingAction* getSteppingAction() override final
    { return static_cast<G4UserSteppingAction*>( getAction() ); }
    virtual G4UserTrackingAction* getTrackingAction() override final
    { return static_cast<G4UserTrackingAction*>( getAction() ); }
    virtual G4UserEventAction* getEventAction() override final
    { return static_cast<G4UserEventAction*>( getAction() ); }

    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
  protected:
    virtual std::unique_ptr<VerboseSelector> makeAction() override final;
  private:
    VerboseSelector::Config m_config;
  }; // class VerboseSelectorTool
  
  
} // namespace G4UA 
#endif
