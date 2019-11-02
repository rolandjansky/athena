/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRACKJETCLEANINGSELECTOR_H_
#define TRACKJETCLEANINGSELECTOR_H_

#include "TopEventSelectionTools/EventSelectorBase.h"

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "JetInterface/IJetSelector.h"
#include "JetSelectorTools/IEventCleaningTool.h"

namespace top {
  class TopConfig;

  class TrackJetCleaningSelector: public EventSelectorBase {
  public:
    explicit TrackJetCleaningSelector(std::shared_ptr<top::TopConfig> config);

    bool apply(const top::Event& event) const override;

    std::string name() const override;
  private:
    std::shared_ptr<top::TopConfig> m_config;
  };
}

#endif
