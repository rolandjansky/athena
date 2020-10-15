/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "L2OverlapRemoverMonMT.h"

#include "MuonMatchingTool.h"

L2OverlapRemoverMonMT :: L2OverlapRemoverMonMT(const std::string& name, ISvcLocator* pSvcLocator )
  : TrigMuonMonitorAlgorithm(name, pSvcLocator)
{}


StatusCode L2OverlapRemoverMonMT :: fillVariablesPerChain(const EventContext &ctx, const std::string &chain) const {

  ATH_MSG_DEBUG ("Filling histograms for " << name() << "...");
  ATH_CHECK( fillVariablesOverlapRemoverPlots<xAOD::L2StandAloneMuon>(chain, "L2SA") );
  ATH_CHECK( fillVariablesOverlapRemoverPlots<xAOD::L2CombinedMuon>(chain, "L2CB") );

  return StatusCode::SUCCESS;
}


bool L2OverlapRemoverMonMT :: isOverlap(const std::string &chain, const ElementLink<xAOD::L2StandAloneMuonContainer> muEL1, const ElementLink<xAOD::L2StandAloneMuonContainer> muEL2) const {

  const float ZERO_LIMIT = 0.00001;

  auto dR           = Monitored::Scalar<float>("L2SA_"+chain+"_dR", -999.);
  auto invMass      = Monitored::Scalar<float>("L2SA_"+chain+"_invMass", -999.);
  auto dRLog10      = Monitored::Scalar<float>("L2SA_"+chain+"_dRLog10", -999.);
  auto invMassLog10 = Monitored::Scalar<float>("L2SA_"+chain+"_invMassLog10", -999.);

  const auto [mu1Pt, mu1Eta, mu1Phi] = L2ORPosForMatchFunc((*muEL1));
  const auto [mu2Pt, mu2Eta, mu2Phi] = L2ORPosForMatchFunc((*muEL2));

  if( ( (std::abs(mu1Eta) < ZERO_LIMIT) && (std::abs(mu1Phi) < ZERO_LIMIT) ) || 
      ( (std::abs(mu2Eta) < ZERO_LIMIT) && (std::abs(mu2Phi) < ZERO_LIMIT) ) || 
      (std::abs(mu1Pt) < ZERO_LIMIT) || (std::abs(mu2Pt) < ZERO_LIMIT) ) return false;

  double dRThres   = 9999.;
  double massThres = 9999.;
  bool isBarrel1 = (*muEL1)->sAddress() != -1 ? true : false;
  bool isBarrel2 = (*muEL2)->sAddress() != -1 ? true : false;

  if(  isBarrel1 && isBarrel2 ) { // BB
    dRThres  =m_dRSAThresBB;
    massThres=m_massSAThresBB;
  }
  else if( (isBarrel1 && ! isBarrel2) || (!isBarrel1 && isBarrel2) ) { // BE
    dRThres  =m_dRSAThresBE;
    massThres=m_massSAThresBE;
  }
  else { // EE
    double absEta = (std::abs(mu1Pt) > std::abs(mu2Pt)) ? std::abs(mu1Eta) : std::abs(mu2Eta);
    unsigned int iThres=0;
    for(unsigned int i=0; i<(m_etaBins.size()-1); i++) {
      if ( m_etaBins[i] <= absEta && absEta < m_etaBins[i+1] ) iThres = i;
    }
    dRThres   = m_dRSAThresEC[iThres];
    massThres = m_massSAThresEC[iThres];
  }


  // same sign cut
  bool sameSign = ( mu1Pt*mu2Pt > 0) ? true : false;

  // dR cut
  float deta = mu1Eta - mu2Eta;
  float dphi = xAOD::P4Helpers::deltaPhi(mu1Phi, mu2Phi);
  dR = sqrt(deta*deta + dphi*dphi);
  bool dRisClose = ( dR < dRThres ) ? true : false;

  // mass cut
  invMass = calcinvMass(0., mu1Pt, mu1Eta, mu1Phi, 0., mu2Pt, mu2Eta, mu2Phi);
  bool massIsClose = ( invMass < massThres ) ? true : false;

  // for monitorinng log10 plot
  const float monitor_limit = 1e-4;
  dRLog10 = ( dR >= monitor_limit ) ? log10(dR) : log10(monitor_limit);
  invMassLog10 = ( invMass >= monitor_limit ) ? log10(invMass) : log10(monitor_limit);


  // total judge
  bool overlap = ( sameSign && dRisClose && massIsClose ) ? true : false;
  ATH_MSG_DEBUG( "   ...=> isOverlap=" << overlap );

  fill(m_group+"_"+chain, dR, invMass, dRLog10, invMassLog10);

  return overlap;
}


bool L2OverlapRemoverMonMT :: isOverlap(const std::string &chain, const ElementLink<xAOD::L2CombinedMuonContainer> muEL1, const ElementLink<xAOD::L2CombinedMuonContainer> muEL2) const {

  const float ZERO_LIMIT = 0.00001;

  auto dR           = Monitored::Scalar<float>("L2CB_"+chain+"_dR", -999.);
  auto invMass      = Monitored::Scalar<float>("L2CB_"+chain+"_invMass", -999.);
  auto dRLog10      = Monitored::Scalar<float>("L2CB_"+chain+"_dRLog10", -999.);
  auto invMassLog10 = Monitored::Scalar<float>("L2CB_"+chain+"_invMassLog10", -999.);

  const auto [mu1Pt, mu1Eta, mu1Phi] = L2ORPosForMatchFunc((*muEL1));
  const auto [mu2Pt, mu2Eta, mu2Phi] = L2ORPosForMatchFunc((*muEL2));

  if( ( (std::abs(mu1Eta) < ZERO_LIMIT) && (std::abs(mu1Phi) < ZERO_LIMIT) ) || 
      ( (std::abs(mu2Eta) < ZERO_LIMIT) && (std::abs(mu2Phi) < ZERO_LIMIT) ) || 
      (std::abs(mu1Pt) < ZERO_LIMIT) || (std::abs(mu2Pt) < ZERO_LIMIT) ) return false;

  double absEta = (std::abs(mu1Pt) > std::abs(mu2Pt)) ? std::abs(mu1Eta) : std::abs(mu2Eta);
  unsigned int iThres=0;
  for(unsigned int i=0; i<(m_etaBins.size()-1); i++) {
    if ( m_etaBins[i] <= absEta && absEta < m_etaBins[i+1] ) iThres = i;
  }
  float dRThres   = m_dRCBThres[iThres];
  float dRbySAThres = m_dRbySAThres[iThres];
  float massThres = m_massCBThres[iThres];


  // same sign cut
  bool sameSign = ( mu1Pt*mu2Pt > 0) ? true : false;

  // dR cut
  float deta = mu1Eta - mu2Eta;
  float dphi = xAOD::P4Helpers::deltaPhi(mu1Phi, mu2Phi);
  dR = sqrt(deta*deta + dphi*dphi);
  bool dRisClose = ( dR < dRThres ) ? true : false;

  // dR(by L2SA) cut
  bool dRbySAisClose = false;
  const xAOD::L2StandAloneMuon* muSA1 = (*muEL1)->muSATrack();
  const xAOD::L2StandAloneMuon* muSA2 = (*muEL2)->muSATrack();
  if( muSA1 == 0 || muSA2 == 0 ) return false;
  else {
    float deta = muSA1->etaMS() - muSA2->etaMS();
    float dphi = xAOD::P4Helpers::deltaPhi(muSA1->phiMS(), muSA2->phiMS());
    float dRBySA = sqrt(deta*deta + dphi*dphi);
    if( dRBySA < dRbySAThres ) dRbySAisClose = true;
  }

  // mass cut
  invMass = calcinvMass(0., mu1Pt, mu1Eta, mu1Phi, 0., mu2Pt, mu2Eta, mu2Phi);
  bool massIsClose = ( invMass < massThres ) ? true : false;

  // for monitorinng log10 plot
  const float monitor_limit = 1e-4;
  dRLog10 = ( dR >= monitor_limit ) ? log10(dR) : log10(monitor_limit);
  invMassLog10 = ( invMass >= monitor_limit ) ? log10(invMass) : log10(monitor_limit);


  // total judge
  bool overlap = ( sameSign && dRisClose && massIsClose && dRbySAisClose ) ? true : false;
  ATH_MSG_DEBUG( "   ...=> isOverlap=" << overlap );

  fill(m_group+"_"+chain, dR, invMass, dRLog10, invMassLog10);

  return overlap;
}


StatusCode L2OverlapRemoverMonMT::chooseBestMuon(const std::string &chain, std::vector< TrigCompositeUtils::LinkInfo<xAOD::L2StandAloneMuonContainer> > featureCont, std::vector<unsigned int> muResult) const
{
  unsigned int i,j,k;

  std::vector<float> vec_RemovedEta, vec_RemovedPhi, vec_RemovedPt;
  auto RemovedEta = Monitored::Collection("L2SA_"+chain+"_RemovedEta", vec_RemovedEta);
  auto RemovedPhi = Monitored::Collection("L2SA_"+chain+"_RemovedPhi", vec_RemovedPhi);
  auto RemovedPt  = Monitored::Collection("L2SA_"+chain+"_RemovedPt", vec_RemovedPt);

  for(i=0; i<featureCont.size(); i++) {
    ATH_MSG_DEBUG( "++ i=" << i << ": result=" << muResult[i] );
    if( muResult[i] != i ) {
      ATH_MSG_DEBUG( "   overlap to some one. skip." );
      continue;
    }
    std::vector<unsigned int> others;
    for(j=0; j<featureCont.size(); j++) {
      if( muResult[j] == muResult[i] ) others.emplace_back(j);
    }
    if( others.size() == 1 ) {
      ATH_MSG_DEBUG( "   unique object. keep it active." );
      continue;
    }
    else { // must choose one best
      ATH_MSG_DEBUG( "   overlapped objects among: " << others );
      unsigned int BestMuon = 0;
      float maxPt  = 0.;
      float maxPtRoI = 0.;
      for(k=0; k<others.size(); k++) {
        j=others[k];
        const ElementLink<xAOD::L2StandAloneMuonContainer> muEL = featureCont[j].link;
        float pt  = std::abs((*muEL)->pt());
        float ptRoI = (*muEL)->roiThreshold();
        if( (ptRoI-maxPtRoI) > 0.1 ) {
          maxPtRoI = ptRoI;
          maxPt  = pt;
          BestMuon = j;
        }
        else if( std::abs(ptRoI-maxPtRoI) < 0.1 ) {
          if( pt > maxPt ) {
            maxPtRoI = ptRoI;
            maxPt  = pt;
            BestMuon = j;
          }
        }
      }

      for(k=0; k<others.size(); k++) {
        j=others[k];
        if( j != BestMuon ) { // removed muon
          const ElementLink<xAOD::L2StandAloneMuonContainer> muEL = featureCont[j].link;
          vec_RemovedPt.push_back( (*muEL)->pt() );
          vec_RemovedEta.push_back( (*muEL)->etaMS() );
          vec_RemovedPhi.push_back( (*muEL)->phiMS() );
        }
      }
    }
  }

  fill(m_group+"_"+chain, RemovedEta, RemovedPhi, RemovedPt);

  return StatusCode::SUCCESS;
}

StatusCode L2OverlapRemoverMonMT::chooseBestMuon(const std::string &chain, std::vector< TrigCompositeUtils::LinkInfo<xAOD::L2CombinedMuonContainer> > featureCont, std::vector<unsigned int> muResult) const
{
  unsigned int i,j,k;

  std::vector<float> vec_RemovedEta, vec_RemovedPhi, vec_RemovedPt;
  auto RemovedEta = Monitored::Collection("L2CB_"+chain+"_RemovedEta", vec_RemovedEta);
  auto RemovedPhi = Monitored::Collection("L2CB_"+chain+"_RemovedPhi", vec_RemovedPhi);
  auto RemovedPt  = Monitored::Collection("L2CB_"+chain+"_RemovedPt", vec_RemovedPt);

  for(i=0; i<featureCont.size(); i++) {
    ATH_MSG_DEBUG( "++ i=" << i << ": result=" << muResult[i] );
    if( muResult[i] != i ) {
      ATH_MSG_DEBUG( "   overlap to some one. skip." );
      continue;
    }
    std::vector<unsigned int> others;
    for(j=0; j<featureCont.size(); j++) {
      if( muResult[j] == muResult[i] ) others.emplace_back(j);
    }
    if( others.size() == 1 ) {
      ATH_MSG_DEBUG( "   unique object. keep it active." );
      continue;
    }
    else { // must choose one best
      ATH_MSG_DEBUG( "   overlapped objects among: " << others );
      unsigned int BestMuon = 0;
      float maxPt  = 0.;
      for(k=0; k<others.size(); k++) {
        j=others[k];
        const ElementLink<xAOD::L2CombinedMuonContainer> muEL = featureCont[j].link;
        float pt  = std::abs((*muEL)->pt()/1e3);
        if( pt > maxPt ) {
          maxPt  = pt;
          BestMuon = j;
        }
      }

      for(k=0; k<others.size(); k++) {
        j=others[k];
        if( j != BestMuon ) { // removed muon
          const ElementLink<xAOD::L2CombinedMuonContainer> muEL = featureCont[j].link;
          vec_RemovedPt.push_back( (*muEL)->pt()/1e3 * (*muEL)->charge() );
          vec_RemovedEta.push_back( (*muEL)->eta() );
          vec_RemovedPhi.push_back( (*muEL)->phi() );
        }
      }
    }
  }

  fill(m_group+"_"+chain, RemovedEta, RemovedPhi, RemovedPt);

  return StatusCode::SUCCESS;
}



float L2OverlapRemoverMonMT::calcinvMass(double m1, double pt1, double eta1, double phi1,
                                         double m2, double pt2, double eta2, double phi2) const
{
  const double ZERO_LIMIT = 1e-12;

  double theta1 = 2*atan2((double)exp(-eta1),1.);
  double theta2 = 2*atan2((double)exp(-eta2),1.);

  double fpt1   = fabs(pt1);
  double fpt2   = fabs(pt2);

  double px1    = fpt1*cos(phi1);
  double py1    = fpt1*sin(phi1);
  double pz1    = fpt1/tan(theta1);
  double  e1    = sqrt(px1*px1+py1*py1+pz1*pz1+m1*m1);

  double px2    = fpt2*cos(phi2);
  double py2    = fpt2*sin(phi2);
  double pz2    = fpt2/tan(theta2);
  double  e2    = sqrt(px2*px2+py2*py2+pz2*pz2+m2*m2);

  double pxsum  = px1 + px2;
  double pysum  = py1 + py2;
  double pzsum  = pz1 + pz2;
  double esum   =  e1 +  e2;

  double mass  = 0;
  double mass2 = esum*esum - pxsum*pxsum - pysum*pysum - pzsum*pzsum;
  if( mass2 > ZERO_LIMIT ) mass = sqrt(mass2);

  return mass;
}


std::tuple<float,float,float> L2OverlapRemoverMonMT :: L2ORPosForMatchFunc(const xAOD::L2StandAloneMuon *trig){
  return std::forward_as_tuple(trig->pt(), trig->etaMS(), trig->phiMS());
}

std::tuple<float,float,float> L2OverlapRemoverMonMT :: L2ORPosForMatchFunc(const xAOD::L2CombinedMuon *trig){
  return std::forward_as_tuple( (trig->pt()/1e3 * trig->charge() ), trig->eta(), trig->phi());
}
