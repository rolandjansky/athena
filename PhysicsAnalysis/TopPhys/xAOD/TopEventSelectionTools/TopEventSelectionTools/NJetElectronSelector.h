#ifndef NJETELECTRONSELECTOR_H_
#define NJETELECTRONSELECTOR_H_

#include "TopEventSelectionTools/SignValueSelector.h"

namespace top {
  class TopConfig;

  /**
   * @brief A tool that selects events with a certain number of jet-electrons.
   */
  class NJetElectronSelector : public SignValueSelector {
  public:
    explicit NJetElectronSelector(const std::string& params, std::shared_ptr<top::TopConfig> config);
    
    bool apply(const top::Event& event) const override;
    
  };
  
}

#endif
