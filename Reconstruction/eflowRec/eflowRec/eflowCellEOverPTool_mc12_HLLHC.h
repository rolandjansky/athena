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
  StatusCode execute(eflowEEtaBinnedParameters *binnedParameters) ;
  StatusCode finalize() ;

 private:

  enum E_BINS        { ENERGY_BIN_START = 0, E001bin = ENERGY_BIN_START, E003point5bin = 1, E010bin = 2, E020bin = 3, E032point5bin = 4, E040bin = 5, ENERGY_BIN_END = 6 };
    
  enum ETA_BINS      { ETA_BIN_START = 0, eta050bin = ETA_BIN_START, eta100bin = 1, eta150bin = 2, eta250bin = 3, eta350bin = 4, eta450bin = 5, ETA_BIN_END = 6};

  enum SHAPE_PARAMS  { SHAPE_START = 0, NORM1 = SHAPE_START, WIDTH1 = 1, NORM2 = 2, WIDTH2 = 3, SHAPE_END = 4 };

  std::vector<double>  m_eBinValues;
  std::vector<double> m_etaBinBounds;
  std::vector<std::string> m_eBinLabels;
  std::vector<std::string> m_etaBinLabels;

};
#endif
