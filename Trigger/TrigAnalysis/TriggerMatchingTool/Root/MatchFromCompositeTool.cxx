/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerMatchingTool/MatchFromCompositeTool.h"
#include "FourMomUtils/xAODP4Helpers.h"

namespace Trig {
  MatchFromCompositeTool::MatchFromCompositeTool(const std::string& name) :
    asg::AsgTool(name)
  {
    declareProperty("MatchShallow", m_matchShallow,
        "Whether to check if two objects are shallow copies of each other. "
        "If this is not true then pointer equality will be used.");
    declareProperty("DRThreshold", m_drThreshold,
        "If greater than 0 then use the DR between two objects to check if "
        "they are the same object.");
    declareProperty("InputPrefix", m_inputPrefix="TrigMatch_",
        "The input prefix to expect at the beginning of the TrigComposite "
        "container names.");
  }

  MatchFromCompositeTool::~MatchFromCompositeTool() {}

  StatusCode MatchFromCompositeTool::initialize() {
    ATH_MSG_INFO( "initializing " << name() );
    return StatusCode::SUCCESS;
  }

  bool MatchFromCompositeTool::match(
      const xAOD::IParticle& recoObject,
      const std::string& chain,
      double, bool)
  {
    return match({&recoObject}, chain);
  }

  bool MatchFromCompositeTool::match(
      const std::vector<const xAOD::IParticle*>& recoObjects,
      const std::string& chain,
      double, bool)
  {
    const xAOD::TrigCompositeContainer* composites(nullptr);
    if (evtStore()->retrieve(composites, m_inputPrefix+chain).isFailure() )
      throw std::runtime_error(
          "Failed to retrieve composite corresponding to chain " + chain);
    for (const xAOD::TrigComposite* composite : *composites) {
      static constAcc_t<vecLink_t<xAOD::IParticleContainer>> accMatched(
          "TrigMatchedObjects");
      if (testCombination(accMatched(*composite), recoObjects) )
        return true;
    }
    // If we get here then none of the online combinations worked
    return false;
  }

  bool MatchFromCompositeTool::testCombination(
      const vecLink_t<xAOD::IParticleContainer>& onlineLinks,
      const std::vector<const xAOD::IParticle*>& offline) const
  {
    // We need to make sure we don't allow two offline particles to match to the
    // same online particle, so we copy the vector so we can erase objects if
    // necessary. Dereference the links at the same time.
    std::vector<const xAOD::IParticle*> online;
    online.reserve(onlineLinks.size() );
    for (const ElementLink<xAOD::IParticleContainer>& link : onlineLinks)
      // This line could throw an exception if the link is invalid!
      online.push_back(*link);
    // I will follow the way the current tool works and match even if there are
    // fewer reco objects than trigger objects
    for (const xAOD::IParticle* offlinePart : offline) {
      bool isMatched = false;
      for (auto itr = online.begin(); itr != online.end(); ++itr) {
        if (areTheSame(*offlinePart, **itr) ) {
          // Remove this online particle from consideration
          online.erase(itr);
          isMatched = true;
          break;
        }
      }
      if (!isMatched)
        // We've found an offline particle we couldn't match so the whole
        // combination doesn't match.
        return false;
    }
    // To reach this point every offline particle must have found a match.
    return true;
  }

  bool MatchFromCompositeTool::areTheSame(
      const xAOD::IParticle& lhs,
      const xAOD::IParticle& rhs) const
  {
    // If we've been given a dR threshold > 0 then we just use that.
    if (m_drThreshold > 0) {
      return xAOD::P4Helpers::deltaR(lhs, rhs, false) < m_drThreshold;
    }
    else if (m_matchShallow) {
      static SG::AuxElement::ConstAccessor<
        ElementLink<xAOD::IParticleContainer>> accOOL("originalObjectLink");
      // For now assume that we've got a shallow copy iff this is available
      const xAOD::IParticle* lhsOrig = 
        accOOL.isAvailable(lhs) ? *accOOL(lhs) : &lhs;
      const xAOD::IParticle* rhsOrig = 
        accOOL.isAvailable(rhs) ? *accOOL(rhs) : &rhs;
      return lhsOrig == rhsOrig;
    }
    else {
      // Otherwise just use pointer equality
      return &lhs == &rhs;
    }
  }

} //> end namespace Trig
