/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWCELLEOVERPTOOL_RUN2_MC16_JETETMISS_H
#define EFLOWCELLEOVERPTOOL_RUN2_MC16_JETETMISS_H

#include "eflowRec/IEFlowCellEOverPTool.h"

class eflowBaseParameters;

/**
Class to store reference e/p mean and widths, as well as reference energy density radial profile fit parameters. The data is input to an eflowEEtaBinnedParameters object in the execute method. Stores data at the uncalibrated (EM) scale and is used by default. This inherits from IEFlowCellEOverPTool.
These were derived from the mc16 CP reprocessing round 1 samples.
*/
class eflowCellEOverPTool_Run2_mc16_JetETMiss : public IEFlowCellEOverPTool {

 public:

  eflowCellEOverPTool_Run2_mc16_JetETMiss(const std::string& type,const std::string& name,const IInterface* parent);

  ~eflowCellEOverPTool_Run2_mc16_JetETMiss() {};

  StatusCode initialize();
  StatusCode fillBinnedParameters(eflowEEtaBinnedParameters *binnedParameters) const;
  StatusCode finalize() ;

 private:

  std::vector<double>  m_eBinValues;
  std::vector<double> m_etaBinBounds;
  std::vector<std::string> m_eBinLabels;
  std::vector<std::string> m_etaBinLabels;

};
#endif
