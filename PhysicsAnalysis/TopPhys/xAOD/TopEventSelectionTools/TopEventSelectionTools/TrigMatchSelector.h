/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMATCHSELECTOR_H_
#define TRIGMATCHSELECTOR_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include <memory>

namespace top { 
  // Forward declare
  class Event; 
  class TopConfig;

  /**
  * @brief Match offline object to trigger.
  *
  */
  class TrigMatchSelector : public EventSelectorBase {
    public:

      TrigMatchSelector(const std::string& selectorName,std::shared_ptr<top::TopConfig> config);

      virtual bool apply(const top::Event& event) const override;

      std::string name() const override;
      
    private:
      ///List of triggers to 'or' together for each event. If any one passes, the event passes
      std::vector<std::string> m_electronTriggers;      
      std::vector<std::string> m_muonTriggers;
      std::vector<std::string> m_tauTriggers;
  };
}

#endif /* TRIGDECISIONSELECTOR_H_ */
