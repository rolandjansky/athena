#include "TopEventSelectionTools/NTruthTopWSelector.h"

#include <algorithm>

namespace top {

  NTruthTopWSelector::NTruthTopWSelector(const std::string& params) :
    SignValueSelector("N_Top_W", params, true) {
    checkMultiplicityIsInteger();
  }

  bool NTruthTopWSelector::apply(const top::Event& event) const {
    auto func = [&](const xAOD::TruthParticle* truth){return truth->pdgId()==6 || truth->pdgId()==24 || truth->pdgId()==-6 || truth->pdgId()==-24 ;};
    auto count = std::count_if(event.m_truth->begin(), event.m_truth->end(), func);
    return checkInt(count, multiplicity());
  }
  
  
}
