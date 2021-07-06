
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "StoreGate/WriteHandle.h"
#include "GaudiKernel/EventContext.h"
#include "TrigConfHLTUtils/HLTUtils.h"
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "L1TestDecoder.h"

L1TestDecoder::L1TestDecoder(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{}


StatusCode L1TestDecoder::initialize() {
  ATH_MSG_INFO( "Reading RoIB infromation from: "<< m_RoIBResultKey.objKey() << " : " << m_RoIBResultKey.fullKey() << " : " << m_RoIBResultKey.key() );  
  ATH_CHECK( m_RoIBResultKey.initialize( ) );
  ATH_CHECK( m_trigEMRoIsKey.initialize() );
  ATH_CHECK(m_EMDecisionsKey.initialize() );
  ATH_CHECK( m_trigFSRoIKey.initialize() );  
  return StatusCode::SUCCESS;
}


StatusCode L1TestDecoder::execute (const EventContext& ctx) const {
  using namespace TrigCompositeUtils;
  SG::ReadHandle<ROIB::RoIBResult> roibH( m_RoIBResultKey, ctx );
  ATH_CHECK( roibH.isValid() );

  SG::WriteHandle<TrigRoiDescriptorCollection> handleTrigRoIs = createAndStoreNoAux(m_trigEMRoIsKey, ctx ); 
  auto emRoIs = handleTrigRoIs.ptr();

  SG::WriteHandle<TrigRoiDescriptorCollection> handleTrigFSRoI = createAndStoreNoAux(m_trigFSRoIKey, ctx ); 
  auto fsRoIs = handleTrigFSRoI.ptr();
  fsRoIs->push_back( new TrigRoiDescriptor( true ) );
  using namespace TrigConf;
  TrigConf::TriggerThreshold threshold;

  SG::WriteHandle<DecisionContainer> handleDecisions = createAndStore(m_EMDecisionsKey, ctx ); 
  auto decisionOutput = handleDecisions.ptr();
  
  std::vector<TrigConf::TriggerThreshold*> thresholds{ & threshold};
  
  for ( auto& emTauFragment : roibH->eMTauResult() ) {
    for ( auto& roi : emTauFragment.roIVec() ) {
      uint32_t roIWord = roi.roIWord();      
      if ( not ( LVL1::TrigT1CaloDefs::EMRoIWordType == roi.roIType() ) )  {
	ATH_MSG_VERBOSE( "Skipping RoI as it is not EM threshold " << roIWord );
	continue;
      }
      LVL1::RecEmTauRoI recRoI( roIWord, &thresholds );
      auto roiDesc = new TrigRoiDescriptor( roIWord, 0u ,0u,
					recRoI.eta(), recRoI.eta()-m_roIWidth, recRoI.eta()+m_roIWidth,
					recRoI.phi(), recRoI.phi()-m_roIWidth, recRoI.phi()+m_roIWidth );
      ATH_MSG_DEBUG("Decoded EM RoI at position " << *roiDesc );
      emRoIs->push_back( roiDesc );

      auto decision  = TrigCompositeUtils::newDecisionIn( decisionOutput, l1DecoderNodeName() );
      decision->setObjectLink( initialRoIString(), ElementLink<TrigRoiDescriptorCollection>( m_trigEMRoIsKey.key(), emRoIs->size()-1 ) );
      addDecisionID( HLT::Identifier( "HLT_EMTestChain" ), decision );      
    }
  }
  
  return StatusCode::SUCCESS;  
}

// StatusCode L1TestDecoder::finalize() {
//   return StatusCode::SUCCESS;
// }
