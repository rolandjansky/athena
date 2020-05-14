/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "FlavorTagDiscriminants/VRJetOverlapDecorator.h"

namespace {

  struct VRConfig {
    VRConfig();
    double min;
    double max;
    double scale;
  };
  VRConfig::VRConfig():
    min(NAN),
    max(NAN),
    scale(NAN)
  {}


  double getRadius(const TLorentzVector& vec, const VRConfig& cfg) {
    return std::max(cfg.min, std::min(cfg.max, cfg.scale / vec.Pt()));
  }
  struct VRJetDR {
    double relative;
    double absolute;
  };

  std::vector<VRJetDR> getMinimumRelativeDR(const xAOD::JetContainer& jets,
                                            const VRConfig& cfg) {
    const size_t n_jets = jets.size();
    std::vector<VRJetDR> min_dr(n_jets, {INFINITY,INFINITY});
    for (size_t iii = 0; iii < n_jets; iii++) {
      const xAOD::Jet* j1 = jets.at(iii);
      const TLorentzVector j1p4 = j1->p4();
      const double j1_radius = getRadius(j1p4, cfg);
      for (size_t jjj = iii+1; jjj < n_jets; jjj++) {
        const xAOD::Jet* j2 = jets.at(jjj);
        const TLorentzVector j2p4 = j2->p4();
        const double j2_radius = getRadius(j2p4, cfg);
        const double min_radius = std::min(j1_radius, j2_radius);
        const double dR = j1p4.DeltaR(j2p4);
        const double rel_dR = dR / min_radius;
        // check both jets, if the relative dR is less than whatever
        // we have recorded, save this relative dR and absolute dR
        for (const size_t idx: {iii,jjj}) {
          if (rel_dR < min_dr.at(idx).relative) {
            VRJetDR vrstruct;
            vrstruct.relative = rel_dR;
            vrstruct.absolute = dR;
            min_dr.at(idx) = vrstruct;
          }
        }
      }
    }
    return min_dr;
  }

}

VRJetOverlapConfig::VRJetOverlapConfig(VRJetParameters param_source):
  relative_name("relativeDeltaRToVRJet"),
  absolute_name("deltaRToVRJet"),
  jet_parameters_source(param_source)
{}


VRJetOverlapDecorator::VRJetOverlapDecorator(const VRJetOverlapConfig& cfg):
  m_param_source(cfg.jet_parameters_source),
  m_rel_decorator(cfg.relative_name),
  m_abs_decorator(cfg.absolute_name),
  m_min_radius("VariableRMinRadius"),
  m_max_radius("SizeParameter"),
  m_mass_scale("VariableRMassScale")
{
}

void VRJetOverlapDecorator::decorate(const xAOD::JetContainer& jets) const {
  if (jets.size() < 2) return;
  VRConfig cfg;
  if (m_param_source == VRJetParameters::EDM) {
    // we'll assume all the jets have the same parameters, I don't
    // know of counterexamples but if they exist we'd have to rethink
    // all the code here.
    const xAOD::Jet* jet = jets.at(0);
    cfg.min = m_min_radius(*jet);
    cfg.max = m_max_radius(*jet);
    cfg.scale = m_mass_scale(*jet);
  } else if (m_param_source == VRJetParameters::RHO30MIN02MAX4) {
    // right now we only have one set of VR jets where we'd use this
    // tool thus the hardcoded numbers here.
    cfg.min = 0.02;
    cfg.max = 0.4;
    cfg.scale = 30e3;
  } else {
    throw std::logic_error("unknown jet parameter lookup");
  }
  auto min_dr = getMinimumRelativeDR(jets, cfg);
  for (size_t iii = 0; iii < jets.size(); iii++) {
    const xAOD::Jet* jet = jets.at(iii);
    const auto drs = min_dr.at(iii);
    m_rel_decorator(*jet) = drs.relative;
    m_abs_decorator(*jet) = drs.absolute;
  }
}
