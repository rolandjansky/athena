/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef HTT_DUMPOUTPUTSTATALG_H
#define HTT_DUMPOUTPUTSTATALG_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigHTTInput/IHTTEventOutputHeaderTool.h"


class TH2F;

class HTTDumpOutputStatAlg : public AthAlgorithm {
public:
  HTTDumpOutputStatAlg (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~HTTDumpOutputStatAlg () {};
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;  
  StatusCode BookHistograms();


private:
  ToolHandle<IHTTEventOutputHeaderTool>    m_readOutputTool  { this, "InputTool",  "HTTOutputHeaderTool/ReadOutputHeaderTool", "Input Tool" };
  ToolHandle<IHTTEventOutputHeaderTool>    m_writeOutputTool { this, "OutputTool", "HTTOutputHeaderTool/WriteOutputHeaderTool", "Output Tool" };
  
  // histograms
  TH2F*   m_hits_r_vs_z = nullptr;
};

#endif // HTT_DUMPOUTPUTSTATALG_H
