/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include <cmath>


TrigMultiTrkComboHypoTool::TrigMultiTrkComboHypoTool(const std::string& type, const std::string& name, const IInterface* parent)
    : ComboHypoToolBase(type, name, parent) {}


StatusCode TrigMultiTrkComboHypoTool::initialize()
{
  ATH_MSG_DEBUG("AcceptAll            = " << (m_acceptAll==true ? "True" : "False") );
  if(m_TotChargeCut >=0)ATH_MSG_DEBUG("Total Charge Cut     = " << m_TotChargeCut);
  else ATH_MSG_DEBUG("Total Charge Cut is disabled");
  ATH_MSG_DEBUG("LowerMassCut         = " << m_lowerMassCut );
  ATH_MSG_DEBUG("UpperMassCut         = " << m_upperMassCut );
  ATH_MSG_DEBUG("ApplyUpperMassCut    = " << m_applyUpperMassCut );
  ATH_MSG_DEBUG("ApplyChi2Cut         = " << m_applyChi2Cut );
  ATH_MSG_DEBUG("Chi2VtxCut           = " << m_chi2VtxCut );

  if ( not m_monTool.name().empty() ) {
        ATH_CHECK( m_monTool.retrieve() );
        ATH_MSG_DEBUG("m_monTool name: " << m_monTool);
  }
  if(static_cast<int>(m_ptTrkMin.size()) != m_nTrk){
      ATH_MSG_ERROR("Requested " << m_nTrk << " tracks per vertex, but only provided "
            << m_ptTrkMin.size() << " track pTs!");
      return StatusCode::FAILURE;
  } else if(msgLvl(MSG::DEBUG)){
      msg() << MSG::DEBUG <<  "requiring " << m_nTrk << " tracks with pT: ";
      for(float pt :  m_ptTrkMin) msg() << MSG::INFO << pt <<", ";
      msg() << MSG::DEBUG << endmsg;
  }

  ATH_MSG_DEBUG("Initialization completed successfully");

  return StatusCode::SUCCESS;
}


bool TrigMultiTrkComboHypoTool::passed(const xAOD::TrigBphys* trigBphys) const {

  using namespace Monitored;

  ATH_MSG_DEBUG( "in TrigMultiTrkHypoTool::decideOnSingleObject(), looking at TrigBphys object");

  bool thisPassedMassCut = false;
  bool thisPassedChi2Cut = false;
  bool thisPassedTrkPtCut = false;
  bool thisPassedChargeCut = false;
  bool result = false;

  std::vector<float> ini_trkPts(0);

  auto mon_cutCounter = Monitored::Scalar<int>("CutCounter",-1);
  auto mon_fitChi2    = Monitored::Scalar<float>("FitChi2",-1.);
  auto mon_vtxMass   = Monitored::Scalar<float>("VertexMass",-1.);
  auto mon_trkPts     = Monitored::Collection("trackPts", ini_trkPts);
  auto mon_totCharge  = Monitored::Scalar<int>("totCharge", -9);
  auto mon = Monitored::Group( m_monTool, mon_cutCounter, mon_fitChi2, mon_vtxMass, mon_trkPts, mon_totCharge);

  mon_cutCounter = 0;

  if (trigBphys->particleType() == xAOD::TrigBphys::MULTIMU ) {

      ATH_MSG_DEBUG("Got Bphys particle with mass " << trigBphys->mass() << " chi2 :  " << trigBphys->fitchi2() );
      float vertexMass = trigBphys->mass();
      thisPassedMassCut = (m_lowerMassCut < vertexMass && ((vertexMass < m_upperMassCut) || (!m_applyUpperMassCut) ));
      thisPassedChi2Cut = ((!m_applyChi2Cut) || (trigBphys->fitchi2() < m_chi2VtxCut && trigBphys->fitchi2() >= -1e-10) );
      if(thisPassedMassCut){ ATH_MSG_DEBUG("Passed vertex mass cut (mass = " << vertexMass << " GeV)");}
      mon_vtxMass = ((vertexMass*0.001));
      if(thisPassedChi2Cut){
            ATH_MSG_DEBUG("Apply chi2 cut : " << m_applyChi2Cut << " chi2 :  " << trigBphys->fitchi2() << " passed Chi2 cut < "<< m_chi2VtxCut );
      }
      mon_fitChi2 = trigBphys->fitchi2();
      unsigned int nTrk =trigBphys->nTrackParticles();
      if( nTrk != m_ptTrkMin.size() ){
        ATH_MSG_ERROR("Vertex has " << nTrk << " tracks trying to pass " << m_ptTrkMin.size() << " track thresholds!");
        return false;
      }
      //std::vector<bool> tracksPass; tracksPass.assign(nTrk, false);
      int totq = 0;
      std::vector<float> trackPts;
      for(unsigned int i=0; i < nTrk; i++ ){
          const auto tp = trigBphys->trackParticle(i);
          ATH_MSG_DEBUG("Track pt/eta/phi/charge: " <<
            trigBphys->trackParticle(i)->pt() << ", " <<
            trigBphys->trackParticle(i)->eta() << ", " <<
            trigBphys->trackParticle(i)->phi() << ", " <<
            trigBphys->trackParticle(i)->charge());
          totq += (tp->qOverP() > 0) ? 1 : ((tp->qOverP() < 0) ? -1 : 0);
          trackPts.push_back(tp->pt());
      }
      mon_totCharge = totq;
      thisPassedChargeCut = m_TotChargeCut <= -1 || (std::abs(totq) == m_TotChargeCut);
      if(thisPassedChargeCut){
      	ATH_MSG_DEBUG("Passed charge cut with " << totq);
      }
      std::sort(trackPts.rbegin(), trackPts.rend()); //reverse sort the list
      unsigned int nTrk_pass = 0;
      for(unsigned int i=0; i < nTrk; i++ ){
          ini_trkPts.push_back(trackPts.at(i)*0.001);
          if(trackPts.at(i) > m_ptTrkMin[i]) nTrk_pass++;
          ATH_MSG_DEBUG("track pt " << trackPts.at(i) << " >? " << m_ptTrkMin[i]);
      }
      ATH_MSG_DEBUG("Passed " << nTrk_pass << " tracks, out of " << nTrk << " tracks");
      if(nTrk_pass == nTrk){
          thisPassedTrkPtCut = true;
      }
      if(!thisPassedMassCut && !thisPassedChi2Cut && !thisPassedTrkPtCut){
          ATH_MSG_DEBUG("Did not pass mass & chi2 & trk pT cuts ");
      }

  }

  if ( thisPassedMassCut ) {
    mon_cutCounter++;
    if ( thisPassedChi2Cut ) {
      mon_cutCounter++;
      if ( thisPassedTrkPtCut ){
        mon_cutCounter++;
        if(thisPassedChargeCut){
           mon_cutCounter++;
        }
      }
    }
  }
  if ( thisPassedMassCut & thisPassedChi2Cut & thisPassedTrkPtCut & thisPassedChargeCut) {
    result = true;
    ATH_MSG_DEBUG("accepting event");
  }

  ATH_MSG_DEBUG("AcceptAll is set to : " << (m_acceptAll ? "True, taking all events " : "False, applying selection" ));
  // Accept-All mode: temporary patch; should be done with force-accept
  if (m_acceptAll) {
      return true;
  }

  return result;
}


StatusCode TrigMultiTrkComboHypoTool::decideOnSingleObject(Decision* decision, const std::vector<DecisionIDContainer*>& previousDecisionIDs) const {

  ATH_CHECK( decision->hasObjectLink(TrigCompositeUtils::featureString()) );

  auto trigBphysEL = decision->objectLink<xAOD::TrigBphysContainer>(TrigCompositeUtils::featureString());
  ATH_CHECK( trigBphysEL.isValid() );

  ATH_CHECK( previousDecisionIDs.size() == legDecisionIds().size() );
  for (size_t i = 0; i < previousDecisionIDs.size(); ++i) {
    if (!TrigCompositeUtils::passed(legDecisionId(i).numeric(), *previousDecisionIDs[i])) {
      ATH_MSG_DEBUG( "Didn't pass previous decision: " << legDecisionId(i) );
      return StatusCode::SUCCESS;
    }
  }

  if (passed(*trigBphysEL)) {
    TrigCompositeUtils::addDecisionID(decisionId(), decision);
  }

  return StatusCode::SUCCESS;
}
