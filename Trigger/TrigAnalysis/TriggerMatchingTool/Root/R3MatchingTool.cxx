/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerMatchingTool/R3MatchingTool.h"
#include "xAODBase/IParticleContainer.h"
#include "TrigCompositeUtils/Combinations.h"
#include "xAODEgamma/Egamma.h"
#include <numeric>
#include <algorithm>

namespace Trig
{

  R3MatchingTool::R3MatchingTool(const std::string &name) : asg::AsgTool(name)
  {
    m_trigDecTool.setTypeAndName("Trig::TrigDecisionTool/TrigDecisionTool");
    declareProperty("TrigDecisionTool", m_trigDecTool, "The trigger decision tool");
  }

  R3MatchingTool::~R3MatchingTool() {}

  StatusCode R3MatchingTool::initialize()
  {
    ATH_CHECK(m_trigDecTool.retrieve());
    ATH_CHECK(m_scoreTool.retrieve());
    return StatusCode::SUCCESS;
  }

  bool R3MatchingTool::match(
      const std::vector<const xAOD::IParticle *> &recoObjects,
      const std::string &chain,
      double matchThreshold,
      bool rerun) const
  {
    if (recoObjects.size() == 0)
      // If there are no reco objects, the matching is trivially true
      return true;
    // Make the LinkInfo type less verbose
    using IPartLinkInfo_t = TrigCompositeUtils::LinkInfo<xAOD::IParticleContainer>;
    using VecLinkInfo_t = std::vector<IPartLinkInfo_t>;
    // TODO - detect if we're looking at run 3 data.
    // If we are, then setting rerun to true should give a warning as it no
    // longer makes sense for run 3

    // In what follows, the same comparisons between reco and trigger objects will done
    // fairly frequently. As these include DR checks we want to minimise how often we do these
    // Therefore we keep track of any comparisons that we've already done
    // There is one map per input reco object, and then each map entry is the keyed on information
    // extracted from the element link
    std::vector<std::map<std::pair<uint32_t, uint32_t>, bool>> cachedComparisons(recoObjects.size());

    // Note that the user can supply a regex pattern which matches multiple chains
    // We should return true if any individual chain matches
    const Trig::ChainGroup *chainGroup = m_trigDecTool->getChainGroup(chain);
    for (const std::string &chainName : chainGroup->getListOfTriggers())
    {
      if (!m_trigDecTool->isPassed(chainName, rerun ? TrigDefs::Physics | TrigDefs::allowResurrectedDecision : TrigDefs::Physics))
      {
        ATH_MSG_DEBUG("Chain " << chainName << " did not pass");
        continue;
      }
      ATH_MSG_DEBUG("Chain " << chainName << " passed");
      VecLinkInfo_t features = m_trigDecTool->features<xAOD::IParticleContainer>(chainName);
      // See if we have any that have invalid links. This is a sign that the
      // input file does not contain the required information and should be seen
      // as reason for a job failure
      for (IPartLinkInfo_t &linkInfo : features)
      {
          if (!linkInfo.isValid())
          {
              ATH_MSG_ERROR("Chain " << chainName << " has invalid link info!");
              throw std::runtime_error("Bad link info");
          }
      }
      // Now we have to build up combinations
      // TODO - right now we use a filter that passes everything that isn't pointer-equal.
      // This will probably need to be fixed to something else later - at least the unique RoI filter
      TrigCompositeUtils::Combinations combinations = TrigCompositeUtils::buildCombinations(
        chainName,
        features,
        m_trigDecTool->ExperimentalAndExpertMethods().getChainConfigurationDetails(chainName),
        TrigCompositeUtils::FilterType::UniqueObjects);
      // Warn once per call if one of the chain groups is too small to match anything
      if (combinations.size() < recoObjects.size())
      {
        ATH_MSG_WARNING(
            "Chain " << chainName << " (matching pattern " << chain << ") has too few objects ("
                     << combinations.size() << ") to match the number of provided reco objects (" << recoObjects.size() << ")");
        continue;
      }
      // Now we iterate through the available combinations
      for (const VecLinkInfo_t &combination : combinations)
      {
        // Prepare the index vector
        std::vector<std::size_t> onlineIndices(combination.size());
        std::iota(onlineIndices.begin(), onlineIndices.end(), 0);
        do
        {
          bool match = true;
          for (std::size_t recoIdx = 0; recoIdx < recoObjects.size(); ++recoIdx)
          {
            if (!matchObjects(recoObjects[recoIdx], combination[onlineIndices[recoIdx]].link, cachedComparisons[recoIdx], matchThreshold))
            {
              match = false;
              break;
            }
          }
          if (match)
            return true;
        } while (std::next_permutation(onlineIndices.begin(), onlineIndices.end()));
      }
    }

    // If we reach here we've tried all combinations from all chains in the group and none of them matched
    return false;
  }

  bool R3MatchingTool::match(
      const xAOD::IParticle &recoObject,
      const std::string &chain,
      double matchThreshold,
      bool rerun) const
  {
    std::vector<const xAOD::IParticle *> tmpVec{&recoObject};
    return match(tmpVec, chain, matchThreshold, rerun);
  }

  bool R3MatchingTool::matchObjects(
      const xAOD::IParticle *reco,
      const ElementLink<xAOD::IParticleContainer> &onlineLink,
      std::map<std::pair<uint32_t, uint32_t>, bool> &cache,
      double scoreThreshold) const
  {
    if (!onlineLink.isValid())
    {
      ATH_MSG_WARNING("Invalid element link!");
      return false;
    }
    std::pair<uint32_t, uint32_t> linkIndices(onlineLink.persKey(), onlineLink.persIndex());
    auto cacheItr = cache.find(linkIndices);
    if (cacheItr == cache.end())
    {
      const xAOD::IParticle *online = *onlineLink;
      ATH_MSG_DEBUG("Match online " << online->type() << " to offline " << reco->type());
      bool match = online->type() == reco->type();
      if (online->type() == xAOD::Type::CaloCluster && (reco->type() == xAOD::Type::Electron || reco->type() == xAOD::Type::Photon))
      {
        // Calo cluster is a special case - some of the egamma chains can return these (the etcut chains)
        // In these cases we need to match this against the caloCluster object contained in electrons or photons
        const xAOD::Egamma *egamma = dynamic_cast<const xAOD::Egamma *>(reco);
        if (!egamma)
          // this should never happen
          throw std::runtime_error("Failed to cast to egamma object");
        const xAOD::IParticle *cluster = egamma->caloCluster();
        if (cluster)
          reco = cluster;
        else
          ATH_MSG_WARNING("Cannot retrieve egamma object's primary calorimeter cluster, will match to the egamma object");
        match = true;
      }
      if (match)
        match = m_scoreTool->score(*online, *reco) < scoreThreshold;
      cacheItr = cache.insert(std::make_pair(linkIndices, match)).first;
    }
    return cacheItr->second;
  }

} // namespace Trig
