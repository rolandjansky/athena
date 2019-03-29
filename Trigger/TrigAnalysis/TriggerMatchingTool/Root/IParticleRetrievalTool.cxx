/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Package includes
#include "TriggerMatchingTool/IParticleRetrievalTool.h"

// Anonymous namespace contains helper functions
namespace {
  /// @brief functions to convert between CLID, xAOD types and container names.
  using CLIDTuple_t =
    std::tuple<HLT::class_id_type, xAOD::Type::ObjectType, std::string>;

  template <std::size_t FROM, std::size_t TO>
    bool CLIDLookUp(
      typename std::tuple_element<TO, CLIDTuple_t>::type& to,
      const typename std::tuple_element<FROM, CLIDTuple_t>::type& from)
    {
      static std::vector<CLIDTuple_t> CLIDVector {
        {1178459224, xAOD::Type::Muon, "xAOD::MuonContainer"},
        {1087532415, xAOD::Type::Electron, "xAOD::ElectronContainer"},
        {1219821989, xAOD::Type::CaloCluster, "xAOD::CaloClusterContainer"},
        {1105575213, xAOD::Type::Photon, "xAOD::PhotonContainer"},
        {1177172564, xAOD::Type::Tau, "xAOD::TauJetContainer"}
      };
      auto itr = std::find_if(CLIDVector.begin(), CLIDVector.end(),
          [from] (const CLIDTuple_t& tup) {
            return std::get<FROM>(tup) == from;
          });
      if (itr == CLIDVector.end() )
        return false;
      else {
        to = std::get<TO>(*itr);
        return true;
      }
    }
  bool CLIDToObjectType(xAOD::Type::ObjectType& type, HLT::class_id_type clid)
  {
    return CLIDLookUp<0, 1>(type, clid);
  }
  bool ObjectTypeToCLID(HLT::class_id_type& clid, xAOD::Type::ObjectType type)
  {
    return CLIDLookUp<1, 0>(clid, type);
  }
  bool CLIDToContainerType(std::string& name, HLT::class_id_type clid)
  {
    return CLIDLookUp<0, 2>(name, clid);
  }

} //> end anonymous namespace

namespace Trig {
  IParticleRetrievalTool::IParticleRetrievalTool(const std::string& name) :
    asg::AsgTool(name)
  {
    declareProperty("TrigDecisionTool", m_tdt,
        "The trigger decision tool to use.");
  }

  IParticleRetrievalTool::~IParticleRetrievalTool() {}

  StatusCode IParticleRetrievalTool::initialize()
  {
    ATH_MSG_INFO("Initializing " << name() );
    ATH_CHECK( m_tdt.retrieve() );
    /// We need the expert methods for the navigation
    m_tdt->ExperimentalAndExpertMethods()->enable();
    return StatusCode::SUCCESS;
  }

  StatusCode IParticleRetrievalTool::retrieveParticles(
      std::vector<std::vector<const xAOD::IParticle*>>& combinations,
      const std::string& chain,
      bool rerun)
  {
    // Make sure what we're getting is cleared.
    combinations.clear();
    // Start by getting the chain group
    const ChainGroup* cg = m_tdt->getChainGroup(chain);
    // Make sure that this group actually contains triggers. If it doesn't then
    // this is very wrong and the job shouldn't be allowed to continue
    if (cg->getListOfTriggers().size() == 0) {
      ATH_MSG_ERROR("Chain group " << chain 
          << " is empty! This means that no matching chains were found!");
      return StatusCode::FAILURE;
    }
    if (!cg->isPassed( rerun 
                       ? TrigDefs::Physics | TrigDefs::allowResurrectedDecision 
                       : TrigDefs::Physics) ) {
      ATH_MSG_DEBUG("Chain: " << chain << " was not passed!");
      return StatusCode::SUCCESS;
    }
    
    FeatureContainer features = cg->features();
    for (const Combination& combo : features.getCombinations() ) {
      // The assumption here is that each combination represents a *single* way
      // in which the trigger could have been passed. This should be true for
      // the types of trigger that this tool expects to examine. It won't be
      // true for (e.g.) jet triggers. 
      std::vector<const xAOD::IParticle*> currentCombination;
      for (const HLT::TriggerElement* te : combo.tes() ) {
        if (!retrieveParticles(currentCombination, te).isSuccess() ) {
          // Interpret a failure this way so that we can report the chain name
          ATH_MSG_ERROR("Failed to retrieve particles for chain " << chain );
          return StatusCode::FAILURE;
        }
      }
      combinations.push_back(currentCombination);
    }
    return StatusCode::SUCCESS;
  }

  StatusCode IParticleRetrievalTool::retrieveParticles(
      std::vector<const xAOD::IParticle*>& combination,
      const HLT::TriggerElement* te) const
  {
    ATH_MSG_DEBUG( "Processing TE " << Trig::getTEName(*te) );
    // Keep track of whether or not we found a particle here.
    const xAOD::IParticle* part = nullptr;
    for (const auto& feature : te->getFeatureAccessHelpers() ) {
      xAOD::Type::ObjectType type;
      if (!CLIDToObjectType(type, feature.getCLID() ) )
        // Skip any features that don't hold 'final' state particles
        continue;
      if (type == xAOD::Type::Electron || type == xAOD::Type::Photon) {
        // If this is an egamma type we have to check if we are meant to be
        // looking for a CaloCluster instead!
        xAOD::Type::ObjectType egType = getEGammaTEType(te);
        if (egType == xAOD::Type::Other) {
          ATH_MSG_ERROR("Unable to determine the correct type for TE "
              << Trig::getTEName(*te) );
          return StatusCode::FAILURE;
        }
        else if (egType == xAOD::Type::CaloCluster) {
          HLT::class_id_type clid = 0;
          ObjectTypeToCLID(clid, egType);
          // This will be where we store the TE containing the calo cluster
          // feature
          const HLT::TriggerElement* sourceTE = nullptr;
          const HLT::TrigNavStructure* navigation = 
            m_tdt->ExperimentalAndExpertMethods()->getNavigation();
          HLT::TriggerElement::FeatureAccessHelper egFeature = 
            navigation->getFeatureRecursively(te, clid, "", sourceTE);
          if (!sourceTE) {
            ATH_MSG_ERROR("Unable to retrieve feature of type " << egType
                << " from TE " << Trig::getTEName(*te) );
            return StatusCode::FAILURE;
          }
          ATH_CHECK( retrieveFeatureParticle(part, egFeature, sourceTE) );
          // If it's a calo-cluster like TE then stop here, otherwise we'll
          // encounter the other EG type again and fail...
          break;
        }
        else if (egType != type) {
          // This is the wrong feature to be looking at
          continue;
        }
        // Otherwise we continue as before
      }
      // If we found a particle from another feature access helper then this is
      // a problem. Our assumption is that there is only one particle per leg!
      if (part) {
        ATH_MSG_ERROR(
            "TE " << Trig::getTEName(*te) 
            << " has multiple 'final' particles attached to it!"
            << " This breaks this tool's assumptions!");
        return StatusCode::FAILURE;
      }
      ATH_CHECK( retrieveFeatureParticle(part, feature, te) );
    } //> end loop over features

    // If we found a particle then we can stop going through this branch of the
    // tree
    if (part) {
      combination.push_back(part);
      return StatusCode::SUCCESS;
    }

    // Otherwise look at each of the next TEs separately
    for (const HLT::TriggerElement* nextTE : 
        HLT::TrigNavStructure::getDirectPredecessors(te) )
      ATH_CHECK( retrieveParticles(combination, nextTE) );

    return StatusCode::SUCCESS;
  }

  xAOD::Type::ObjectType IParticleRetrievalTool::getEGammaTEType(
      const HLT::TriggerElement* te) const
  {
    // We need to check the TE name for this
    std::string teName = Trig::getTEName(*te);
    if (teName.find("etcut") != std::string::npos &&
        teName.find("trkcut") == std::string::npos)
      return xAOD::Type::CaloCluster;
    else if (teName.find("EF_e") == 0)
      return xAOD::Type::Electron;
    else if (teName.find("EF_g") == 0)
      return xAOD::Type::Photon;
    else 
      return xAOD::Type::Other;
  }

  StatusCode IParticleRetrievalTool::retrieveFeatureParticle(
      const xAOD::IParticle*& particle,
      const HLT::TriggerElement::FeatureAccessHelper& feature,
      const HLT::TriggerElement* te) const
  {
    // Get the right container type name
    std::string containerType;
    if (!CLIDToContainerType(containerType, feature.getCLID() ) ) {
      ATH_MSG_ERROR("Unrecognised CLID " << feature.getCLID() << " received!");
      return StatusCode::FAILURE;
    }
    // Build a vector of typeless features so that we can use the central
    // functions.
    const HLT::TrigNavStructure* navigation = 
      m_tdt->ExperimentalAndExpertMethods()->getNavigation();
    std::vector<Trig::TypelessFeature> featureVec;
    featureVec.emplace_back(
        feature,
        te,
        navigation->label(feature.getCLID(), feature.getIndex().subTypeIndex() ) );
    auto particleFeatures = Trig::FeatureAccessImpl::typedGet<
      xAOD::IParticle, xAOD::IParticleContainer, xAOD::IParticleContainer>(
          featureVec, const_cast<HLT::TrigNavStructure*>(navigation),
          &*evtStore(), containerType);
    // Make sure the answer is what we expect
    switch (particleFeatures.size() ) {
      case 0:
        ATH_MSG_ERROR("No particles retrieved from feature "
            << navigation->label(feature.getCLID(), feature.getIndex().subTypeIndex() )
            << " from TE " << Trig::getTEName(*te) );
        return StatusCode::FAILURE;
      case 1:
        // Set the output.
        particle = particleFeatures.at(0);
        break;
      default:
        // Some TEs can end up reporting multiple outputs within the same RoI.
        // AFAIK this only happens within EGamma TEs but I don't know that for
        // sure. In any case this shouldn't matter too much for the matching
        // given that they will be nearby each other. Just return the highest pT
        // object.
        particle = *(std::max_element(
              particleFeatures.begin(), particleFeatures.end(),
              [] (const xAOD::IParticle* lhs, const xAOD::IParticle* rhs)
              { return lhs->pt() < rhs->pt(); }) );
    }
    return StatusCode::SUCCESS;
  }

} //> end namespace Trig
