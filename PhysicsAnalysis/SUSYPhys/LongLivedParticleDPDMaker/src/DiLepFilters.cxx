/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// DiLepFilters.cxx, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Dominik Krauss (krauss@mpp.mpg.de)

// STL
#include <cmath>

// Athena
#include "LongLivedParticleDPDMaker/DiLepFilters.h"
#include "xAODEgamma/ElectronxAODHelpers.h"

DerivationFramework::DiLepFilters::DiLepFilters(const std::string& t, const std::string& n, const IInterface* p)
                                               : AthAlgTool(t, n, p), m_tdt("Trig::TrigDecisionTool/TrigDecisionTool"),
                                                 m_pass_siph(false), m_pass_diph(false), m_pass_simu(false),
                                                 m_el_eta(0.), m_ph_eta(0.), m_mu_eta(0.), m_mu_beta(0.),
                                                 m_el_d0(0.), m_mu_d0(0.),
                                                 m_siel_pt(0.), m_siph_pt(0.), m_siph_xpt(0.), m_simu_pt(0.),
                                                 m_diel_pt(0.), m_diph_pt(0.), m_dielph_pt(0.), m_diloelph_pt(0.)
{
  declareInterface<IDiLepFilters>(this);

  declareProperty("TrigDecisionTool", m_tdt);

  declareProperty("SiPhTriggers", m_trig_siph);
  declareProperty("DiPhTriggers", m_trig_diph);
  declareProperty("SiMuTriggers", m_trig_simu);

  declareProperty("ElEtaMax", m_el_eta);
  declareProperty("PhEtaMax", m_ph_eta);
  declareProperty("MuEtaMax", m_mu_eta);
  declareProperty("MuBaEtaMax", m_mu_beta);

  declareProperty("ElD0Min", m_el_d0);
  declareProperty("MuD0Min", m_mu_d0);

  declareProperty("SiElPtMin", m_siel_pt);
  declareProperty("SiPhPtMin", m_siph_pt);
  declareProperty("SiPhXPtMin", m_siph_xpt);
  declareProperty("SiMuPtMin", m_simu_pt);
  declareProperty("DiElPtMin", m_diel_pt);
  declareProperty("DiPhPtMin", m_diph_pt);
  declareProperty("DiElPhPtMin", m_dielph_pt);
  declareProperty("DiLoElPhPtMin", m_diloelph_pt);
}

StatusCode DerivationFramework::DiLepFilters::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");

  CHECK(m_tdt.retrieve());

  return StatusCode::SUCCESS;
}

bool DerivationFramework::DiLepFilters::GetTriggers()
{
  m_pass_siph = false;
  m_pass_diph = false;
  m_pass_simu = false;

  for(const std::string& tn: m_trig_siph)
  {
    if(m_tdt->isPassed(tn))
    {
      m_pass_siph = true;
      break;
    }
  }
  for(const std::string& tn: m_trig_diph)
  {
    if(m_tdt->isPassed(tn))
    {
      m_pass_diph = true;
      break;
    }
  }
  for(const std::string& tn: m_trig_simu)
  {
    if(m_tdt->isPassed(tn))
    {
      m_pass_simu = true;
      break;
    }
  }

  return m_pass_siph || m_pass_diph || m_pass_simu;
}

bool DerivationFramework::DiLepFilters::PassSiEl(const xAOD::Electron& el) const
{
  if(!m_pass_siph) return false;

  return PassCuts(el, m_siel_pt);
}

bool DerivationFramework::DiLepFilters::PassSiPhX(const xAOD::Photon& ph, const xAOD::Electron& el) const
{
  if(!m_pass_siph) return false;

  if(SameCluster(ph, el)) return false;

  if(!PassCuts(ph, m_siph_pt)) return false;
  if(!PassCuts(el, m_siph_xpt)) return false;

  return true;
}

bool DerivationFramework::DiLepFilters::PassSiPhX(const xAOD::Photon& ph1, const xAOD::Photon& ph2) const
{
  if(!m_pass_siph) return false;

  if(SameCluster(ph1, ph2)) return false;

  if(PassCuts(ph1, m_siph_pt) && PassCuts(ph2, m_siph_xpt)) return true;
  if(PassCuts(ph2, m_siph_pt) && PassCuts(ph1, m_siph_xpt)) return true;

  return false;
}

bool DerivationFramework::DiLepFilters::PassSiPhX(const xAOD::Photon& ph, const xAOD::Muon& mu) const
{
  if(!m_pass_siph) return false;

  if(!PassCuts(ph, m_siph_pt)) return false;
  if(!PassCuts(mu, m_siph_xpt, m_mu_eta)) return false;

  return true;
}

bool DerivationFramework::DiLepFilters::PassSiMu(const xAOD::Muon& mu) const
{
  if(!m_pass_simu) return false;

  return PassCuts(mu, m_simu_pt, m_mu_beta);
}

bool DerivationFramework::DiLepFilters::PassDiEl(const xAOD::Electron& el1, const xAOD::Electron& el2) const
{
  if(!m_pass_diph) return false;

  if(SameCluster(el1, el2)) return false;

  if(!PassCuts(el1, m_diel_pt)) return false;
  if(!PassCuts(el2, m_diel_pt)) return false;

  return true;
}

bool DerivationFramework::DiLepFilters::PassDiPh(const xAOD::Photon& ph1, const xAOD::Photon& ph2) const
{
  if(!m_pass_diph) return false;

  if(SameCluster(ph1, ph2)) return false;

  if(!PassCuts(ph1, m_diph_pt)) return false;
  if(!PassCuts(ph2, m_diph_pt)) return false;

  return true;
}

bool DerivationFramework::DiLepFilters::PassDiElPh(const xAOD::Electron& el, const xAOD::Photon& ph) const
{
  if(!m_pass_diph) return false;

  if(SameCluster(el, ph)) return false;

  if(!PassCuts(el, m_dielph_pt)) return false;
  if(!PassCuts(ph, m_dielph_pt)) return false;

  return true;
}

bool DerivationFramework::DiLepFilters::PassDiLoElPh(const xAOD::Electron& el, const xAOD::Photon& ph) const
{
  if(!m_pass_diph) return false;

  if(SameCluster(el, ph)) return false;

  if(!PassCuts(el, m_diloelph_pt, true)) return false;
  if(!PassCuts(ph, m_diloelph_pt)) return false;

  return true;
}

bool DerivationFramework::DiLepFilters::PassEta(const xAOD::IParticle& part, double eta_cut) const
{
  return std::fabs(part.eta()) < eta_cut;
}

bool DerivationFramework::DiLepFilters::Passd0(const xAOD::Electron& el, double d0_cut) const
{
  const xAOD::TrackParticle* el_tr = xAOD::EgammaHelpers::getOriginalTrackParticle(&el);
  if(el_tr == nullptr)
  {
    ATH_MSG_WARNING("Could not retrieve electron ID track!");
    return false;
  }

  return std::fabs(el_tr->d0()) > d0_cut;
}

bool DerivationFramework::DiLepFilters::Passd0(const xAOD::Muon& mu, double d0_cut) const
{
  const xAOD::TrackParticle* mu_tr = mu.trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
  if(mu_tr == nullptr)
  {
    ATH_MSG_WARNING("Could not retrieve muon ID track!");
    return false;
  }

  return std::fabs(mu_tr->d0()) > d0_cut;
}

bool DerivationFramework::DiLepFilters::IsLoose(const xAOD::Egamma& eg) const
{
  bool isLoose = false;
  eg.passSelection(isLoose, "Loose");
  return isLoose;
}

bool DerivationFramework::DiLepFilters::SameCluster(const xAOD::Egamma& eg1, const xAOD::Egamma& eg2) const
{
  const xAOD::CaloCluster* calo1 = eg1.caloCluster();
  const xAOD::CaloCluster* calo2 = eg2.caloCluster();
  if(calo1 == nullptr || calo2 == nullptr)
  {
    ATH_MSG_WARNING("Could not retrieve CaloCluster!");
    return false;
  }

  return calo1->p4().DeltaR(calo2->p4()) < 0.01;
}

bool DerivationFramework::DiLepFilters::IsGood(const xAOD::Muon& mu) const
{
  if(mu.muonType() != xAOD::Muon::Combined) return false;

  float chi2 = 0.;
  if(!mu.parameter(chi2, xAOD::Muon::msInnerMatchChi2)) return false;

  int dof = 1;
  if(!mu.parameter(dof, xAOD::Muon::msInnerMatchDOF)) return false;
  if(dof == 0) dof = 1;

  return (chi2 / static_cast<float>(dof)) < 5.;
}

bool DerivationFramework::DiLepFilters::PassCuts(const xAOD::Electron& el, double pt_cut, bool loose) const
{
  if(el.pt() < pt_cut * m_GeVtoMeV) return false;
  if(!PassEta(el, m_el_eta)) return false;

  if(loose)
  {
    if(!IsLoose(el)) return false;
  }
  else
  {
    if(!Passd0(el, m_el_d0)) return false;
  }

  return true;
}

bool DerivationFramework::DiLepFilters::PassCuts(const xAOD::Photon& ph, double pt_cut) const
{
  if(ph.pt() < pt_cut * m_GeVtoMeV) return false;
  if(!PassEta(ph, m_ph_eta)) return false;
  if(!IsLoose(ph)) return false;

  return true;
}

bool DerivationFramework::DiLepFilters::PassCuts(const xAOD::Muon& mu, double pt_cut, double eta_cut) const
{
  const xAOD::IParticle* mu_ip = nullptr;

  if(IsGood(mu))
  {
    if(!Passd0(mu, m_mu_d0)) return false;

    mu_ip = &mu;
  }
  else
  {
    const xAOD::TrackParticle* mu_mstr = mu.trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);
    if(mu_mstr == nullptr) return false;

    mu_ip = mu_mstr;
  }

  if(mu_ip->pt() < pt_cut * m_GeVtoMeV) return false;
  if(!PassEta(*mu_ip, eta_cut)) return false;

  return true;
}
