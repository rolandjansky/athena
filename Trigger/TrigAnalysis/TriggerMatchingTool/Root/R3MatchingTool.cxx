/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerMatchingTool/R3MatchingTool.h"
#include "xAODBase/IParticleContainer.h"

// Anonymous namespace for helper functions
namespace
{
  // Helper function to efficiently decide if two objects are within a drThreshold of each other
  bool fastDR(float eta1, float phi1, float eta2, float phi2, float drThreshold)
  {
    float dEta = eta1 - eta2;
    if (dEta > drThreshold)
      return false;
    float dPhi = phi1 - phi2;
    if (dPhi > drThreshold)
      return false;
    return dEta * dEta + dPhi * dPhi < drThreshold * drThreshold;
  }
} // namespace

namespace Trig
{

  R3MatchingTool::R3MatchingTool(const std::string &name) : asg::AsgTool(name)
  {
    declareProperty("TrigDecisionTool", m_trigDecTool, "The trigger decision tool");
  }

  R3MatchingTool::~R3MatchingTool() {}

  StatusCode R3MatchingTool::initialize()
  {
    ATH_CHECK(m_trigDecTool.retrieve());
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
    // TODO - detect if we're looking at run 3 data.
    // If we are, then setting rerun to true should give a warning as it no
    // longer makes sense for run 3
    if (recoObjects.size() != 1)
    {
      ATH_MSG_WARNING("Matching of multiple objects is not yet supported!");
      return false;
    }
    // This has to assume that the last IParticle feature in each chain is the
    // correct one - I don't *think* this will cause any issues.
    // Unlike with TEs, each chain should build in nodes that it actually uses.
    // This means that we don't need anything clever for the etcut triggers any more (for example)
    std::vector<IPartLinkInfo_t> features = m_trigDecTool->features<xAOD::IParticleContainer>(
        chain,
        rerun ? TrigDefs::Physics | TrigDefs::allowResurrectedDecision : TrigDefs::Physics);
    ATH_MSG_DEBUG("Found " << features.size() << " features for chain group " << chain);
    // TODO:
    //  Right now we are only looking at single object chains, so we only need
    //  to find a single match for one reco-object.
    //  Longer term we need to deal with combinations so this gets harder
    const xAOD::IParticle &recoObject = *recoObjects.at(0);
    for (const IPartLinkInfo_t &info : features)
    {
      if (!info.link.isValid())
      {
        // This could result in false negatives...
        ATH_MSG_WARNING("Invalid link to trigger feature for chain " << chain);
        continue;
      }
      const xAOD::IParticle &trigObject = **info.link;
      if (fastDR(
              recoObject.eta(),
              recoObject.phi(),
              trigObject.eta(),
              trigObject.phi(),
              matchThreshold))
        // Once we find one match, that is enough
        return true;
    }
    // If we get here then there was no good match
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

} // namespace Trig