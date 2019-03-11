// TriggerMatchingTest includes
#include "TriggerMatchingTestAlg.h"
#include "BuildCombinations.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODEgamma/Egamma.h"

#include <regex>


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
  std::vector<std::string> splitString(const std::string& input, const std::string& delim)
  {
    std::vector<std::string> output;
    std::string::size_type pos = 0;
    while (true) {
      auto nextPos = input.find(delim, pos);
      output.push_back(input.substr(pos, nextPos-pos) );
      if (nextPos == std::string::npos)
        break;
      pos = nextPos + delim.size();
    }
    return output;
  }
}


TriggerMatchingTestAlg::TriggerMatchingTestAlg( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm( name, pSvcLocator ){
  declareProperty("ChainName", m_chainName);
  declareProperty("InputElectrons", m_inputColls[xAOD::Type::Electron] = "Electrons");
  declareProperty("InputPhotons", m_inputColls[xAOD::Type::Photon] = "Photons");
  declareProperty("InputMuons", m_inputColls[xAOD::Type::Muon] = "Muons");
  declareProperty("InputTaus", m_inputColls[xAOD::Type::Tau] = "TauJets");
  declareProperty("MatchingTool", m_matchTool);
}


TriggerMatchingTestAlg::~TriggerMatchingTestAlg() {}


StatusCode TriggerMatchingTestAlg::initialize() {

  ATH_MSG_INFO ("Initializing " << name() << "...");

  std::vector<std::string> chainDetails = splitString(m_chainName, "_");

  // The list of signature names here comes from
  // https://gitlab.cern.ch/atlas/athena/blob/21.2/Trigger/TriggerCommon/TriggerMenu/python/menu/SignatureDicts.py
  std::regex signaturePattern(R"((\d*)(e|j|ht|g|mu|tau|xe|xs|te|afp|mb|hi|cosmic|calib|streamer|mon|beamspot|eb|TestChain)(\d+))");

  xAOD::Type::ObjectType currentObj = xAOD::Type::Other;
  std::size_t currentMult = 0;
  for (const std::string& detail : chainDetails) {
    std::smatch sm;
    if (std::regex_match(detail, sm, signaturePattern) ) {
      if (currentObj != xAOD::Type::Other)
        m_legs[currentObj] += currentMult;
      if (sm[1] == "")
        currentMult = 1;
      else
        currentMult = std::stoi(sm[1]);
      if (sm[2] == "e")
        currentObj = xAOD::Type::Electron;
      else if (sm[2] == "g")
        currentObj = xAOD::Type::Photon;
      else if (sm[2] == "mu")
        currentObj = xAOD::Type::Muon;
      else if (sm[2] == "tau")
        currentObj = xAOD::Type::Tau;
      else
        currentObj = xAOD::Type::Other;
    }
    else if (detail == "etcut" && currentObj != xAOD::Type::Other) {
      currentObj = xAOD::Type::CaloCluster;
      m_usesClusters = true;
    }
  }
  if (currentObj != xAOD::Type::Other)
    m_legs[currentObj] += currentMult;

  return StatusCode::SUCCESS;
}


StatusCode TriggerMatchingTestAlg::execute() {  

  std::map<xAOD::Type::ObjectType, std::vector<const xAOD::IParticle*>> inputs;
  for (const auto& p : m_inputColls) {
    const xAOD::IParticleContainer* cont = nullptr;
    ATH_CHECK(evtStore()->retrieve(cont, p.second) );
    inputs[p.first].assign(cont->begin(), cont->end() );
  }

  if (m_usesClusters) {
    auto& caloVec = inputs[xAOD::Type::CaloCluster];
    for (xAOD::Type::ObjectType egType : {xAOD::Type::ObjectType::Electron, xAOD::Type::ObjectType::Photon})
      caloVec.insert(caloVec.end(), inputs[egType].begin(), inputs[egType].end() );
  }

  // Now, collect all combinations of these things for each leg
  std::vector<DerivationFramework::TriggerMatchingUtils::RangedItr<typename std::vector<const xAOD::IParticle*>::const_iterator>> legInputs;
  for (const auto& legPair : m_legs) {
    for (std::size_t ii = 0; ii < legPair.second; ++ii)
      legInputs.emplace_back(inputs[legPair.first].begin(), inputs[legPair.first].end() );
  }

  // Now build the full set of possible offline combinations. This is probably
  // going to pretty huge...
  std::vector<std::vector<const xAOD::IParticle*>> testCombinations = DerivationFramework::TriggerMatchingUtils::getAllDistinctCombinations<const xAOD::IParticle*>(legInputs);

  // Now go through and remove any that doesn't pass the matching tool
   for (auto itr = testCombinations.begin(); itr != testCombinations.end();) {
     if (m_usesClusters) {
       std::vector<const xAOD::IParticle*> newCombination;
       for (const xAOD::IParticle* part : *itr) {
         if (part->type() == xAOD::Type::Electron || part->type() == xAOD::Type::Photon)
           newCombination.push_back(dynamic_cast<const xAOD::Egamma*>(part)->caloCluster() );
         else
           newCombination.push_back(part);
       }
       if (!m_matchTool->match(newCombination, m_chainName) )
         itr = testCombinations.erase(itr);
       else
         ++itr;
     }
     else {
       if (!m_matchTool->match(*itr, m_chainName) )
         itr = testCombinations.erase(itr);
       else
         ++itr;
     }
  }

  std::size_t nTest = testCombinations.size();

  // Now get the matchings that were obtained through the new tool. Note that
  // we're actually only going to put ones in here that *weren't* in the
  // previous vector. After this loop is done, the only ones remaining in the
  // previous vector will be the ones that weren't obtained through the new
  // tool!
  std::vector<std::vector<const xAOD::IParticle*>> newToolCombinations;

  // This holds the common combinations
  std::vector<std::vector<const xAOD::IParticle*>> commonCombinations;

  const xAOD::TrigCompositeContainer* cont(nullptr);
  ATH_CHECK( evtStore()->retrieve(cont, m_chainName) );

  std::size_t nNewTool = cont->size();
  static SG::AuxElement::ConstAccessor<std::vector<ElementLink<xAOD::IParticleContainer>>> acc_links("TrigMatchedObjects");
  for (const xAOD::TrigComposite* comp : *cont) {
    std::vector<const xAOD::IParticle*> combination;
    for (const auto& link : acc_links(*comp) )
      combination.emplace_back(*link);
    // See if this was in the other list (allow any permutation)
    auto itr = std::find_if(
        testCombinations.begin(),
        testCombinations.end(),
        [newItr = combination.begin()] (const std::vector<const xAOD::IParticle*>& testCombo) 
        { return std::is_permutation(testCombo.begin(), testCombo.end(), newItr); });
    if (itr !=testCombinations.end() ) {
      // If it was, remove it from that list
      testCombinations.erase(itr);
      commonCombinations.push_back(std::move(combination) );
    }
    else {
      // otherwise add it to the newToolCombinations list
      newToolCombinations.push_back(std::move(combination) );
    }
  }
  // See if we had any mismatches
  if (newToolCombinations.size() || testCombinations.size() ) {
    ATH_MSG_INFO("Mismatch for chain " << m_chainName << ", tool found " << nNewTool << " combinations, test found " << nTest << ".");
    if (commonCombinations.size() ) {
      ATH_MSG_INFO(commonCombinations.size() << " common combinations were found:");
      for (const auto& combo : commonCombinations) {
        ATH_MSG_INFO("======");
        for (const xAOD::IParticle* part : combo)
          ATH_MSG_INFO(*part);
      }
    }
    if (newToolCombinations.size() ) {
      ATH_MSG_INFO("New tool produced " << newToolCombinations.size() << " combinations not found through the matching: ");
      for (const auto& combo : newToolCombinations) {
        ATH_MSG_INFO("------");
        for (const xAOD::IParticle* part : combo)
          ATH_MSG_INFO(*part);
      }
    }
    if (testCombinations.size() ) {
      ATH_MSG_INFO("Matching produced " << testCombinations.size() << " combinations not found through the new tool: ");
      for (const auto& combo : testCombinations) {
        ATH_MSG_INFO("++++++");
        for (const xAOD::IParticle* part : combo)
          ATH_MSG_INFO(*part);
      }
    }
  }

  return StatusCode::SUCCESS;
}
