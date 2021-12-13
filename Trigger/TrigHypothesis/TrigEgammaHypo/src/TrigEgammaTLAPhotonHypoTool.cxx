/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigEgammaTLAPhotonHypoTool.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODTrigger/TrigCompositeContainer.h"

using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionIDContainer;
using TrigCompositeUtils::decisionIDs;

TrigEgammaTLAPhotonHypoTool::TrigEgammaTLAPhotonHypoTool(const std::string& type, const std::string& name, const IInterface* parent):
  AthAlgTool(type, name, parent),
  m_decisionId(HLT::Identifier::fromToolName(name))
  {}

TrigEgammaTLAPhotonHypoTool::~TrigEgammaTLAPhotonHypoTool() {}

StatusCode TrigEgammaTLAPhotonHypoTool::initialize()
{
  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaTLAPhotonHypoTool::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaTLAPhotonHypoTool::decide(TrigCompositeUtils::DecisionContainer* outputDecisions) const
{

    for (auto decision : *outputDecisions )
    {
      // creates container of Decision identifiers that will be filled with the previous decisions attached to the object
      DecisionIDContainer previousDecisionIDs;
      // not sure whtat this does
      const auto previousDecisionEL = TrigCompositeUtils::getLinkToPrevious(decision).at(0);
      decisionIDs(*previousDecisionEL, previousDecisionIDs);
      // if we are sure that there is only one decision added by the parent HypoAlg (it should be), then checking that count > 0
      // is equivalent to checking if the previous decision was positive-NO
      // checks that the decision corresponding to the chain name is positive? uff
      if (previousDecisionIDs.count( m_decisionId.numeric()) > 0)
      {
        TrigCompositeUtils::addDecisionID(getId().numeric(), decision );
      }
    }
  return StatusCode::SUCCESS;

}

const HLT::Identifier& TrigEgammaTLAPhotonHypoTool::getId() const{
   return m_decisionId;
}
