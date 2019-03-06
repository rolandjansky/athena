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
  std::ostream& operator<<(std::ostream& os, const xAOD::IParticle& part)
  {
    os << "(type, pt, eta, phi, m) = ("
      << part.type() << ", "
      << part.pt()   << ", "
      << part.eta()  << ", "
      << part.phi()  << ", "
      << part.m()    << ")";
    return os;
  }
  /// Helper typedefs for accessors/decorators, vectors of ele links
  template <typename T>
    using constAcc_t = SG::AuxElement::ConstAccessor<T>;
  template <typename T>
    using dec_t = SG::AuxElement::Decorator<T>;
  template <typename T>
    using vecLink_t = std::vector<ElementLink<T>>;
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

namespace DerivationFramework {

  TriggerMatchingTool::TriggerMatchingTool(
      const std::string& type,
      const std::string& name,
      const IInterface* pSvcLocator) :
    AthAlgTool(type, name, pSvcLocator),
    m_tdt("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_navigation(nullptr)
  {
    declareInterface<IAugmentationTool>(this);
    declareProperty("ChainNames", m_chainNames, 
        "The list of trigger chains to match.");
    declareProperty("TrigDecisionTool", m_tdt,
        "The trigger decision tool to use.");
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
  }

  StatusCode TriggerMatchingTool::initialize()
  {
    ATH_MSG_INFO( "Initializing " << name() );

    ATH_CHECK( m_tdt.retrieve() );
    // We're going to need the expert methods...
    m_tdt->ExperimentalAndExpertMethods()->enable();

    ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", name() );
    ATH_CHECK( incSvc.retrieve() );
    // All we do on this incident is reset an internal value so the ordering
    // doesn't matter.
    incSvc->addListener(this, IncidentType::BeginInputFile);

    return StatusCode::SUCCESS;
  }

  StatusCode TriggerMatchingTool::addBranches() const
  {
    // Make sure that we have the navigation
    if (!m_navigation)
      m_navigation = m_tdt->ExperimentalAndExpertMethods()->getNavigation();

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
          std::vector<const xAOD::IParticle*>(cont->begin(), cont->end() ) ) );
    }

    // Store possible matches from online to offline particles here
    std::map<const xAOD::IParticle*, particleVec_t> matches;

    // Iterate through the chains to get the matchings
    for (const std::string& chain : m_chainNames) {
      // Create the output
      xAOD::TrigCompositeContainer* container(nullptr);
      ATH_CHECK( createOuputContainer(container, chain) );

      // Retrieve the chain group from the TDT
      const Trig::ChainGroup* cg = m_tdt->getChainGroup(chain);
      // If the chain failed then there's no matching
      if (!cg->isPassed(
            m_rerun ?
            TrigDefs::Physics | TrigDefs::allowResurrectedDecision :
            TrigDefs::Physics) )
        continue;

      using particleRange_t = TriggerMatchingUtils::RangedItr<typename particleVec_t::const_iterator>;
      /* ATH_MSG_INFO("Chain: " << chain ); */
      std::vector<particleVec_t> onlineCombinations;
      ATH_CHECK( getOnlineCombinations( onlineCombinations, *cg) );
      ATH_MSG_INFO("Number of combinations: " << onlineCombinations.size() );
      std::vector<particleVec_t> offlineCombinations;
      /* ATH_MSG_INFO("Number of combinations found: " << onlineCombinations.size() ); */
      for (const particleVec_t& combination : onlineCombinations) {
        std::vector<particleRange_t> matchCandidates;
        for (const xAOD::IParticle* part : combination) {
          const particleVec_t& possibleMatches = getCandidateMatchesFor(
              part, offlineCandidates, matches);
          matchCandidates.emplace_back(possibleMatches.begin(), possibleMatches.end() );
        } //> end loop over particles
        ATH_MSG_INFO(chain << ": checking combos with " <<  matchCandidates.size() << " legs");
        for (const particleRange_t& range : matchCandidates)
          ATH_MSG_INFO("leg size is " << range.size() );
        auto theseCombos = TriggerMatchingUtils::getAllDistinctCombinations<const xAOD::IParticle*>(matchCandidates);
        for (const particleVec_t& vec : theseCombos) {
        /* for (const particleVec_t& vec : */ 
        /*     TriggerMatchingUtils::getAllDistinctCombinations<const xAOD::IParticle*>(matchCandidates) ) { */
          TriggerMatchingUtils::insertIntoSortedVector(offlineCombinations, vec);
        }
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


  void TriggerMatchingTool::handle(const Incident& inc)
  {
    if (inc.type() == IncidentType::BeginInputFile)
      beginNewFile();
    else
      ATH_MSG_WARNING("Received unexpected incident type: " << inc.type() );
  }

  void TriggerMatchingTool::beginNewFile()
  {
    // reset the internal pointer to the navigation
    m_navigation = nullptr;
  }

  StatusCode TriggerMatchingTool::createOuputContainer(
      xAOD::TrigCompositeContainer*& container,
      const std::string& chain) const
  {
    auto uniqueContainer = std::make_unique<xAOD::TrigCompositeContainer>();
    auto aux = std::make_unique<xAOD::AuxContainerBase>();
    uniqueContainer->setStore(aux.get() );
    container = uniqueContainer.get();
    ATH_CHECK( evtStore()->record(std::move(uniqueContainer), chain) );
    ATH_CHECK( evtStore()->record(std::move(aux), chain+"Aux.") );
    return StatusCode::SUCCESS;
  }

  StatusCode TriggerMatchingTool::getOnlineCombinations(
      std::vector<particleVec_t>& combinations,
      const Trig::ChainGroup& cg) const
  {
    // Compile the result in a temporary vector so that the function is
    // side-effect free if there's an error.
    std::vector<particleVec_t> tmp;
    Trig::FeatureContainer features = cg.features();
    ATH_MSG_INFO("Investigating combination (" << features.getCombinations().size() << ")");
    for (const Trig::Combination& comb : features.getCombinations() ) {
      ATH_CHECK( getCombination(tmp, comb) );
      auto iparticles = comb.getIParticle(1219821989, "xAOD::CaloClusterContainer");
      ATH_MSG_INFO( "getIParticle found " << iparticles.size() << " results" );
      for (const auto& f : iparticles)
        ATH_MSG_INFO(*f.cptr() );
    }
    ATH_MSG_INFO("Found " << tmp.size() << " combos");
    for (const particleVec_t& v : tmp) {
      ATH_MSG_INFO( "leg size " << v.size() );
      for (const xAOD::IParticle* p : v)
        ATH_MSG_INFO(*p);
    }

    std::swap(tmp, combinations);
    return StatusCode::SUCCESS;
  }

  StatusCode TriggerMatchingTool::getCombination(
      std::vector<particleVec_t>& output,
      const Trig::Combination& combination) const
  {
    // The vector of particles that correspond to this combination
    particleVec_t particles;
    for (const HLT::TriggerElement* te : combination.tes() )
      ATH_CHECK( processTE(particles, te) );
    output.push_back(std::move(particles) );
    return StatusCode::SUCCESS;
  }

  StatusCode TriggerMatchingTool::processTE(
      particleVec_t& particles,
      const HLT::TriggerElement* te) const
  {
    ATH_MSG_DEBUG("TE name: " << Trig::getTEName(*te) );
    const xAOD::IParticle* part = nullptr;
    for (const auto& feature : te->getFeatureAccessHelpers() ) {
      xAOD::Type::ObjectType type;
      if (!CLIDToObjectType(type, feature.getCLID() ) )
        continue;
      ATH_MSG_DEBUG("Looking at feature with type: " << type);
      ATH_MSG_DEBUG( feature );
      // If we already found a particle from this TE then there's a problem
      if (part) {
        ATH_MSG_ERROR("Single TE has multiple 'final' particles attached to it!"
            << " This is not expected!");
        return StatusCode::FAILURE;
      }
      // If the TE contains an egamma type then we have to use a specialised
      // function
      if (type == xAOD::Type::Electron || type == xAOD::Type::Photon)
        return processEGammaTE(particles, te);
      else
        ATH_CHECK(retrieveFeatureParticle(part, feature, te) );
    }
    // If we found a particle then we're done for this leg.
    if (part) {
      particles.push_back(part);
      return StatusCode::SUCCESS;
    }
    // Otherwise, the chain diverges here and we need to process each leg
    // separately.
    for (const HLT::TriggerElement* nextTE : 
        HLT::TrigNavStructure::getDirectPredecessors(te) )
      ATH_CHECK( processTE(particles, nextTE) );
    return StatusCode::SUCCESS;
  }

  StatusCode TriggerMatchingTool::processEGammaTE(
      particleVec_t& particles,
      const HLT::TriggerElement* te) const
  {
    // TODO (?)
    // When this is looking at an etcut trigger there's no way to communicate
    // whether this should be matched by an electron or a photon, so it will be
    // matched to both. I don't know that this is really an issue (it's not
    // inconceivable that a user might want to match a photon to an 'electron'
    // etcut trigger after all.
    
    // First, see if we've already seen a TE of this type before
    auto itr = m_egammaTEs.find(te->getId() );
    if (itr == m_egammaTEs.end() ) {
      std::string teName = Trig::getTEName(*te);
      xAOD::Type::ObjectType teType;
      if (teName.find("etcut") != std::string::npos)
        teType = xAOD::Type::CaloCluster;
      else if (teName.find("EF_e") == 0)
        teType = xAOD::Type::Electron;
      else if (teName.find("EF_g") == 0)
        teType = xAOD::Type::Photon;
      else {
        ATH_MSG_ERROR("Unable to determine the correct type to retrieve from "
            << " te " << teName << " (ID = " << te->getId() << ")" );
        return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG("TE ID: " << te->getId() << ", name: " << teName << ", type: " << teType);
      itr = m_egammaTEs.emplace(te->getId(), teType).first;
    }
    ATH_MSG_DEBUG( "Looking for TE: " << itr->second );
    xAOD::Type::ObjectType teType = itr->second;
    HLT::class_id_type clid = 0;
    ObjectTypeToCLID(clid, teType);
    const HLT::TriggerElement* sourceTE = nullptr;
    HLT::TriggerElement::FeatureAccessHelper feature = 
      m_navigation->getFeatureRecursively(te, clid, "", sourceTE);
    if (!sourceTE) {
      ATH_MSG_ERROR("Unable to retrieve feature of type " << teType <<
          " from TE " << Trig::getTEName(*te) );
      return StatusCode::FAILURE;
    }
    const xAOD::IParticle* part = nullptr;
    ATH_CHECK( retrieveFeatureParticle(part, feature, te) );
    particles.push_back(part);
    return StatusCode::SUCCESS;
  }

  StatusCode TriggerMatchingTool::retrieveFeatureParticle(
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
    std::vector<Trig::TypelessFeature> featureVec;
    featureVec.emplace_back(
        feature,
        te,
        m_navigation->label(feature.getCLID(), feature.getIndex().subTypeIndex() ) );
    auto particlesFeatures = Trig::FeatureAccessImpl::typedGet<
      xAOD::IParticle, xAOD::IParticleContainer, xAOD::IParticleContainer>(
          featureVec, const_cast<HLT::TrigNavStructure*>(m_navigation), &*evtStore(), containerType);
    switch (particlesFeatures.size() ) {
      case 0:
        ATH_MSG_ERROR("No particles retrieved from feature!");
        return StatusCode::FAILURE;
      case 1:
        particle = particlesFeatures.at(0);
        return StatusCode::SUCCESS;
      default:
        // Multiple online objects reconstructed within the same RoI! This *can*
        // happen. In these cases they'll all be very close together so it's not
        // a problem for the matching. Just return the highest pt candidate.
        particle = *(std::max_element(
              particlesFeatures.begin(), particlesFeatures.end(),
              [] (const xAOD::IParticle* lhs, const xAOD::IParticle* rhs)
              { return lhs->pt() < rhs->pt(); }) );
    }
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
