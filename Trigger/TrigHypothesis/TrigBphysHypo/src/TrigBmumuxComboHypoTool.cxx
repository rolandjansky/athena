/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBmumuxComboHypoTool.h"


using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::DecisionIDContainer;

TrigBmumuxComboHypoTool::TrigBmumuxComboHypoTool(const std::string& type, const std::string& name, const IInterface* parent)
    : ComboHypoToolBase(type, name, parent) {}


StatusCode TrigBmumuxComboHypoTool::initialize()
{
  if (!m_monTool.empty()) {
    ATH_CHECK( m_monTool.retrieve() );
    ATH_MSG_DEBUG( "GenericMonitoringTool name:" << m_monTool );
  }
  else {
    ATH_MSG_DEBUG( "No GenericMonitoringTool configured: no monitoring histograms will be available" );
  }

  ATH_MSG_DEBUG( "Initialization completed successfully" );

  return StatusCode::SUCCESS;
}


bool TrigBmumuxComboHypoTool::passed(const xAOD::TrigBphys* trigBphys) const {

  using namespace Monitored;

  auto mon_chi2 = Monitored::Scalar<float>("Chi2", -1.);
  auto mon_fitmass = Monitored::Scalar<float>("Fitmass", -1.);
  auto mon_mass = Monitored::Scalar<float>("Mass", -1.);
  auto mon_pt = Monitored::Scalar<float>("Pt", -1.);
  auto group = Monitored::Group(m_monTool, mon_chi2, mon_fitmass, mon_mass, mon_pt);

  ATH_MSG_DEBUG( "in TrigBmumuxComboHypoTool::decideOnSingleObject(), looking at TrigBphys object");

  bool result = false;

  if (static_cast<int>(trigBphys->particleType()) == m_decay) {
    result = true;
    ATH_MSG_DEBUG("accepting event");
  }

  if (result) {
    mon_chi2 = trigBphys->fitchi2();
    mon_fitmass = trigBphys->fitmass();
    mon_mass = trigBphys->mass();
    mon_pt = trigBphys->pt();
  }

  return result;
}


StatusCode TrigBmumuxComboHypoTool::decideOnSingleObject(Decision* decision, const std::vector<const DecisionIDContainer*>& previousDecisionIDs) const {

  ATH_CHECK( decision->hasObjectLink(TrigCompositeUtils::featureString()) );

  auto trigBphysEL = decision->objectLink<xAOD::TrigBphysContainer>(TrigCompositeUtils::featureString());
  ATH_CHECK( trigBphysEL.isValid() );

  ATH_CHECK( previousDecisionIDs.size() == 2 );

  const std::vector<HLT::Identifier>& legDecisionIDs = legDecisionIds();
  if (legDecisionIDs.size() == 1 && legMultiplicity().at(0) >= 2) {  // trigger with symmetric legs
    const DecisionID id = legDecisionIDs[0].numeric();
    if (!TrigCompositeUtils::passed(id, *previousDecisionIDs[0]) || !TrigCompositeUtils::passed(id, *previousDecisionIDs[1])) {
      ATH_MSG_DEBUG( "Trigger with symmetric legs did not match to the previous decisions" );
      return StatusCode::SUCCESS;
    }
  }
  else if (legDecisionIDs.size() == 2) {  // trigger with asymmetric legs
    bool direct = true;
    bool inverse = true;
    for (size_t i = 0; i < 2; ++i) {
      if (direct && !TrigCompositeUtils::passed(legDecisionIDs.at(i).numeric(), *previousDecisionIDs.at(i))) direct = false;
      if (inverse && !TrigCompositeUtils::passed(legDecisionIDs.at(i).numeric(), *previousDecisionIDs.at(1-i))) inverse = false;
    }
    if (!direct && !inverse) {
      ATH_MSG_DEBUG( "Trigger with asymmetric legs matched to the previous decisions neither direct nor inverse way" );
      return StatusCode::SUCCESS;
    }
  }
  else {
    ATH_MSG_ERROR( "TrigBmumuxComboHypoTool can not check previous decisions for " << name() );
    return StatusCode::FAILURE;
  }

  if (passed(*trigBphysEL)) {
    TrigCompositeUtils::addDecisionID(decisionId(), decision);
  }

  return StatusCode::SUCCESS;
}


bool TrigBmumuxComboHypoTool::executeAlg(const std::vector<Combo::LegDecision>&) const {
  ATH_MSG_ERROR("executeAlg not supported for TrigBmumuxComboHypoTool.");
  return true;
}
