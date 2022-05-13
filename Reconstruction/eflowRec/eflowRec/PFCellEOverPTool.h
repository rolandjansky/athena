/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFCELLEOVERPTOOL_H
#define PFCELLEOVERPTOOL_H

#include "eflowRec/IEFlowCellEOverPTool.h"

class eflowBaseParameters;

/**
Class to store reference e/p mean and widths, as well as reference energy density radial profile fit parameters. The data is input to an eflowEEtaBinnedParameters object in the execute method. Stores data at the uncalibrated (EM) scale and is used by default. This inherits from IEFlowCellEOverPTool.
*/
class PFCellEOverPTool : public IEFlowCellEOverPTool {

 public:

  PFCellEOverPTool(const std::string& type,const std::string& name,const IInterface* parent);

  ~PFCellEOverPTool() {};

  StatusCode initialize();
  StatusCode fillBinnedParameters(eflowEEtaBinnedParameters *binnedParameters) const;
  StatusCode finalize() ;

 private:

  std::vector<double>  m_eBinValues;
  std::vector<double> m_etaBinBounds;
  std::vector<std::string> m_eBinLabels;
  std::vector<std::string> m_etaBinLabels;

  std::string_view eBinsToString(const E_BINS& e_bins) const;
  std::string_view etaBinsToString(const ETA_BINS& eta_bins) const;

  const std::array<E_BINS,6>   m_e_bins{E001bin,E003point5bin,E010bin,E020bin,E032point5bin,E040bin};
  const std::array<ETA_BINS,4> m_eta_bins{eta050bin,eta100bin,eta150bin,eta250bin};
  const std::array<eflowFirstIntRegions::J1STLAYER,8> m_firstInt_bins{eflowFirstIntRegions::EMB1,eflowFirstIntRegions::EMB2,eflowFirstIntRegions::EMB3,eflowFirstIntRegions::EME1,eflowFirstIntRegions::EME2,
  eflowFirstIntRegions::EME3,eflowFirstIntRegions::HEC,eflowFirstIntRegions::Tile};
  const std::array<eflowCalo::LAYER,13> m_layerBins{eflowCalo::EMB1,eflowCalo::EMB2,eflowCalo::EMB3,eflowCalo::EME1,eflowCalo::EME2,eflowCalo::EME3,eflowCalo::HEC1,eflowCalo::HEC2,eflowCalo::HEC3
  ,eflowCalo::HEC4,eflowCalo::Tile1,eflowCalo::Tile2,eflowCalo::Tile3};

  /** Location for e/p and cell ordering reference files */
  Gaudi::Property<std::string> m_referenceFileLocation{this,"referenceFileLocation","/eos/user/m/mhodgkin/EoverP_Results/Run3_mc21_rel22/","Location for e/p and cell ordering reference files"};
};
#endif
