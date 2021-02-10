/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerMatchingTool/R3IParticleRetrievalTool.h"
#include "xAODBase/IParticleContainer.h"
#include "TrigCompositeUtils/Combinations.h"

namespace Trig
{
  R3IParticleRetrievalTool::R3IParticleRetrievalTool(const std::string &name)
      : asg::AsgTool(name)
  {
    declareProperty("TrigDecisionTool", m_tdt);
  }

  R3IParticleRetrievalTool::~R3IParticleRetrievalTool() {}

  StatusCode R3IParticleRetrievalTool::initialize()
  {
    ATH_MSG_INFO("Initializing " << name());
    ATH_CHECK(m_tdt.retrieve());
    // We need the expert methods for the navigation
    m_tdt->ExperimentalAndExpertMethods()->enable();
    return StatusCode::SUCCESS;
  }

  StatusCode R3IParticleRetrievalTool::retrieveParticles(
      std::vector<std::vector<const xAOD::IParticle *>> &combinations,
      const std::string &chain,
      bool rerun) const
  {
    using IPartLinkInfo_t = TrigCompositeUtils::LinkInfo<xAOD::IParticleContainer>;
    using VecLinkInfo_t = std::vector<IPartLinkInfo_t>;
    // Make sure we start from a blank slate
    combinations.clear();

    // Start by getting the chain group
    const ChainGroup *cg = m_tdt->getChainGroup(chain);
    // Make sure that this group actually contains triggers. If it doesn't then
    // this is very wrong and the job shouldn't be allowed to continue
    if (cg->getListOfTriggers().size() == 0)
    {
      ATH_MSG_ERROR("Chain group " << chain
                                   << " is empty! This means that no matching chains were found!");
      return StatusCode::FAILURE;
    }

    // Now go chain by chain
    for (const std::string &name : cg->getListOfTriggers())
    {
      if (!m_tdt->isPassed(name, rerun ? TrigDefs::Physics | TrigDefs::allowResurrectedDecision : TrigDefs::Physics))
      {
        ATH_MSG_DEBUG("Chain " << name << " did not pass");
        continue;
      }
      ATH_MSG_DEBUG("Chain " << name << " passed");
      // Build up the full list of trigger combinations
      // TODO - right now we use a filter that passes everything that isn't pointer-equal.
      // This will probably need to be fixed to something else later - at least the unique RoI filter
      TrigCompositeUtils::Combinations trigCombinations = TrigCompositeUtils::buildCombinations(
          name,
          m_tdt->features<xAOD::IParticleContainer>(name),
          m_tdt->ExperimentalAndExpertMethods()->getChainConfigurationDetails(name),
          TrigCompositeUtils::FilterType::UniqueObjects);
      // Copy the combinations into the output vector
      for (const VecLinkInfo_t &combo : trigCombinations)
      {
        std::vector<const xAOD::IParticle *> comboOut;
        comboOut.reserve(combo.size());
        for (const IPartLinkInfo_t &info : combo)
          comboOut.push_back(*info.link);
        combinations.push_back(std::move(comboOut));
      }
    }
    return StatusCode::SUCCESS;
  }
} // namespace Trig