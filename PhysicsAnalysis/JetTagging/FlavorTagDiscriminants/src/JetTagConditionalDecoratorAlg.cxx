/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/JetTagConditionalDecoratorAlg.h"

namespace FlavorTagDiscriminants {
  JetTagConditionalDecoratorAlg::JetTagConditionalDecoratorAlg(
    const std::string& name, ISvcLocator* svcloc):
    detail::JetTag_t(name, svcloc)
  {
  }

  StatusCode JetTagConditionalDecoratorAlg::initialize() {
    ATH_CHECK(detail::JetTag_t::initialize());
    m_tagFlagReadDecor = SG::ReadDecorHandleKey<xAOD::JetContainer>(
      this, m_tagFlag, m_containerKey.key() + "." + m_tagFlag, "");
    ATH_CHECK(m_tagFlagReadDecor.initialize());
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
