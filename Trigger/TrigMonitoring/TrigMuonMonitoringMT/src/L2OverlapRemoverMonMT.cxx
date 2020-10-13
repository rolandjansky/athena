/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "L2OverlapRemoverMonMT.h"

#include "MuonMatchingTool.h"

L2OverlapRemoverMonMT :: L2OverlapRemoverMonMT(const std::string& name, ISvcLocator* pSvcLocator )
  : TrigMuonMonitorAlgorithm(name, pSvcLocator)
{}


StatusCode L2OverlapRemoverMonMT :: initialize(){
  StatusCode sc = TrigMuonMonitorAlgorithm::initialize();
  return sc;
}


StatusCode L2OverlapRemoverMonMT :: fillVariablesPerChain(const EventContext &ctx, const std::string &chain) const {

  ATH_MSG_DEBUG ("Filling histograms for " << name() << "...");

  const float ZERO_LIMIT = 0.00001;

  std::vector< TrigCompositeUtils::LinkInfo<xAOD::L2StandAloneMuonContainer> > featureCont = getTrigDecisionTool()->features<xAOD::L2StandAloneMuonContainer>( chain, TrigDefs::includeFailedDecisions );
  for(const TrigCompositeUtils::LinkInfo<xAOD::L2StandAloneMuonContainer>& muLinkInfo : featureCont){
    ATH_CHECK( muLinkInfo.isValid() );
    const ElementLink<xAOD::L2StandAloneMuonContainer> muEL = muLinkInfo.link;
  }


  return StatusCode::SUCCESS;
}
