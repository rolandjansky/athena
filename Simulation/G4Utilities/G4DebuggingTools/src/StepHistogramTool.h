/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4UA__STEPHISTOGRAMTOOL_H
#define G4DEBUGGINGTOOLS_G4UA__STEPHISTOGRAMTOOL_H

#include "G4AtlasTools/UserActionToolBase.h"
#include "StepHistogram.h"
#include "GaudiKernel/ITHistSvc.h"

namespace G4UA{ 
  
  
  /// @class StepHistogramTool
  /// @brief Tool which manages the StepHistogram action.
  ///
  /// Create the StepHistogram for each worker thread
  ///
  /// @author Miha Muskinja
  ///
  
  class StepHistogramTool: public UserActionToolBase<StepHistogram>
  {

  public:
    /// standard tool ctor
    StepHistogramTool(const std::string& type, const std::string& name,const IInterface* parent);
    /// return the event action

    // initialize
    virtual StatusCode initialize() override;
    // finalize
    virtual StatusCode finalize() override;

  protected:
    /// Create action for this thread
    virtual std::unique_ptr<StepHistogram>
    makeAndFillAction(G4AtlasUserActions&) override final;

  private:
    StepHistogram::Config m_config;

    ServiceHandle<ITHistSvc> m_histSvc;

    void BookHistograms(StepHistogram::HistoMapMap_t &hMap, const char* suffix, const char* subfolder = "");
    
  }; // class StepHistogramTool
  
  
} // namespace G4UA 
#endif
