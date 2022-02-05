/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JET_TAG_CONDITIONALDECORATOR_ALG_H
#define JET_TAG_CONDITIONALDECORATOR_ALG_H

#include "FlavorTagDiscriminants/DecoratorAlg.h"
#include "FlavorTagDiscriminants/IJetTagConditionalDecorator.h"

#include "xAODJet/JetContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace detail {
  using JetTag_t = FlavorTagDiscriminants::DecoratorAlg<
    xAOD::JetContainer,
    IJetTagConditionalDecorator,
    xAOD::TrackParticleContainer
    >;
}

namespace FlavorTagDiscriminants {
  class JetTagConditionalDecoratorAlg : public detail::JetTag_t
  {
  public:
    JetTagConditionalDecoratorAlg(const std::string& name,
                                  ISvcLocator* svcloc);
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& cxt ) const override;
  private:
    Gaudi::Property<std::string>  m_tagFlag {
      this, "tagFlag", "", "Jet variable to flag a jet for tagging"
    };
    SG::ReadDecorHandleKey<xAOD::JetContainer> m_tagFlagReadDecor;
  };
}

#endif
