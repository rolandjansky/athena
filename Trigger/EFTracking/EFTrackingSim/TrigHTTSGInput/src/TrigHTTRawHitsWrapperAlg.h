/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigHTTSGInput_TrigHTTRawHistWrapperAlg_h
#define TrigHTTSGInput_TrigHTTRawHistWrapperAlg_h


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigHTTSGInput/ITrigHTTInputTool.h"

class TFile;
class TTree;
class TH2F;
class HTTEventInputHeader;
/**
 * @brief Steering algorithm to run ITrigHTTInputTool and save the output in plain ROOT tree
 * 
 */
class TrigHTTRawHitsWrapperAlg : public AthAlgorithm {
public:
  TrigHTTRawHitsWrapperAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TrigHTTRawHitsWrapperAlg() {};
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;


private:
  // configuration parameters  
  ToolHandle<ITrigHTTInputTool> m_hitInputTool { this, "InputTool", "TrigHTTSGToRawHitsTool/TrigHTTInputTool", "HitInput Tool" };
  Gaudi::Property<std::string> m_outpath { this, "OutFileName", "httsim_smartwrapper.root", "output path" };

  // internal pointers
  HTTEventInputHeader* m_eventHeader = nullptr;
  TFile* m_outfile = nullptr;
  TTree* m_EventTree = nullptr;
};

#endif
