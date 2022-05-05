/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/JetTagConditionalDecoratorAlg.h"

namespace FlavorTagDiscriminants {
  JetTagConditionalDecoratorAlg::JetTagConditionalDecoratorAlg(
    const std::string& name, ISvcLocator* svcloc):
    detail::JetCondTag_t(name, svcloc)
  {
  }

  StatusCode JetTagConditionalDecoratorAlg::initialize() {
    m_tagFlagReadDecor = m_containerKey.key() + "." + m_tagFlag;
    ATH_CHECK(m_tagFlagReadDecor.initialize());
    ATH_CHECK(detail::JetCondTag_t::initialize());
    return StatusCode::SUCCESS;
  }

  StatusCode JetTagConditionalDecoratorAlg::execute(
    const EventContext& cxt ) const {
    SG::ReadDecorHandle<xAOD::JetContainer, char> container(
      m_tagFlagReadDecor, cxt);
    if (!container.isValid()) {
      ATH_MSG_ERROR("no container " << container.key());
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG(
      "Decorating " + std::to_string(container->size()) + " elements");
    for (const auto* element: *container) {
      if (container(*element)) {
        m_decorator->decorate(*element);
      } else {
        m_decorator->decorateWithDefaults(*element);
      }
    }
    return StatusCode::SUCCESS;
  }

}
