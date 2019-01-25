/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
// package includes
#include "EleLinkThinningTool.h"
#include "EleLinkThinningHelpers.h"
#include "EleLinkThinningTypeList.h"

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
  declareProperty("LinkName", m_linkName, "The name of the link to use. "
      "The format should be <linkName>(list,of,containers)[maxToProcess]. "
      "The list of containers and maxToProcess are optional; if the list of "
      "containers is not set then no thinning will be applied to elements "
      "pointed to by that link, if maxToProcess is not set then all elements "
      "from that link will be processed. Thinning is allowed to recurse down "
      "chains of element links, in this case multiple copies of this pattern "
      "should be provided, separated by '.' characters. Each link will be read "
      "from the elements provided by the previous link in the chain. The whole "
      "chain will be exhausted when one of its constituent links hits its "
      "maxToProcess.");
  declareProperty("ApplyAnd", m_and = false,
      "Whether or not to use the AND operator when thinning");
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
  if (!m_helper) {
    ATH_CHECK( makeHelper() );
  }
  // Initialise the masks for each step.
  ATH_CHECK( m_helper->initMask(*evtStore() ) );

  for (std::size_t idx = 0; idx < mask.size(); ++idx) {
    if (!mask.at(idx) )
      continue;
    // Set the helper to the start of this element
    m_helper->loadElement(parent, idx);
    const SG::AuxVectorData* container;
    std::size_t index;
    try {
      while (m_helper->next(container, index) ) {}
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
  return m_helper->thin(*m_thinningSvc);
}

StatusCode EleLinkThinningTool::makeHelper() const
{
  // The tool allows you to recurse through several levels of element links. If
  // this feature is being used then the links will be separated by a '.'
  // character.
  std::size_t pos = 0;
  while (pos != std::string::npos) {
    std::size_t nextPos = m_linkName.find('.', pos);
    std::string auxName = m_linkName.substr(pos, nextPos-pos);
    pos = (nextPos == std::string::npos ? std::string::npos : nextPos + 1);
    static std::regex auxNameRegex(
        R"((\w+)(?:\((\w+(?:,\s*\w+)*)\))?(?:\[(\d*)\])?)");
    std::smatch m;
    if (!std::regex_match(auxName, m, auxNameRegex) ) {
      ATH_MSG_ERROR("Link name '" << auxName << "' failed to match regex!");
      return StatusCode::FAILURE;
    }
    auxName = m[1];
    std::string containerListStr = m[2];
    std::size_t maxToSee = m[3].str().empty() ? -1 : std::stoi(m[3]);
    static std::regex sepRegex(R"(,\s*)");
    std::vector<std::string> containerList;
    if (!containerListStr.empty() )
      std::copy(
          std::sregex_token_iterator(
            containerListStr.begin(), containerListStr.end(), sepRegex, -1),
          std::sregex_token_iterator(),
          std::back_inserter(containerList) );
    bool doThin = !containerList.empty();

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
      std::unique_ptr<EleLinkThinningHelpers::ThinningHelperBase> tmp;
      tmp.reset(m_helper.release() );
      m_helper = EleLinkThinningHelpers::createHelper(
          EleLinkThinningHelpers::TypeList::types, *ti,
          auxName, containerList, op, doThin, maxToSee, std::move(tmp) );
    }
    catch (const FunctorHelpers::UnknownTypeException& e) {
      ATH_MSG_ERROR("Caught UnknownTypeException: " << e.what() );
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}
