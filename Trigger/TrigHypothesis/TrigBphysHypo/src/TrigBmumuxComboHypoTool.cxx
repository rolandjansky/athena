/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigBmumuxComboHypoTool.h"


using TrigCompositeUtils::Decision;
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
  ATH_CHECK( previousDecisionIDs.size() == legDecisionIds().size() );
  bool direct = true;
  bool inverse = true;
  for (size_t i = 0; i < 2; ++i) {
    if (direct && !TrigCompositeUtils::passed(legDecisionId(i).numeric(), *previousDecisionIDs[i])) direct = false;
    if (inverse && !TrigCompositeUtils::passed(legDecisionId(i).numeric(), *previousDecisionIDs[1-i])) inverse = false;
  }
  if (!direct && !inverse) {
    ATH_MSG_DEBUG( "Trigger legs matched to the previous decisions neither direct nor inverse way" );
    return StatusCode::SUCCESS;
  }

  if (passed(*trigBphysEL)) {
    TrigCompositeUtils::addDecisionID(decisionId(), decision);
  }

  return StatusCode::SUCCESS;
}
