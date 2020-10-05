/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWCELLEOVERPTOOL_MC12_HLLHC_H
#define EFLOWCELLEOVERPTOOL_MC12_HLLHC_H

#include "eflowRec/IEFlowCellEOverPTool.h"

class eflowBaseParameters;

/**
Class to store reference e/p mean and widths, as well as reference energy density radial profile fit parameters. The data is input to an eflowEEtaBinnedParameters object in the execute method. Stores data at the uncalibrated (EM) scale and is for HLLHC setup. This inherits from IEFlowCellEOverPTool. The values are derived using single pions with the AMI tags e3895_s3020_r9063.
*/
class eflowCellEOverPTool_mc12_HLLHC : public IEFlowCellEOverPTool {

 public:

  eflowCellEOverPTool_mc12_HLLHC(const std::string& type,const std::string& name,const IInterface* parent);

  ~eflowCellEOverPTool_mc12_HLLHC() {};

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
