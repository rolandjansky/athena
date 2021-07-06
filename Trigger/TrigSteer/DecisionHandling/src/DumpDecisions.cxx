/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
// DecisionHandling includes

#include "TrigCompositeUtils/HLTIdentifier.h"

// FrameWork includes
#include "Gaudi/Property.h"
#include "DumpDecisions.h"

///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////
DumpDecisions::DumpDecisions(const std::string &name,
                             ISvcLocator *pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator)
{
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode DumpDecisions::initialize()
{
  ATH_MSG_DEBUG("Initializing " << name() << "...");
  CHECK(m_decisionKeys.initialize());
  return StatusCode::SUCCESS;
}

StatusCode DumpDecisions::execute(const EventContext &ctx) const
{
  using namespace TrigCompositeUtils;

  for (auto key : m_decisionKeys)
  {
    auto decisionInput = SG::makeHandle(key, ctx);
    if (not decisionInput.isValid())
    {
      ATH_MSG_DEBUG("No decisions key " << key.key() << " skipping");
      continue;
    }
    ATH_MSG_DEBUG("Retrieved decision with the key " << key.key());
    for (auto d : *decisionInput)
    {
      DecisionIDContainer ids;
      TrigCompositeUtils::decisionIDs(d, ids);
      ATH_MSG_DEBUG("Decision object with " << ids.size() << " decisions");
      for (auto id : ids)
      {
        ATH_MSG_VERBOSE("Passing decision " << HLT::Identifier(id));
      }
    }
  }
  return StatusCode::SUCCESS;
}
