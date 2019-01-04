// package includes
#include "EleLinkThinningTool.h"
#include "EleLinkThinningHelpers.h"
#include "EleLinkThinningTypeList.h"
#include "ApplyFunctorSwitch.h"

// athena includes
#include "AthContainers/AuxVectorData.h"
#include "AthContainers/AuxTypeRegistry.h"

// STL includes
#include <regex>

using namespace EleLinkThinningHelpers;

EleLinkThinningTool::EleLinkThinningTool(
    const std::string& type,
    const std::string& name,
    const IInterface* parent) :
  ::AthAlgTool(type, name, parent),
  m_thinningSvc("ThinningSvc", name)
{
  declareInterface<IForwardParentThinningTool>(this);
  declareProperty("ThinningService", m_thinningSvc, "The thinning service.");
  // TODO update LinkName description.
  declareProperty("LinkName", m_linkName, "The name of the link to use. "
      "This can be several links separated by '.' characters. In this case "
      "each link will be followed in turn, retrieving each subsequent "
      "from the current linked container. A '+' character can be prepended "
      "to force an interim container to be thinned, a '-' character forces "
      "the container *not* to be thinned. The default is set by the "
      "ThinInterim property. The final container is *always* thinned.");
  declareProperty("ApplyAnd", m_and = false,
      "Whether or not to use the AND operator when thinning");
  declareProperty("MaxThin", m_maxThin = -1,
      "The maximum number of particles to thin from the final step. If "
      "negative then all final step particles will be thinned.");
}

StatusCode EleLinkThinningTool::initialize()
{
  ATH_MSG_INFO("initialize() " << name() );
  ATH_CHECK( m_thinningSvc.retrieve() );
  if (m_linkName.empty() ) {
    ATH_MSG_ERROR("LinkName not set!");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// Define the destructor here, where there's a full definition of the Helper
// struct (needed as we have a unique_ptr member).
EleLinkThinningTool::~EleLinkThinningTool() {}

StatusCode EleLinkThinningTool::forwardParentThinning(
    const xAOD::IParticleContainer* parent,
    const std::vector<bool>& mask) const
{
  // We create the helper here as the aux name may only have been registered
  // during the first execute function
  if (m_helperVec.helpers.size() == 0) {
    ATH_CHECK( makeHelpers() );
  }
  // Initialise the masks for each step.
  ATH_CHECK( m_helperVec.initMask(*evtStore() ) );

  for (std::size_t idx = 0; idx < mask.size(); ++idx) {
    if (!mask.at(idx) )
      continue;
    // Set the helper vec to the start of this element
    m_helperVec.loadElement(parent, idx);
    std::size_t nThinned = 0;
    std::map<const SG::AuxVectorData*, std::set<std::size_t>> thinnedIndices;
    const SG::AuxVectorData* thinnedContainer = nullptr;
    std::size_t thinnedIdx = -1;
    try {
      while (nThinned < m_maxThin && 
            m_helperVec.next(thinnedContainer, thinnedIdx) ) {
        if (thinnedIndices[thinnedContainer].insert(thinnedIdx).second)
          ++nThinned;
      }
    }
    catch (const EleLinkThinningHelpers::UnknownContainerInLink& e) {
      const std::string* namePtr = evtStore()->keyToString(e.key);
      std::string name = namePtr ? *namePtr : "**unresolved**";
      ATH_MSG_ERROR( "Unexpected container " << name << " found in " 
          << "ElementLink. Candidates were:" );
      for (const std::string& candidate : e.candidates)
        ATH_MSG_ERROR( "\t" << candidate);
      return StatusCode::FAILURE;
    }
  }
  return m_helperVec.thin(*m_thinningSvc);
}

StatusCode EleLinkThinningTool::makeHelpers() const
{
  // The tool allows you to recurse through several levels of element links. If
  // this feature is being used then the links will be separated by a '.'
  // character.
  std::size_t pos = 0;
  while (pos != std::string::npos) {
    std::size_t nextPos = m_linkName.find('.', pos);
    std::string auxName = m_linkName.substr(pos, nextPos-pos);
    pos = (nextPos == std::string::npos ? std::string::npos : nextPos + 1);
    static std::regex auxNameRegex(R"((\w+)(?:\((\w+(?:,\s*\w+)*)\))?)");
    std::smatch m;
    if (!std::regex_match(auxName, m, auxNameRegex) ) {
      ATH_MSG_ERROR("Link name '" << auxName << "' failed to match regex!");
      return StatusCode::FAILURE;
    }
    auxName = m[1];
    std::string containerListStr = m[2];
    static std::regex sepRegex(R"(,\s*)");
    std::vector<std::string> containerList;
    if (!containerListStr.empty() )
      std::copy(
          std::sregex_token_iterator(
            containerListStr.begin(), containerListStr.end(), sepRegex, -1),
          std::sregex_token_iterator(),
          std::back_inserter(containerList) );

    // Now retrieve the aux ID and type info
    SG::auxid_t auxid = SG::AuxTypeRegistry::instance().findAuxID(auxName);
    if (auxid == SG::null_auxid) {
      ATH_MSG_ERROR("Unknown aux name " << auxName << " requested!");
      return StatusCode::FAILURE;
    }
    const std::type_info* ti = SG::AuxTypeRegistry::instance().getType(auxid);
    if (!ti) {
      ATH_MSG_ERROR("Failed to get type_info for aux name " << auxName);
      return StatusCode::FAILURE;
    }
    IThinningSvc::Operator::Type op = m_and ? 
      IThinningSvc::Operator::And : IThinningSvc::Operator::Or;
    // Create the tool
    try {
      m_helperVec.helpers.push_back(EleLinkThinningHelpers::createHelper(
            EleLinkThinningHelpers::TypeList::types, *ti,
            auxName, containerList, op, !containerList.empty() ) );
    }
    catch (const FunctorHelpers::UnknownTypeException& e) {
      ATH_MSG_ERROR("Caught UnknownTypeException: " << e.what() );
      return StatusCode::FAILURE;
    }
  }
  // Make sure that the last tool has thinning enabled.
  if (!m_helperVec.helpers.back()->doThin) {
    ATH_MSG_ERROR("Final step has doThin = false. This probably means that "
        "no containers were specified!");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
