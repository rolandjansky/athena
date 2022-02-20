/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWCELLEOVERPTOOL_RUN2_MC20_JETETMISS_H
#define EFLOWCELLEOVERPTOOL_RUN2_MC20_JETETMISS_H

#include "eflowRec/IEFlowCellEOverPTool.h"

class eflowBaseParameters;

/**
Class to store reference e/p mean and widths, as well as reference energy density radial profile fit parameters. The data is input to an eflowEEtaBinnedParameters object in the execute method. Stores data at the uncalibrated (EM) scale and is used by default. This inherits from IEFlowCellEOverPTool.
These were derived from the mc20 CP reprocessing round 1 samples which consisted of these datasets:
mc16_13TeV.426327.ParticleGun_single_piminus_logE5to2000.recon.ESD.e5661_s3170_r12317_r12253_r12317
mc16_13TeV.426328.ParticleGun_single_piplus_logE5to2000.recon.ESD.e5661_s3170_r12317_r12253_r12317
mc16_13TeV.426329.ParticleGun_single_piminus_E2to5.recon.ESD.e5661_s3170_r12317_r12253_r12317
mc16_13TeV.426330.ParticleGun_single_piplus_E2to5.recon.ESD.e5661_s3170_r12317_r12253_r12317
mc16_13TeV.426331.ParticleGun_single_piminus_E0p4to2.recon.ESD.e5661_s3170_r12317_r12253_r12317
mc16_13TeV.426332.ParticleGun_single_piplus_E0p4to2.recon.ESD.e5661_s3170_r12317_r12253_r12317
The following software was used to produce AOD and ntuples from the above (we used an athena master nightly build from March 2021), and then to perform the derivaton of the parameters:
https://gitlab.cern.ch/atlas-jetetmiss/PFlow-EoverP-Ntuple-Code
*/
class eflowCellEOverPTool_Run2_mc20_JetETMiss : public IEFlowCellEOverPTool {

 public:

  eflowCellEOverPTool_Run2_mc20_JetETMiss(const std::string& type,const std::string& name,const IInterface* parent);

  ~eflowCellEOverPTool_Run2_mc20_JetETMiss() {};

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
