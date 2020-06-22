/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWCELLEOVERPTOOL_MC12_LC_H
#define EFLOWCELLEOVERPTOOL_MC12_LC_H

/********************************************************************

NAME:     eflowCellEOverPTool_mc12_LC.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  March, 2014

Description: E/P reference at LC scale

********************************************************************/

#include "eflowRec/IEFlowCellEOverPTool.h"

class eflowBaseParameters;

/**
 Class to store reference e/p mean and widths, as well as reference energy density radial profile fit parameters.  The data is input to an eflowEEtaBinnedParameters object in the execute method. Stores data at the Local Hadron Calibration (LC) scale. This inherits from IEFlowCellEOverPTool.
*/
class eflowCellEOverPTool_mc12_LC : public IEFlowCellEOverPTool {

 public:

  eflowCellEOverPTool_mc12_LC(const std::string& type,const std::string& name,const IInterface* parent);

  ~eflowCellEOverPTool_mc12_LC() {};

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
