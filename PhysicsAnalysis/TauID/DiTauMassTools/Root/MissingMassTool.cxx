/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
// Local Includes
#include "DiTauMassTools/MissingMassTool.h"
#include "xAODTau/TauJet.h"

const double GEV = 1000.;

// Default constructor
MissingMassTool::MissingMassTool(const std::string& name) : asg::AsgTool(name)

{
  declareProperty("Decorate",			m_decorate=false, "Activate EventInfo decoration");
  declareProperty("CalibSet",			m_calib_set="2016MC15C", "Calibration: 2016MC15C/2015/2012/2011");
  // default negative. Only set parameter if positive
  // so that the default are in MissingMassCalculator code
  declareProperty("alg_version",		m_alg_version=-1, "Algorithm Version");
  declareProperty("NsigmaMET",			m_n_sigma_met=-1);
  declareProperty("UseTailCleanup",		m_tail_cleanup=-1); 
  declareProperty("UseVerbose",			m_use_verbose=-1);
  declareProperty("NiterFit2",			m_niter_fit_2=-1);
  declareProperty("NiterFit3",			m_niter_fit_3=-1);
  // Property not there in latest MMC tags
  declareProperty("UseTauProbability",		m_use_tau_probability=-1);
  declareProperty("UseDefaults",		m_use_defaults=-1);
  declareProperty("UseEfficiencyRecovery",	m_use_efficiency_recovery=-1);
  declareProperty("UseMETDphiLL",m_use_met_param_dphiLL = false);

  // Should not be needed after autodetection; can be removed later. --GJ 31/5/16
  //declareProperty("LFVmode", m_lfv_mode="etau", "LFV mode: etau/mutau");

}

// Copy constructor
MissingMassTool::MissingMassTool(const MissingMassTool& other): asg::AsgTool(other.name() + "_copy") {}


StatusCode MissingMassTool::initialize()

{
  ATH_MSG_INFO("Initialize MissingMassTool");

  m_MMC = new MissingMassCalculator();
  // set properties if non negative
  if (m_alg_version>=0) m_MMC->SetAlgorithmVersion(m_alg_version);
  if (m_n_sigma_met>=0) m_MMC->SetNsigmaMETscan(m_n_sigma_met);
  if (m_tail_cleanup>=0) m_MMC->SetUseTailCleanup(m_tail_cleanup);
  if (m_use_verbose>=0) m_MMC->SetUseVerbose(m_use_verbose);
  if (m_niter_fit_2>=0) m_MMC->SetNiterFit2(m_niter_fit_2);
  if (m_niter_fit_3>=0) m_MMC->SetNiterFit3(m_niter_fit_3);
  if (m_use_tau_probability>=0) m_MMC->SetUseTauProbability(m_use_tau_probability);
  if (m_use_defaults>=0) m_MMC->SetUseDefaults(m_use_defaults);
  if (m_use_efficiency_recovery>=0) m_MMC->SetUseEfficiencyRecovery(m_use_efficiency_recovery);
  if (m_use_met_param_dphiLL) m_MMC->SetUseDphiLL(m_use_met_param_dphiLL);

  // could be made a property but maybe not with the enum
  // What about a string argument ?


  if (m_calib_set == "2015") {
    m_MMC->SetCalibrationSet(MMCCalibrationSet::MMC2015);
  } else if (m_calib_set == "2015HIGHMASS") {
    m_MMC->SetCalibrationSet(MMCCalibrationSet::MMC2015HIGHMASS);
  } else if (m_calib_set == "2012") {
    m_MMC->SetCalibrationSet(MMCCalibrationSet::MMC2012);
  } else if (m_calib_set == "2011") {
    m_MMC->SetCalibrationSet(MMCCalibrationSet::MMC2011);
  } else if (m_calib_set == "UPGRADE") {
    m_MMC->SetCalibrationSet(MMCCalibrationSet::UPGRADE);
  } else if (m_calib_set == "LFV") {
    m_MMC->SetCalibrationSet(MMCCalibrationSet::LFVMMC2012);
  } else if (m_calib_set == "2016MC15C") {
    m_MMC->SetCalibrationSet(MMCCalibrationSet::MMC2016MC15C);
  }else {
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode MissingMassTool::finalize()

{
  ATH_MSG_INFO("Finalize MissingMassTool");
  delete m_MMC;

  return StatusCode::SUCCESS;
}


// generic method
CP::CorrectionCode MissingMassTool::apply(const xAOD::EventInfo& ei,
    const xAOD::IParticle* part1,
    const xAOD::IParticle* part2,
    const xAOD::MissingET* met,
    const int & njets)
{

  int mmcType1 = mmcType(part1);
  if (mmcType1 < 0) return CP::CorrectionCode::Error;

  int mmcType2 = mmcType(part2);
  if (mmcType2 < 0) return CP::CorrectionCode::Error;

  if(setLFVMode(part1, part2, mmcType1, mmcType2) == CP::CorrectionCode::Error) {
    return CP::CorrectionCode::Error;
  }

  // this will be in MeV but MMC allows MeV
  // assume the mass is correct as well
  TLorentzVector tlvTau1=part1->p4(); 
  TLorentzVector tlvTau2=part2->p4();


  //---- buggy version, no MET passed, need to dereference MET
  //   calculate(ei, tlvTau1, tlvTau2,
  // 	    mmcType1, mmcType2,
  // 	    met, njets);
  calculate(ei, tlvTau1, tlvTau2,
      mmcType1, mmcType2,
      *met, njets);

  return CP::CorrectionCode::Ok;
}


// Internal calculation -- called by the apply method
void MissingMassTool::calculate(const xAOD::EventInfo & ei, 
    const TLorentzVector & vis_tau1,
    const TLorentzVector & vis_tau2,
    const int & tau1_decay_type,
    const int & tau2_decay_type,
    const xAOD::MissingET & met,
    const int & njets)
{

  //Convert to GeV. In principle, MMC should cope with MeV but should check thoroughly
  TLorentzVector fixedtau1;
  fixedtau1.SetPtEtaPhiM(vis_tau1.Pt()/GEV,vis_tau1.Eta(),vis_tau1.Phi(),vis_tau1.M()/GEV);
  TLorentzVector fixedtau2;
  fixedtau2.SetPtEtaPhiM(vis_tau2.Pt()/GEV,vis_tau2.Eta(),vis_tau2.Phi(),vis_tau2.M()/GEV);

  m_MMC->SetVisTauType(0, tau1_decay_type);
  m_MMC->SetVisTauType(1, tau2_decay_type);
  m_MMC->SetVisTauVec(0, fixedtau1);
  m_MMC->SetVisTauVec(1, fixedtau2);

  ATH_MSG_DEBUG("running for tau types " << tau1_decay_type << " " << tau2_decay_type);
  TVector2 met_vec(met.mpx()/GEV , met.mpy()/GEV); 
  m_MMC->SetMetVec(met_vec);
  ATH_MSG_DEBUG("passing SumEt=" << met.sumet()/GEV);
  m_MMC->SetSumEt(met.sumet()/GEV);
  m_MMC->SetNjet25(njets);

  // This is actually where the work is done
  m_MMC->RunMissingMassCalculator();

  // Very dry decoration - MET and resonance vectors are retrieved
  // in dedicated method (see MissingMassTool.h)

  if (m_decorate) {
    int aFitStatus = m_MMC->GetFitStatus();
    ei.auxdecor<int>("mmc_fit_status") = aFitStatus;
    ei.auxdecor<double>("mmc_maxw_mass")  = aFitStatus==1 ? m_MMC->GetFittedMass(MMCFitMethod::MAXW) : -1;
    ei.auxdecor<double>("mmc_mlm_mass")   = aFitStatus==1 ? m_MMC->GetFittedMass(MMCFitMethod::MLM) : -1;
    ei.auxdecor<double>("mmc_mlnu3p_mass") = aFitStatus==1 ? m_MMC->GetFittedMass(MMCFitMethod::MLNU3P) : -1;
    TLorentzVector null4V(-1,-1,-1,-1);
    ei.auxdecor<TLorentzVector>("mmc_mlnu3p_4vect") = aFitStatus==1 ? m_MMC->GetResonanceVec(MMCFitMethod::MLNU3P) : null4V;
  }

}


int MissingMassTool::mmcType(const xAOD::IParticle* part) const
{
  if(part == nullptr) {
    ATH_MSG_DEBUG("MissingMassTool::mmcType() got a nullptr - returning -1");
    return -1;
  }

  xAOD::Type::ObjectType partType=part->type();
  int aType=-1;
  if (partType==xAOD::Type::Electron || partType==xAOD::Type::Muon) 
  {
    aType=0;
  }
  else if (partType==xAOD::Type::Tau)
  {
    const xAOD::TauJet * aTauJet=dynamic_cast<const xAOD::TauJet*>(part);
    if (aTauJet==0)
    {
      ATH_MSG_WARNING("MissingMassTool::mmcType() dynamic_cast of TauJet failed");
      aType=-2;
    }
    if (aTauJet->nTracks() <= 1) {
      aType=10;
    } else {
      aType=30;
    }
  } 
  else
  {
    ATH_MSG_WARNING("MissingMassTool::mmcType() unrecognised particle type! Only Electron, Muon, TauJet allowed. If no mistake, please call MissingMassTool::calculate() directly.");
    aType=-1;
  }

  return aType;
}

CP::CorrectionCode MissingMassTool::setLFVMode(const xAOD::IParticle* p1, const xAOD::IParticle* p2, int mmcType1, int mmcType2) {
  // In case we're in LFV calibration, pass the LFV mode to the tool
  if (m_calib_set != "LFV") {
    return CP::CorrectionCode::Ok;
  }

  if (mmcType1 == -1) mmcType1 = mmcType(p1);
  if (mmcType1 < 0) return CP::CorrectionCode::Error;

  if (mmcType2 == -1) mmcType1 = mmcType(p2);
  if (mmcType2 < 0) return CP::CorrectionCode::Error;
  
  // we don't use mmcType as it's 0 for both leptons
  const xAOD::IParticle *p;
  if(mmcType1 == 0 && mmcType2 == 0) {
    // both leptonic; find whichever has the highest pT
    if(p1->pt() > p2->pt() ) {
      p = p1;
    } else {
      p = p2;
    }

  } else {
    // one of them is a lepton, find it
    if(mmcType1 == 0) {
      p = p1; 
    } else if(mmcType2 == 0) {
      p = p2;
    } else {
      // if you're here, you've passed 2 taus to the LFV mode. 
      ATH_MSG_ERROR("Trying to set LFV mode for 2 taus!");
      return CP::CorrectionCode::Error;
    }
  }

  if(!p) return CP::CorrectionCode::Error;

  if(p->type() == xAOD::Type::Muon) {
    // mu+tau mode
    ATH_MSG_DEBUG("Setting MMC to mu+tau mode");
    m_MMC->SetLFVmode(1);
  } else {
    // e+tau mode
    ATH_MSG_DEBUG("Setting MMC to e+tau mode");
    m_MMC->SetLFVmode(0);
  }
  
  return CP::CorrectionCode::Ok;
}
