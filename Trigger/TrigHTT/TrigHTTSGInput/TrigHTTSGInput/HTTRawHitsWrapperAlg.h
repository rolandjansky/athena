/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTT_RAWHITSWRAPPERALG_H
#define HTT_RAWHITSWRAPPERALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigHTTSGInput/HTT_IInputTool.h"

class TFile;
class TTree;
class TH2F;
class HTTEventInputHeader;

class HTTRawHitsWrapperAlg : public AthAlgorithm {
public:
  HTTRawHitsWrapperAlg (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~HTTRawHitsWrapperAlg () {};
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;


private:
  // configuration parameters  
  ToolHandle<HTT_IInputTool>    m_hitInputTool;// { this, "HitInputTool", "HTT_SGHitInput/HTT_SGHitInput", "HitInput Tool" };
  StringProperty m_outpath ;//    {this, "OutFileName", "httsim_smartwrapper.root", "output path"};

  // internal pointers
  HTTEventInputHeader* m_eventHeader;
  TFile *m_outfile;
  TTree *m_EventTree;
};

#endif // HTTRAWHITSWRAPPERALG_h
