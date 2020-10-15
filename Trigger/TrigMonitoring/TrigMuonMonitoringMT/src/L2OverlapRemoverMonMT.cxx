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
  const float ZERO_LIMIT = 0.00001;

  auto NInMuon = Monitored::Scalar<int>(chain+"_NInMuon", -999.);
  auto NOutMuon = Monitored::Scalar<int>(chain+"_NOutMuon", -999.);
  auto NRemovedMuon = Monitored::Scalar<int>(chain+"_NRemovedMuon", -999.);

  std::vector< TrigCompositeUtils::LinkInfo<xAOD::L2StandAloneMuonContainer> > featureCont = getTrigDecisionTool()->features<xAOD::L2StandAloneMuonContainer>( chain, TrigDefs::includeFailedDecisions );
  NInMuon = featureCont.size();


  // Check overlap
  std::vector<int> vec_OverlapInsonsistent;
  auto OverlapInsonsistent = Monitored::Collection(chain+"_OverlapInsonsistent", vec_OverlapInsonsistent);

  unsigned int i,j;
  std::vector<unsigned int> muResult;
  bool errorWhenIdentifyingOverlap = false;

  for(i=0; i<featureCont.size(); i++) muResult.emplace_back(i);
  for(i=0; i<featureCont.size(); i++){
    ATH_CHECK( featureCont[i].isValid() );
    for(j=i+1; j<featureCont.size(); j++){
      ATH_CHECK( featureCont[j].isValid() );
      bool overlapped = isOverlap(chain, featureCont[i].link, featureCont[j].link, L2SAPosForMatchFunc);
      if( !overlapped ){ // judged as different
        if( muResult[i] == muResult[j] ) { // but marked as same by someone
          ATH_MSG_DEBUG( "inconsistentency in muFast overlap removal for more than two objects" );
          ATH_MSG_DEBUG( "two objects are judged as different but both were already marked as identical by someone else as: " );
          ATH_MSG_DEBUG( "i/j/result[i]/result[j]=" << i << " / " << j << " / " << muResult[i] << " / "  << muResult[j] );
          vec_OverlapInsonsistent.push_back(0);
          errorWhenIdentifyingOverlap = true;
        }
      }
      else{ // judged as overlap
	if( (muResult[j] != j && muResult[i] != muResult[j]) || (muResult[j] == j && muResult[i] != i) ){
	  ATH_MSG_DEBUG( "inconsistentency in muFast overlap removal for more than two objects" );
	  ATH_MSG_DEBUG( "two objects are judged as overlap but only either was already marked as overlap to someone else: " );
	  ATH_MSG_DEBUG( "i/j/result[i]/result[j]=" << i << " / " << j << " / " << muResult[i] << " / "  << muResult[j] );
          vec_OverlapInsonsistent.push_back(1);
	  errorWhenIdentifyingOverlap = true;
        }
	if( muResult[i] == i ) {
	  ATH_MSG_DEBUG( "   i is not yet marked as overlap. so, it is a newly found overlap" );
	  ATH_MSG_DEBUG( "   -> marking muResult[j] as i..." );
	  muResult[j] = i;
	} else {
	  ATH_MSG_DEBUG( "   both i/j already marked as overlap by: muResult[i]=" << muResult[i] );
	  ATH_MSG_DEBUG( "   -> do nothing..." );
	}
      }
    }
  }

  // If there is inconsistency in the overlap judgment, input becomes output as it is.
  if( errorWhenIdentifyingOverlap ) {
    NOutMuon = featureCont.size();
    NRemovedMuon = 0;
    fill(m_group+"_"+chain, NInMuon, NOutMuon, NRemovedMuon);
    return StatusCode::SUCCESS;
  }
      
  // check number of unique muon
  unsigned int NUniqueMuon = 0;
  for(i=0; i<featureCont.size(); i++) {
    if( muResult[i] == i )  NUniqueMuon++;
  }
  ATH_MSG_DEBUG( "number of unique Muons after overlap removal=" << NUniqueMuon );


  if( featureCont.size() != NUniqueMuon ){
    ATH_CHECK(chooseBestMuon(chain, featureCont, muResult));
    NOutMuon = NUniqueMuon;
    ATH_MSG_DEBUG("debugdebug::: NInMuon = " << NInMuon << ",  NOutMuon = " << NOutMuon);
  } else {
    NOutMuon = featureCont.size();
  }

  NRemovedMuon = featureCont.size() - NUniqueMuon;

  fill(m_group+"_"+chain, NInMuon, NOutMuon, NRemovedMuon);

  return StatusCode::SUCCESS;
}


bool L2OverlapRemoverMonMT :: isOverlap(const std::string &chain, const ElementLink<xAOD::L2StandAloneMuonContainer> muEL1, const ElementLink<xAOD::L2StandAloneMuonContainer> muEL2,
                                        std::tuple<float,float,float> (*trigPosForMatchFunc)(const xAOD::L2StandAloneMuon*)) const {

  const float ZERO_LIMIT = 0.00001;

  auto dR = Monitored::Scalar<float>(chain+"_dR", -999.);
  auto invMass = Monitored::Scalar<float>(chain+"_invMass", -999.);
  auto dRLog10 = Monitored::Scalar<float>(chain+"_dRLog10", -999.);
  auto invMassLog10 = Monitored::Scalar<float>(chain+"_invMassLog10", -999.);

  const auto [mu1Pt, mu1Eta, mu1Phi] = trigPosForMatchFunc((*muEL1));
  const auto [mu2Pt, mu2Eta, mu2Phi] = trigPosForMatchFunc((*muEL2));

  if( ( (std::abs(mu1Eta) < ZERO_LIMIT) && (std::abs(mu1Phi) < ZERO_LIMIT) ) || 
      ( (std::abs(mu2Eta) < ZERO_LIMIT) && (std::abs(mu2Phi) < ZERO_LIMIT) ) || 
      (std::abs(mu1Pt) < ZERO_LIMIT) || (std::abs(mu2Pt) < ZERO_LIMIT) ) return false;

  double dRThres   = 9999.;
  double massThres = 9999.;
  bool isBarrel1 = (*muEL1)->sAddress() != -1 ? true : false;
  bool isBarrel2 = (*muEL2)->sAddress() != -1 ? true : false;

  if(  isBarrel1 && isBarrel2 ) { // BB
    ATH_MSG_DEBUG( "   ...B-B" );
    dRThres  =m_dRThresBB;
    massThres=m_massThresBB;
  }
  else if( (isBarrel1 && ! isBarrel2) || (!isBarrel1 && isBarrel2) ) { // BE
    ATH_MSG_DEBUG( "   ...B-E" );
    dRThres  =m_dRThresBE;
    massThres=m_massThresBE;
  }
  else { // EE
    ATH_MSG_DEBUG( "   ...E-E" );
    double absEta = (std::abs(mu1Pt) > std::abs(mu2Pt)) ? std::abs(mu1Eta) : std::abs(mu2Eta);
    unsigned int iThres=0;
    for(unsigned int i=0; i<(m_etaBinsEC.size()-1); i++) {
      if ( m_etaBinsEC[i] <= absEta && absEta < m_etaBinsEC[i+1] ) iThres = i;
    }
    dRThres   = m_dRThresEC[iThres];
    massThres = m_massThresEC[iThres];
  }
  ATH_MSG_DEBUG( "   ...dR   threshold=" << dRThres );
  ATH_MSG_DEBUG( "   ...mass threshold=" << massThres );


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


StatusCode L2OverlapRemoverMonMT::chooseBestMuon(const std::string &chain, std::vector< TrigCompositeUtils::LinkInfo<xAOD::L2StandAloneMuonContainer> > featureCont, std::vector<unsigned int> muResult) const
{
  unsigned int i,j,k;

  std::vector<float> vec_RemovedEta, vec_RemovedPhi, vec_RemovedPt;
  auto RemovedEta  = Monitored::Collection(chain+"_RemovedEta", vec_RemovedEta);
  auto RemovedPhi  = Monitored::Collection(chain+"_RemovedPhi", vec_RemovedPhi);
  auto RemovedPt   = Monitored::Collection(chain+"_RemovedPt", vec_RemovedPt);

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
    else {
      // must choose one best
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


std::tuple<float,float,float> L2OverlapRemoverMonMT :: L2SAPosForMatchFunc(const xAOD::L2StandAloneMuon *trig){
  return std::forward_as_tuple(trig->pt(), trig->etaMS(), trig->phiMS());
}

