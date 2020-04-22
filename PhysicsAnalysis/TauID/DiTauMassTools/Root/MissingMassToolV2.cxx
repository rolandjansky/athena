/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
// Local include(s)
#include "DiTauMassTools/MissingMassToolV2.h"

// EDM include(s):
#include "xAODTau/TauJet.h"

using namespace DiTauMassTools;

// Default constructor
MissingMassToolV2::MissingMassToolV2(const std::string& name) : asg::AsgTool(name)

{
  declareProperty("Decorate",			m_decorate=false, "Activate EventInfo decoration");
  declareProperty("FloatStoppingCrit",		m_float_stop=true, "Activate floating stopping criterion");
  declareProperty("CalibSet",			m_calib_set="2019", "Calibration: 2016MC15C/2015/2012/2011");
  // default negative. Only set parameter if positive
  // so that the default are in MissingMassCalculator code
  declareProperty("NsigmaMET",			m_n_sigma_met=-1);
  declareProperty("UseTailCleanup",		m_tail_cleanup=-1); 
  declareProperty("UseVerbose",			m_use_verbose=-1);
  declareProperty("NiterFit2",			m_niter_fit_2=-1);
  declareProperty("NiterFit3",			m_niter_fit_3=-1);
  // Property not there in latest MMC tags
  declareProperty("UseTauProbability",		m_use_tau_probability=-1);
  declareProperty("UseMnuProbability",		m_use_mnu_probability=false);
  declareProperty("UseDefaults",		m_use_defaults=-1);
  declareProperty("UseEfficiencyRecovery",	m_use_efficiency_recovery=-1);
  declareProperty("UseMETDphiLL",		m_use_met_param_dphiLL = false);
  declareProperty("ParamFilePath",		m_param_file_path = "MMC_params_v1_fixed.root");
}

// Copy constructor
MissingMassToolV2::MissingMassToolV2(const MissingMassToolV2& other): asg::AsgTool(other.name() + "_copy") {}


StatusCode MissingMassToolV2::initialize()

{
  ATH_MSG_INFO("Initialize MissingMassTool");

  MMCCalibrationSetV2::e aset;

  if (m_calib_set == "2015") {
    aset = MMCCalibrationSetV2::MMC2015;
  } else if (m_calib_set == "2015HIGHMASS") {
    aset = MMCCalibrationSetV2::MMC2015HIGHMASS;
  } else if (m_calib_set == "2012") {
    aset = MMCCalibrationSetV2::MMC2012;
  } else if (m_calib_set == "2011") {
    aset = MMCCalibrationSetV2::MMC2011;
  } else if (m_calib_set == "UPGRADE") {
    aset = MMCCalibrationSetV2::UPGRADE;
  } else if (m_calib_set == "LFV") {
    aset = MMCCalibrationSetV2::LFVMMC2012;
  } else if (m_calib_set == "2016MC15C") {
    aset = MMCCalibrationSetV2::MMC2016MC15C;
  } else if (m_calib_set == "2019") {
    aset = MMCCalibrationSetV2::MMC2019;
  } else {
    return StatusCode::FAILURE;
  }
  m_MMC = new MissingMassCalculatorV2(aset, m_param_file_path);
  // set properties if non negative
  if (!m_float_stop) m_MMC->SetUseFloatStopping(false);
  if (m_n_sigma_met>=0) m_MMC->SetNsigmaMETscan(m_n_sigma_met);
  if (m_tail_cleanup>=0) m_MMC->preparedInput.SetUseTailCleanup(m_tail_cleanup);
  if (m_use_verbose>=0) m_MMC->preparedInput.SetUseVerbose(m_use_verbose);
  if (m_niter_fit_2>=0) m_MMC->SetNiterFit2(m_niter_fit_2);
  if (m_niter_fit_3>=0) m_MMC->SetNiterFit3(m_niter_fit_3);
  if (m_use_tau_probability>=0) m_MMC->Prob->SetUseTauProbability(m_use_tau_probability);
  if (m_use_defaults>=0) m_MMC->preparedInput.SetUseDefaults(m_use_defaults);
  if (m_use_efficiency_recovery>=0) m_MMC->SetUseEfficiencyRecovery(m_use_efficiency_recovery);
  if (m_use_met_param_dphiLL) m_MMC->Prob->SetUseDphiLL(m_use_met_param_dphiLL);
  if (m_use_mnu_probability) m_MMC->Prob->SetUseMnuProbability(m_use_mnu_probability);

  // could be made a property but maybe not with the enum
  // What about a string argument ?

  return StatusCode::SUCCESS;
}

StatusCode MissingMassToolV2::finalize()

{
  ATH_MSG_INFO("Finalize MissingMassTool");
  delete m_MMC;
  //delete Output;

  return StatusCode::SUCCESS;
}


// generic method
CP::CorrectionCode MissingMassToolV2::apply(const xAOD::EventInfo& ei,
    const xAOD::IParticle* part1,
    const xAOD::IParticle* part2,
    const xAOD::MissingET* met,
    const int & njets)
{
  // This is actually where the work is done
  m_MMC->SetEventNumber(ei.eventNumber());
  m_MMC->RunMissingMassCalculator(part1, part2, met, njets);

  // Very dry decoration - MET and resonance vectors are retrieved
  // in dedicated method (see MissingMassToolV2.h)

  if (m_decorate) {
    int aFitStatus = m_MMC->OutputInfo.GetFitStatus();
    ei.auxdecor<int>("mmc_fit_status") = aFitStatus;
    ei.auxdecor<double>("mmc_maxw_mass")  = aFitStatus==1 ? m_MMC->OutputInfo.GetFittedMass(MMCFitMethodV2::MAXW) : -1;
    ei.auxdecor<double>("mmc_mlm_mass")   = aFitStatus==1 ? m_MMC->OutputInfo.GetFittedMass(MMCFitMethodV2::MLM) : -1;
    ei.auxdecor<double>("mmc_mlnu3p_mass") = aFitStatus==1 ? m_MMC->OutputInfo.GetFittedMass(MMCFitMethodV2::MLNU3P) : -1;
    TLorentzVector null4V(-1,-1,-1,-1);
    ei.auxdecor<TLorentzVector>("mmc_mlnu3p_4vect") = aFitStatus==1 ? m_MMC->OutputInfo.GetResonanceVec(MMCFitMethodV2::MLNU3P) : null4V;
  }

  return CP::CorrectionCode::Ok;
}
