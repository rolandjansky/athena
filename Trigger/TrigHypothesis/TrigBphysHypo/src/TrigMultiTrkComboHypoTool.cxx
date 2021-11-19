/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBPhysHypo/TrigMultiTrkHypoTool.cxx
 **
 **   Description: multi-track hypothesis tool for bphys triggers
 **
 **   Author: Heather Russell
 **
 **************************************************************************/

#include "TrigMultiTrkComboHypoTool.h"

#include <numeric>
#include <vector>
#include <iterator>
#include <algorithm>
#include "Math/GenVector/VectorUtil.h"

using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionIDContainer;


TrigMultiTrkComboHypoTool::TrigMultiTrkComboHypoTool(const std::string& type, const std::string& name, const IInterface* parent)
    : ComboHypoToolBase(type, name, parent) {}


StatusCode TrigMultiTrkComboHypoTool::initialize() {
  ATH_MSG_DEBUG( "configuration for '" << this->name() << "'" << endmsg <<
                 "   AcceptAll = " << (m_acceptAll ? "True" : "False") << endmsg <<
                 "   isCombinedChain = "  << (m_isCombinedChain ? "True" : "False") << endmsg <<
                 "   isMergedElectronChain = "  << (m_isMergedElectronChain ? "True" : "False") << endmsg <<
                 "   mass range: ( " <<  m_massRange.value().first << ", " << m_massRange.value().second << " )" << endmsg <<
                 "   chi2 cut: " << m_chi2 << endmsg <<
                 "   " << (m_totalCharge < 0 ? "total charge cut is disabled" : "total charge cut: only right charge combinations") << endmsg <<
                 "   LxyCut: > " << m_LxyCut.value() );

  ATH_CHECK( m_nTrk >= 2 );
  ATH_CHECK( !(m_isMergedElectronChain && m_legMultiplicities[0] != 1) );

  ATH_CHECK( !m_legMultiplicities.empty() );
  if ( m_legMultiplicities.size() > 1 ) {  // chain with asymmetric legs, as HLT_mu6_2mu4_bDimu_L1MU6_3MU4
    // here we consider each decision as an individual object, i.e. for HLT_mu6_2mu4_bDimu_L1MU6_3MU4 we use
    // m_legDecisionIDs = {createLegName("HLT_mu6_2mu4_bDimu_L1MU6_3MU4", 0),
    //                     createLegName("HLT_mu6_2mu4_bDimu_L1MU6_3MU4", 1),
    //                     createLegName("HLT_mu6_2mu4_bDimu_L1MU6_3MU4", 1)};
    for (size_t legIndex = 0; legIndex < m_legMultiplicities.size(); ++legIndex) {
      // can not use legDecisionId(legIndex) at initialize() step, use TrigCompositeUtils::createLegName() instead
      auto legId = TrigCompositeUtils::createLegName(decisionId(), legIndex);
      m_legDecisionIDs.insert(m_legDecisionIDs.end(), m_legMultiplicities[legIndex], legId.numeric());
    }
  }
  else {  // chain with symmetric legs, as HLT_2mu4_bDimu_L12MU4
    m_legDecisionIDs.insert(m_legDecisionIDs.end(), m_legMultiplicities[0], decisionId().numeric());
  }
  ATH_CHECK( m_nTrk <= m_legDecisionIDs.size() || m_isMergedElectronChain || m_isMuonTrkPEB );

  if (!m_monTool.empty()) {
    ATH_CHECK( m_monTool.retrieve() );
    ATH_MSG_DEBUG( "GenericMonitoringTool name:" << m_monTool );
  }
  else {
    ATH_MSG_DEBUG( "No GenericMonitoringTool configured: no monitoring histograms will be available" );
  }

  return StatusCode::SUCCESS;
}


bool TrigMultiTrkComboHypoTool::passed(const xAOD::TrigBphys* trigBphys) const {
  ATH_MSG_DEBUG( "TrigMultiTrkComboHypoTool::passed()" );

  auto mon_totalCharge = Monitored::Scalar<int>("totalCharge", -100);
  auto mon_chi2 = Monitored::Scalar<float>("chi2", -1.);
  auto mon_mass = Monitored::Scalar<float>("mass", -1.);
  auto mon_pT_trk1 = Monitored::Scalar<float>("pT_trk1", -1.);
  auto mon_pT_trk2 = Monitored::Scalar<float>("pT_trk2", -1.);
  auto mon_D0_trk1 = Monitored::Scalar<float>("d0_trk1", -1.);
  auto mon_D0_trk2 = Monitored::Scalar<float>("d0_trk2", -1.);
  auto mon_Lxy  = Monitored::Scalar<float>("Lxy", -1.);
  auto mon_Eta1  = Monitored::Scalar<float>("eta_trk1", -100.);
  auto mon_Eta2  = Monitored::Scalar<float>("eta_trk2", -100.);

  auto mon = Monitored::Group( m_monTool, mon_totalCharge, mon_chi2, mon_mass, mon_pT_trk1, mon_pT_trk2, mon_Lxy, mon_D0_trk1, mon_D0_trk2, mon_Eta1, mon_Eta2);

  if (m_acceptAll || (trigBphys->nTrackParticles() == m_nTrk &&
                      isInMassRange(trigBphys->mass()) &&
                      passedChi2Cut(trigBphys->fitchi2()) &&
                      passedChargeCut(totalCharge(trigBphys)) &&
                      trigBphys->lxy() > m_LxyCut &&
                      passedDeltaRcut(trigBphys))) {
    mon_Lxy = trigBphys->lxy();
    mon_totalCharge = totalCharge(trigBphys);
    mon_chi2 = trigBphys->fitchi2();
    mon_mass = 0.001 * trigBphys->mass();
    mon_pT_trk1 = 0.001 * trigBphys->trackParticle(0)->pt();
    mon_pT_trk2 = 0.001 * trigBphys->trackParticle(1)->pt();
    mon_D0_trk1 =  trigBphys->trackParticle(0)->d0();
    mon_D0_trk2 =  trigBphys->trackParticle(1)->d0();
    mon_Eta1 =  trigBphys->trackParticle(0)->eta();
    mon_Eta2 =  trigBphys->trackParticle(1)->eta();
    ATH_MSG_DEBUG( "accepting event" );
    return true;
  }

  ATH_MSG_DEBUG( "reject event" );
  return false;
}


StatusCode TrigMultiTrkComboHypoTool::decideOnSingleObject(Decision* decision, const std::vector<const DecisionIDContainer*>& previousDecisionIDs) const {
  ATH_MSG_DEBUG( "decideOnSingleObject trigger tool: " << decisionId() );
  ATH_CHECK( decision->hasObjectLink(TrigCompositeUtils::featureString()) );

  auto trigBphysEL = decision->objectLink<xAOD::TrigBphysContainer>(TrigCompositeUtils::featureString());
  ATH_CHECK( trigBphysEL.isValid() );

  if (previousDecisionIDs.size() != (m_isMergedElectronChain || m_isMuonTrkPEB ? 1 : m_nTrk.value())) {
    return StatusCode::SUCCESS;
  }

  if (!checkPreviousDecisionIDs(previousDecisionIDs)) {
    return StatusCode::SUCCESS;
  }

  if (m_acceptAll || passed(*trigBphysEL)) {
    TrigCompositeUtils::addDecisionID(decisionId(), decision);
    if (m_isCombinedChain) {
      for (size_t i = 0; i < m_nTrk; ++i) {
        TrigCompositeUtils::addDecisionID(m_legDecisionIDs[i], decision);
      }
    }
  }

  return StatusCode::SUCCESS;
}


bool TrigMultiTrkComboHypoTool::checkPreviousDecisionIDs(const std::vector<const DecisionIDContainer*>& previousDecisionIDs) const {

  // the default ComboHypoTool is used at the previous step (muEFCB) to check the number of muons is sufficient to fire the BLS trigger
  // now we should only check the muons fitted to the common vertex are marked as passed the corresponding trigger
  // trigger with asymmetric legs (like HLT_mu6_2mu4_bDimu_L1MU6_3MU4) is treated in a specific way:
  // all 6 possible combinations should be checked: {leg0, leg1}, {leg0, leg2}, {leg1, leg0}, {leg1, leg2}, {leg2, leg0}, {leg2, leg1}

  if (m_isMergedElectronChain || m_isMuonTrkPEB) {
    if (!TrigCompositeUtils::passed(m_legDecisionIDs.at(0), *previousDecisionIDs[0])) {
      ATH_MSG_DEBUG( "Trigger for " << (m_isMergedElectronChain ? "close-by electrons" : "muon+track") << " didn't pass previous decision" );
      return false;
    }
    return true;
  }
  else if (decisionId() == legDecisionId(0)) {  // trigger with symmetric legs like HLT_3mu6_bDimu_L13MU6
    for (size_t i = 0; i < m_nTrk; ++i) {
      if (!TrigCompositeUtils::passed(decisionId().numeric(), *previousDecisionIDs[i])) {
        ATH_MSG_DEBUG( "Trigger with symmetric legs didn't pass previous decision" );
        return false;
      }
    }
    ATH_MSG_DEBUG( "Trigger with symmetric legs passed previous decision" );
    return true;
  }
  else {  // trigger with asymmetric legs like HLT_mu6_2mu4_bDimu_L1MU6_3MU4

    std::vector<size_t> a(m_legDecisionIDs.size());
    std::iota(a.begin(), a.end(), 0);  // {0, 1, 2, .., m_legDecisionIDs().size() - 1}
    int i = 1;
    bool result = true;
    do {
      result = true;
      for (size_t k = 0; k < m_nTrk; ++k) {
        result = result && TrigCompositeUtils::passed(m_legDecisionIDs.at(a[k]), *previousDecisionIDs[k]);
      }
      if (msgLvl(MSG::DEBUG)) {
        msg() << "combination #" << i++ << ": { ";
        std::copy(a.begin(), a.begin() + m_nTrk, std::ostream_iterator<int>(msg().stream(), " "));
        msg() << "} " << (result ? " passed" : "didn't pass") << endmsg;
      }
      if (result) break;
    } while (std::next_permutation(a.begin(), a.end()));
    ATH_MSG_DEBUG( "Trigger with asymmetric legs " << (result ? "passed" : "didn't pass") << " previous decision" );
    return result;
  }
  return true;
}


bool TrigMultiTrkComboHypoTool::isInMassRange(double mass) const {

  const auto& range = m_massRange.value();
  if ( range.first > 0. && mass < range.first ) return false;
  if ( range.second > 0. && mass > range.second ) return false;
  return true;
}

bool TrigMultiTrkComboHypoTool::passedDeltaRcut(const xAOD::TrigBphys* trigBphys) const {
  if (m_deltaRMax == std::numeric_limits<float>::max() && m_deltaRMin == std::numeric_limits<float>::lowest()) { // Cut disabled
    return true;
  }
  size_t N = trigBphys->nTrackParticles();
  for (size_t i = 0 ; i < N; i++) {
    auto p1 = trigBphys->trackParticle(i)->genvecP4();
    for (size_t j = i + 1; j < N; j++) {
      auto p2 = trigBphys->trackParticle(j)->genvecP4();
      double deltaR = ROOT::Math::VectorUtil::DeltaR(p1, p2);
      if (deltaR > m_deltaRMax || deltaR < m_deltaRMin) return false;
    }
  }
  return true;
}

int TrigMultiTrkComboHypoTool::totalCharge(const xAOD::TrigBphys* trigBphys) const {

  int charge = 0;
  for (size_t i = 0; i < trigBphys->nTrackParticles(); ++i) {
    charge += static_cast<int>(trigBphys->trackParticle(i)->charge());
  }
  return charge;
}


bool TrigMultiTrkComboHypoTool::executeAlg(const std::vector<Combo::LegDecision>&) const {
  ATH_MSG_ERROR( "executeAlg() is not supported by TrigMultiTrkComboHypoTool" );
  return true;
}
