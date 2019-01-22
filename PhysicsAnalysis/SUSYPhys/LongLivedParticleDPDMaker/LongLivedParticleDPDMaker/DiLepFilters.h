/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// DiLepFilters.h, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Dominik Krauss (krauss@mpp.mpg.de)

#ifndef DERIVATIONFRAMEWORK_DILEPFILTERS_H
#define DERIVATIONFRAMEWORK_DILEPFILTERS_H 1

// STL
#include <string>
#include <vector>

// Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "LongLivedParticleDPDMaker/IDiLepFilters.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODMuon/Muon.h"

namespace DerivationFramework
{
  class DiLepFilters : public AthAlgTool, virtual public IDiLepFilters
  {
    public:
      DiLepFilters(const std::string& t, const std::string& n, const IInterface* p);
      virtual ~DiLepFilters() = default;

      StatusCode initialize() override;

      // retrieves the trigger information
      // has to be called in every event before the filters are used
      // returns true if at least one trigger is passed
      bool GetTriggers() override;

      // single filter implementations
      bool PassSiEl(const xAOD::Electron& el) const override;
      bool PassSiPhX(const xAOD::Photon& ph, const xAOD::Electron& el) const override;
      bool PassSiPhX(const xAOD::Photon& ph1, const xAOD::Photon& ph2) const override;
      bool PassSiPhX(const xAOD::Photon& ph, const xAOD::Muon& mu) const override;
      bool PassSiMu(const xAOD::Muon& mu) const override;

      // di filter implementations
      bool PassDiEl(const xAOD::Electron& el1, const xAOD::Electron& el2) const override;
      bool PassDiPh(const xAOD::Photon& ph1, const xAOD::Photon& ph2) const override;
      bool PassDiElPh(const xAOD::Electron& el, const xAOD::Photon& ph) const override;
      bool PassDiLoElPh(const xAOD::Electron& el, const xAOD::Photon& ph) const override;

    private:
      ToolHandle<Trig::TrigDecisionTool> m_tdt;

      // trigger names
      std::vector<std::string> m_trig_siph;
      std::vector<std::string> m_trig_diph;
      std::vector<std::string> m_trig_simu;

      // trigger flags
      bool m_pass_siph;
      bool m_pass_diph;
      bool m_pass_simu;

      // cut values of the filters
      double m_el_eta;
      double m_ph_eta;
      double m_mu_eta;
      double m_mu_beta;

      double m_el_d0;
      double m_mu_d0;

      double m_siel_pt;
      double m_siph_pt;
      double m_siph_xpt;
      double m_simu_pt;
      double m_diel_pt;
      double m_diph_pt;
      double m_dielph_pt;
      double m_diloelph_pt;

      static constexpr double m_GeVtoMeV = 1000.;

      // small helper functions
      inline bool PassEta(const xAOD::IParticle& part, double eta_cut) const;
      inline bool Passd0(const xAOD::Electron& el, double d0_cut) const;
      inline bool Passd0(const xAOD::Muon& mu, double d0_cut) const;
      inline bool IsLoose(const xAOD::Egamma& eg) const;
      inline bool SameCluster(const xAOD::Egamma& eg1, const xAOD::Egamma& eg2) const;
      inline bool IsGood(const xAOD::Muon& mu) const;

      inline bool PassCuts(const xAOD::Electron& el, double pt_cut, bool loose = false) const;
      inline bool PassCuts(const xAOD::Photon& ph, double pt_cut) const;
      inline bool PassCuts(const xAOD::Muon& mu, double pt_cut, double eta_cut) const;
  };
}

#endif
