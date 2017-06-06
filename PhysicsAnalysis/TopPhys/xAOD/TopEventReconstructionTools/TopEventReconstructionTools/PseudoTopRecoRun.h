/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTOP_TOPEVENTRECONSTRCUTIONTOOLS_PSEUDOTOPRECORUN_H
#define ANALYSISTOP_TOPEVENTRECONSTRCUTIONTOOLS_PSEUDOTOPRECORUN_H

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopEventReconstructionTools/PseudoTopReco.h"

namespace top{
  class Event;
  class TopConfig;
  
  class PseudoTopRecoRun : public EventSelectorBase {
    public:
    PseudoTopRecoRun(const std::string& kLeptonType, std::shared_ptr<top::TopConfig> config);
    virtual ~PseudoTopRecoRun(){}
    
    virtual bool apply(const top::Event&) const override;

    virtual bool applyParticleLevel(const top::ParticleLevelEvent& plEvent) const override;

    std::string name() const;
    
  private:
    std::string m_name;
    
    std::unique_ptr<top::PseudoTopReco> m_PseudoTopReco;
    
  };
}
#endif
