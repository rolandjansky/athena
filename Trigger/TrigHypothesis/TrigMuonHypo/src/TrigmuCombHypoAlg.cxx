/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "DecisionHandling/TrigCompositeUtils.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h" 

#include "TrigMuonHypo/TrigmuCombHypoAlg.h"

using namespace TrigCompositeUtils; 

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigmuCombHypoAlg::TrigmuCombHypoAlg( const std::string& name,
				      ISvcLocator* pSvcLocator ) :
  ::AthReentrantAlgorithm( name, pSvcLocator )
{
   declareProperty("Decisions", m_decisionsKey = std::string("Decisions"), "Decision on muCombHypo to write in");
   declareProperty("MuonSADecisions", m_muonDecisionsKey = std::string("MuonSADecisions"), "Decisions from MufastHypo to read in");
   declareProperty("ViewRoIs", m_viewsKey = std::string("ViewRoIs"), "MUViewRoIs to read in");
   declareProperty("MuCombContainer", m_combinedKey = std::string("MuCombContainer"), "xAOD::L2CombinedMuonContainer to read in");
} 

TrigmuCombHypoAlg::~TrigmuCombHypoAlg() 
{}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigmuCombHypoAlg::initialize()
{
  ATH_MSG_INFO ( "Initializing " << name() << "..." );
  ATH_CHECK(m_hypoTools.retrieve());

  ATH_CHECK(m_decisionsKey.initialize());

  ATH_CHECK(m_muonDecisionsKey.initialize());

  ATH_CHECK(m_viewsKey.initialize());

  renounce(m_combinedKey);
  ATH_CHECK(m_combinedKey.initialize());

  ATH_MSG_INFO( "Initialization completed successfully" );
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigmuCombHypoAlg::finalize() 
{   
  ATH_MSG_INFO( "Finalizing " << name() << "..." );
  ATH_MSG_INFO( "Finalization completed successfully" );
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigmuCombHypoAlg::execute_r(const EventContext& context) const
{

  ATH_MSG_DEBUG("StatusCode TrigmuCombHypoAlg::execute_r start");

  // prepare decisions container
  auto decisions = std::make_unique<DecisionContainer>();
  auto aux = std::make_unique<DecisionAuxContainer>();
  decisions->setStore(aux.get());

  // extract mapping of cluster pointer to an index in the cluster decision collection
  auto clusterDecisionsHandle= SG::makeHandle( m_muonDecisionsKey, context );
  std::map<const xAOD::L2StandAloneMuon*, size_t> clusterToIndexMap;

  size_t clusterCounter = 0;
  for (auto cluIter = clusterDecisionsHandle->begin(); cluIter != clusterDecisionsHandle->end(); ++cluIter) {
    ATH_CHECK((*cluIter)->hasObjectLink("feature"));
    const xAOD::L2StandAloneMuon* cluster = (*cluIter)->object<xAOD::L2StandAloneMuon>("feature");
    clusterToIndexMap.insert(std::make_pair(cluster, clusterCounter));
 
    ++clusterCounter;
  }
  ATH_MSG_DEBUG( "Cluster ptr to decision map has size " << clusterToIndexMap.size() );

  // prepare imput for tools  
  std::vector<TrigmuCombHypoTool::CombinedMuonInfo> hypoToolInput;

  // retrieve views created on l2muCombViewsCreator
  auto viewsHandle = SG::makeHandle(m_viewsKey, context);
  if (!viewsHandle.isValid()) {
    ATH_MSG_ERROR("ReadHandle for std::vector< SG::View*> key:" << m_viewsKey.key() << " is failed");
    return StatusCode::FAILURE;
  }

  for (auto view: *viewsHandle) {
    size_t muonCounter = 0;

    // retrieve xAOD::CombinedMuonContaier created on muComb algorithm
    auto muonHandle = SG::makeHandle(m_combinedKey, context);
    ATH_CHECK(muonHandle.setProxyDict(view));
    if(!muonHandle.isValid()) {
      ATH_MSG_ERROR("ReadHandle for xAOD::L2CombinedMuonContainer key:" << m_combinedKey.key() << " is failed");
      return StatusCode::FAILURE;
    }

    xAOD::L2CombinedMuonContainer::const_iterator muIter;
    for (muIter = muonHandle->begin(); muIter != muonHandle->end(); ++muIter) {
      auto d = newDecisionIn(decisions.get());
      auto element = ElementLink<xAOD::L2CombinedMuonContainer>(view->name()+"_"+m_combinedKey.key(), muonCounter);
      d->setObjectLink("feature", element);

      // get muSATracks from xAOD::CombinedMuonContainer
      auto clusterPtr = (*muIter)->muSATrack();
      ATH_CHECK(clusterPtr != nullptr);

      ATH_MSG_DEBUG("REGTEST: muSATrack pt in " << m_combinedKey.key() << " = " << clusterPtr->pt() << " GeV");
      ATH_MSG_DEBUG("REGTEST: muSATrack eta/phi in " << m_combinedKey.key() << " = " << clusterPtr->eta() << "/" << clusterPtr->phi());
      
      ATH_MSG_DEBUG("REGTEST: muCBTrack pt in " << m_combinedKey.key() << " = " << (*muIter)->pt() << " GeV");
      ATH_MSG_DEBUG("REGTEST: muCBTrack eta/phi in " << m_combinedKey.key() << " = " << (*muIter)->eta() << "/" << (*muIter)->phi());
      
      // now find matching cluster could use geometric matching 
      // but in fact the cluster owned by the decision object and the cluster owned by the electron should be the same       
      // since we have a map made in advance we can make use of the index lookup w/o the need for additional loop 
      auto origCluster = clusterToIndexMap.find(clusterPtr);
      ATH_CHECK(origCluster != clusterToIndexMap.end());
      linkToPrevious(d, m_muonDecisionsKey.key(), origCluster->second);

      // now we have DecisionObject ready to be passed to hypo tool. 
      // it has link to electron, and decisions on clusters 
      // we shall avoid calling the tools for chains which were already rejected on certain cluster, but this is left to hypo tools
      DecisionIDContainer clusterDecisionIDs;
      decisionIDs(clusterDecisionsHandle->at(origCluster->second), clusterDecisionIDs);
      
      auto el = TrigmuCombHypoTool::CombinedMuonInfo{d, *muIter, origCluster->first, clusterDecisionIDs}; 
      hypoToolInput.emplace_back(el);
    } 
    ++muonCounter;
  }

  // hypo tools
  for ( auto & tool: m_hypoTools ) {
    ATH_MSG_DEBUG("Go to " << tool);
    ATH_CHECK( tool->decide( hypoToolInput ) );
  }

  // recorded decision objects on TrigmuCombHypo
  auto handle =  SG::makeHandle( m_decisionsKey, context );
  ATH_CHECK( handle.record( std::move( decisions ), std::move( aux ) ) );

  ATH_MSG_DEBUG("StatusCode TrigmuCombHypoAlg::execute_r success");
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

