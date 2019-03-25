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
  declareProperty("OldMatchingTool", m_oldMatchTool);
  declareProperty("NewMatchingTool", m_newMatchTool);
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

  // Store the combinations only passing the old tool
  std::vector<std::vector<const xAOD::IParticle*>> oldToolCombinations;
  // Store the combinations only passing the new tool
  std::vector<std::vector<const xAOD::IParticle*>> newToolCombinations;
  // Store the combinations only passing both tools
  std::vector<std::vector<const xAOD::IParticle*>> commonCombinations;

  for (auto itr = testCombinations.begin(); itr != testCombinations.end(); ++itr) {
    bool oldToolMatch = false;
     if (m_usesClusters) {
       std::vector<const xAOD::IParticle*> newCombination;
       for (const xAOD::IParticle* part : *itr) {
         if (part->type() == xAOD::Type::Electron || part->type() == xAOD::Type::Photon)
           newCombination.push_back(dynamic_cast<const xAOD::Egamma*>(part)->caloCluster() );
         else
           newCombination.push_back(part);
       }
       oldToolMatch = m_oldMatchTool->match(newCombination, m_chainName);
     }
     else
       oldToolMatch = m_oldMatchTool->match(*itr, m_chainName);
     bool newToolMatch = m_newMatchTool->match(*itr, m_chainName);
     if (newToolMatch && oldToolMatch)
       commonCombinations.push_back(*itr);
     else if (newToolMatch)
       newToolCombinations.push_back(*itr);
     else if (oldToolMatch)
       oldToolCombinations.push_back(*itr);
  }

  if (newToolCombinations.size() || oldToolCombinations.size() ) {
    ATH_MSG_INFO("Mismatch for chain " << m_chainName 
        << ", new tool found " << commonCombinations.size() + newToolCombinations.size() 
        << " combinations, old tool found " << commonCombinations.size() + oldToolCombinations.size() << ".");
    if (commonCombinations.size() ) {
      ATH_MSG_INFO(commonCombinations.size() << " common combinations were found:");
      for (const auto& combo : commonCombinations) {
        ATH_MSG_INFO("======");
        for (const xAOD::IParticle* part : combo)
          ATH_MSG_INFO(*part);
      }
    }
    if (newToolCombinations.size() ) {
      ATH_MSG_INFO("New tool produced " << newToolCombinations.size() << " combinations not found through the old tool: ");
      for (const auto& combo : newToolCombinations) {
        ATH_MSG_INFO("------");
        for (const xAOD::IParticle* part : combo)
          ATH_MSG_INFO(*part);
      }
    }
    if (oldToolCombinations.size() ) {
      ATH_MSG_INFO("Old tool produced " << oldToolCombinations.size() << " combinations not found through the new tool: ");
      for (const auto& combo : oldToolCombinations) {
        ATH_MSG_INFO("++++++");
        for (const xAOD::IParticle* part : combo)
          ATH_MSG_INFO(*part);
      }
    }
  }

  return StatusCode::SUCCESS;
}
