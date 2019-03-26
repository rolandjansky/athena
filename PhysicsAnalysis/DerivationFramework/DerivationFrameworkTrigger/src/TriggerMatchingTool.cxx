/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerMatchingTool.h"
#include "BuildCombinations.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "xAODCore/AuxContainerBase.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "FourMomUtils/xAODP4Helpers.h"
#include <memory>

namespace {
  /// Helper typedefs for accessors/decorators, vectors of ele links
  template <typename T>
    using constAcc_t = SG::AuxElement::ConstAccessor<T>;
  template <typename T>
    using dec_t = SG::AuxElement::Decorator<T>;
  template <typename T>
    using vecLink_t = std::vector<ElementLink<T>>;
} //> end anonymous namespace

namespace DerivationFramework {

  TriggerMatchingTool::TriggerMatchingTool(
      const std::string& type,
      const std::string& name,
      const IInterface* pSvcLocator) :
    AthAlgTool(type, name, pSvcLocator)
  {
    declareInterface<IAugmentationTool>(this);
    declareProperty("ChainNames", m_chainNames, 
        "The list of trigger chains to match.");
    declareProperty("OnlineParticleTool", m_trigParticleTool,
        "The tool to retrieve online particles from the navigation.");
    declareProperty("InputElectrons",
        m_offlineInputs[xAOD::Type::Electron] = "Electrons",
        "Offline electron candidates for matching.");
    declareProperty("InputPhotons",
        m_offlineInputs[xAOD::Type::Photon] = "Photons",
        "Offline photon candidates for matching.");
    declareProperty("InputMuons",
        m_offlineInputs[xAOD::Type::Muon] = "Muons",
        "Offline muon candidates for matching.");
    declareProperty("InputTaus",
        m_offlineInputs[xAOD::Type::Tau] = "TauJets",
        "Offline tau candidates for matching.");
    declareProperty("DRThreshold", m_drThreshold = 0.1,
        "The maximum dR between an offline and an online particle to consider "
        "a match between them.");
    declareProperty("Rerun", m_rerun = false,
        "Whether to match triggers in rerun mode.");
    declareProperty("OutputContainerPrefix", m_outputPrefix="TrigMatch_",
        "The prefix to add to the output containers.");
  }

  StatusCode TriggerMatchingTool::initialize()
  {
    ATH_MSG_INFO( "Initializing " << name() );
    ATH_CHECK( m_trigParticleTool.retrieve() );
    return StatusCode::SUCCESS;
  }

  StatusCode TriggerMatchingTool::addBranches() const
  {

    // Now, get all the possible offline candidates
    std::map<xAOD::Type::ObjectType, particleVec_t> offlineCandidates;
    for (const auto& p : m_offlineInputs) {
      // Skip any that may have been disabled by providing an empty string to
      // the corresponding property
      if (p.second.empty() )
        continue;
      const xAOD::IParticleContainer* cont(nullptr);
      ATH_CHECK( evtStore()->retrieve(cont, p.second) );
      offlineCandidates.emplace(std::make_pair(
          p.first, 
          particleVec_t(cont->begin(), cont->end() )
      ) );
    }

    // Store possible matches from online to offline particles here
    // We do this as multiple chains may use the same online particles so we can
    // reuse this information.
    std::map<const xAOD::IParticle*, particleVec_t> matches;

    // Iterate through the chains to get the matchings
    for (const std::string& chain : m_chainNames) {
      // Create the output
      xAOD::TrigCompositeContainer* container(nullptr);
      ATH_CHECK( createOutputContainer(container, chain) );

      // Get the list of online combinations
      std::vector<particleVec_t> onlineCombinations;
      ATH_CHECK( m_trigParticleTool->retrieveParticles(onlineCombinations, chain, m_rerun) );

      ATH_MSG_DEBUG(
          onlineCombinations.size() << " combinations found for chain" << chain);
      
      // If no combinations were found (because the trigger was not passed) then
      // we can skip this trigger
      if (onlineCombinations.size() == 0)
        continue;

      using particleRange_t = TriggerMatchingUtils::RangedItr<typename particleVec_t::const_iterator>;
      // Now build up the list of offline combinations;
      std::vector<particleVec_t> offlineCombinations;
      for (const particleVec_t& combination : onlineCombinations) {
        // Here we store the possible candidates for the matching. We use the
        // range type as a lightweight method to carry around a view of a vector
        // without copying it (the RangedItr is essentially a combination of 3
        // iterators).
        std::vector<particleRange_t> matchCandidates;
        for (const xAOD::IParticle* part : combination) {
          const particleVec_t& possibleMatches = getCandidateMatchesFor(
              part, offlineCandidates, matches);
          matchCandidates.emplace_back(possibleMatches.begin(), possibleMatches.end() );
        } //> end loop over particles
        // Get all possible combinations of offline objects that could match to
        // this particular online combination.
        auto theseOfflineCombinations = 
          TriggerMatchingUtils::getAllDistinctCombinations<const xAOD::IParticle*>(
              matchCandidates);
        if (msgLvl(MSG::VERBOSE) ) {
          // Spit out some verbose information
          ATH_MSG_VERBOSE(
              "Checking matching for chain " << chain
              << " with " << matchCandidates.size() << " legs");
          std::size_t idx = 0;
          for (const particleRange_t& range : matchCandidates)
            ATH_MSG_VERBOSE( "Leg #" << idx++ << " has " << range.size()
                << " offline candidates." );
          ATH_MSG_VERBOSE(
              "Matching generated " << theseOfflineCombinations.size()
              << " offline combinations");
        }
        // Now push them back into the output. Use a specialised function for
        // inserting into a sorted vector that ensures that we only output
        // unique combinations
        for (const particleVec_t& vec : theseOfflineCombinations)
          TriggerMatchingUtils::insertIntoSortedVector(offlineCombinations, vec);
      } //> end loop over combinations


      // Decorate the found combinations onto trigger composites.
      for (const particleVec_t& foundCombination : offlineCombinations) {
        xAOD::TrigComposite* composite = new xAOD::TrigComposite();
        container->push_back(composite);
        static dec_t<vecLink_t<xAOD::IParticleContainer>> dec_links(
            "TrigMatchedObjects");
        vecLink_t<xAOD::IParticleContainer>& links = dec_links(*composite);
        for (const xAOD::IParticle* part : foundCombination) {
          // TODO This might be dangerous if the user provides the wrong types
          // for the inputs or the input containers are not the owners (so that
          // part->index() isn't the right value...). However I'm not sure what
          // the best option is here...
          links.emplace_back(m_offlineInputs.at(part->type()), part->index() );
        }
      } //> end loop over the found combinations
    } //> end loop over chains
    return StatusCode::SUCCESS;
  }

  StatusCode TriggerMatchingTool::createOutputContainer(
      xAOD::TrigCompositeContainer*& container,
      const std::string& chain) const
  {
    auto uniqueContainer = std::make_unique<xAOD::TrigCompositeContainer>();
    auto aux = std::make_unique<xAOD::AuxContainerBase>();
    uniqueContainer->setStore(aux.get() );
    container = uniqueContainer.get();
    ATH_CHECK( evtStore()->record(std::move(uniqueContainer), m_outputPrefix+chain) );
    ATH_CHECK( evtStore()->record(std::move(aux), m_outputPrefix+chain+"Aux.") );
    return StatusCode::SUCCESS;
  }

  const TriggerMatchingTool::particleVec_t& TriggerMatchingTool::getCandidateMatchesFor(
      const xAOD::IParticle* part,
      std::map<xAOD::Type::ObjectType, particleVec_t>& offlineParticles,
      std::map<const xAOD::IParticle*, particleVec_t>& cache) const
  {
    // Build up all the possible matches between online and offline particles
    auto cacheItr = cache.find(part);
    if (cacheItr == cache.end() ) {
      xAOD::Type::ObjectType type = part->type();
      particleVec_t candidates;
      if (type == xAOD::Type::CaloCluster) {
        // If it's a calo cluster then we need to get the cluster from the
        // egamma types.
        static constAcc_t<vecLink_t<xAOD::CaloClusterContainer>> acc_calo("caloClusterLinks");
        for (xAOD::Type::ObjectType egType : {
            xAOD::Type::Electron, xAOD::Type::Photon})
        {
          for (const xAOD::IParticle* cand : offlineParticles[egType]) {
            const vecLink_t<xAOD::CaloClusterContainer>& links = acc_calo(*cand);
            if (links.size() == 0 || !links.at(0).isValid() )
              continue;
            const xAOD::CaloCluster* clus = *links.at(0);
            if (matchParticles(part, clus) )
              candidates.push_back(cand);
          }
        }
      }
      else {
        for (const xAOD::IParticle* cand : offlineParticles[type])
          if (matchParticles(part, cand) )
            candidates.push_back(cand);
      }
      cacheItr = cache.emplace(
          std::make_pair(part, std::move(candidates) ) ).first;
    }
    return cacheItr->second;
  }

  bool TriggerMatchingTool::matchParticles(
      const xAOD::IParticle* lhs,
      const xAOD::IParticle* rhs) const
  {
    return xAOD::P4Helpers::deltaR(lhs, rhs, false) < m_drThreshold;
  }

} //> end namespace DerivationFramework
