#include "TopEventSelectionTools/NJetElectronSelector.h"
#include "TopConfiguration/TopConfig.h"

#include "TopEventSelectionTools/MsgCategory.h"
using namespace TopEventSelectionTools;

#include <algorithm>

namespace top {

  NJetElectronSelector::NJetElectronSelector(const std::string& params, std::shared_ptr<top::TopConfig> config) :
    SignValueSelector("JET_EL_N", params, true) {
    checkMultiplicityIsInteger();
    if (!config->useJetElectrons()) {
      ATH_MSG_ERROR("JET_EL_N is used in one of the selections,\n"
          << "but \"UseJetElectrons\" is \"False\", which is strange.\n"
          << "Make sure this is really intended.\n");
      throw std::runtime_error("NJetElectronSelector: \"UseJetElectrons\" is \"False\"");
    }
  }

  bool NJetElectronSelector::apply(const top::Event& event) const {
    auto func = [&](const xAOD::Jet* jetPtr){return jetPtr->auxdataConst<int>("jet_electron");};
    auto count = std::count_if(event.m_jets.begin(), event.m_jets.end(), func);
    return checkInt(count, multiplicity());
  }
  
  
}
