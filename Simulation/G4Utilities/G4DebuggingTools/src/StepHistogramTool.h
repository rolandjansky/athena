/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4UA__STEPHISTOGRAMTOOL_H 
#define G4DEBUGGINGTOOLS_G4UA__STEPHISTOGRAMTOOL_H 
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasInterfaces/IG4RunActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "StepHistogram.h"

class ITHistSvc;

namespace G4UA{ 
  
  
  /// @class StepHistogramTool
  /// @brief Tool which manages the StepHistogram action.
  ///
  /// Create the StepHistogram for each worker thread
  ///
  /// @author Miha Muskinja
  ///
  
  class StepHistogramTool: public ActionToolBaseReport<StepHistogram>,
                           public IG4EventActionTool,
                           public IG4RunActionTool,
                           public IG4SteppingActionTool
  {

  public:
    /// standard tool ctor
    StepHistogramTool(const std::string& type, const std::string& name,const IInterface* parent);
    /// return the event action
    virtual G4UserEventAction* getEventAction() override final
    { return static_cast<G4UserEventAction*>( getAction() ); }
    /// return the stepping action
    virtual G4UserSteppingAction* getSteppingAction() override final
    { return static_cast<G4UserSteppingAction*>( getAction() ); }
    /// return the run action
    virtual G4UserRunAction* getRunAction() override final
    { return static_cast<G4UserRunAction*>( getAction() ); }
    /// gaudi's interface handling
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;

    // initialize
    virtual StatusCode initialize() override;
    // finalize
    virtual StatusCode finalize() override;

  protected:
    /// Create action for this thread
    virtual std::unique_ptr<StepHistogram> makeAction() override final;

  private:
    StepHistogram::Config m_config;

    ServiceHandle<ITHistSvc> m_histSvc;

    void BookHistograms(StepHistogram::HistoMapMap_t &hMap, const char* suffix, const char* subfolder = "");
    
  }; // class StepHistogramTool
  
  
} // namespace G4UA 
#endif
