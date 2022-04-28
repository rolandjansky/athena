/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef HTT_RAWHITSWRAPPERALG_H
#define HTT_RAWHITSWRAPPERALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigHTTInput/IHTTEventInputHeaderTool.h"

class HTTRawHitsWrapperAlg : public AthAlgorithm {
public:
  HTTRawHitsWrapperAlg (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~HTTRawHitsWrapperAlg () = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute()    override;
  virtual StatusCode finalize()   override;
  StatusCode BookHistograms();


private:
  // configuration parameters  
  //TODO: use input from SG: HTTSGToRawHitsTool/IHTTInputTool
  ToolHandle<IHTTEventInputHeaderTool>    m_readOutputTool  { this, "InputTool",  "HTTInputHeaderTool/ReadInputHeaderTool", "Input Tool" };
  ToolHandle<IHTTEventInputHeaderTool>    m_writeOutputTool { this, "OutputTool", "HTTInputHeaderTool/WriteInputHeaderTool", "Output Tool" };
 
  // some debug counters
  unsigned int m_tot_hits=0;
  unsigned int m_tot_truth=0;
  unsigned int m_tot_oftracks=0;
};

#endif // HTTRAWHITSWRAPPERALG_h
