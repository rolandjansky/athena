#ifndef NTRUTHTOPWSELECTOR_H_
#define NTRUTHTOPWSELECTOR_H_

#include "TopEventSelectionTools/SignValueSelector.h"

namespace top {

  /**
   * @brief A tool that selects events with a certain number of true top-quarks or W-bosons.
   */
  class NTruthTopWSelector : public SignValueSelector {
  public:
    explicit NTruthTopWSelector(const std::string& params);
    
    bool apply(const top::Event& event) const override;
    
  };
  
}

#endif
