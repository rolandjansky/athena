/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#ifndef NVARRCJETSELECTOR_H_
#define NVARRCJETSELECTOR_H_

#include "TopEventSelectionTools/SignValueSelector.h"


namespace top {
  class NVarRCJetSelector: public top::SignValueSelector {
    std::string m_name;
  public:
    explicit NVarRCJetSelector(const std::string& name, const std::string& params);

    bool apply(const top::Event& event) const override;
    bool applyParticleLevel(const top::ParticleLevelEvent&) const override;
  };
}

#endif
