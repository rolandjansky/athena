#include "FlavorTagDiscriminants/VRJetOverlapDecorator.h"

namespace {
  double getRadius(const TLorentzVector& vec) {
    return std::max(0.02, std::min(0.4, 30e3 / vec.Pt()));
  }
}

VRJetOverlapDecorator::VRJetOverlapDecorator(const std::string& name):
  m_decorator(name)
{
}

void VRJetOverlapDecorator::decorate(const xAOD::JetContainer& jets) const {
  const size_t n_jets = jets.size();
  for (size_t iii = 0; iii < n_jets; iii++) {
    const xAOD::Jet* j1 = jets.at(iii);
    TLorentzVector j1p4 = j1->p4();
    double j1_radius = getRadius(j1p4);
    double closest_jet_relative_dr = INFINITY;
    for (size_t jjj = iii+1; jjj < n_jets; jjj++) {
      TLorentzVector j2p4 = jets.at(jjj)->p4();
      double j2_radius = getRadius(j2p4);
      double min_radius = std::min(j1_radius, j2_radius);
      double rel_dR = j1p4.DeltaR(j2p4) / min_radius;
      closest_jet_relative_dr = std::min(rel_dR, closest_jet_relative_dr);
    }
    m_decorator(*j1) = closest_jet_relative_dr;
  }
}
