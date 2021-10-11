
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "HLTSeedingNoCtpForTesting.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigT1Interfaces/RecEmTauRoI.h"

#include "StoreGate/WriteHandle.h"
#include "GaudiKernel/EventContext.h"


HLTSeedingNoCtpForTesting::HLTSeedingNoCtpForTesting(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator) {}


StatusCode HLTSeedingNoCtpForTesting::initialize() {
  ATH_MSG_INFO( "Reading RoIB information from: " << m_RoIBResultKey.objKey()
                << " : " << m_RoIBResultKey.fullKey() << " : " << m_RoIBResultKey.key() );
  ATH_CHECK( m_RoIBResultKey.initialize( ) );
  ATH_CHECK( m_trigEMRoIsKey.initialize() );
  ATH_CHECK( m_EMDecisionsKey.initialize() );
  ATH_CHECK( m_trigFSRoIKey.initialize() );
  return StatusCode::SUCCESS;
}


StatusCode HLTSeedingNoCtpForTesting::execute (const EventContext& ctx) const {
  using namespace TrigCompositeUtils;
  SG::ReadHandle<ROIB::RoIBResult> roibH( m_RoIBResultKey, ctx );
  ATH_CHECK( roibH.isValid() );

  SG::WriteHandle<TrigRoiDescriptorCollection> emRoIs = createAndStoreNoAux(m_trigEMRoIsKey, ctx);
  SG::WriteHandle<TrigRoiDescriptorCollection> fsRoIs = createAndStoreNoAux(m_trigFSRoIKey, ctx);
  fsRoIs->push_back( std::make_unique<TrigRoiDescriptor>(true) );
  using namespace TrigConf;
  TrigConf::TriggerThreshold threshold;

  SG::WriteHandle<DecisionContainer> decisionOutput = createAndStore(m_EMDecisionsKey, ctx );

  std::vector<TrigConf::TriggerThreshold*> thresholds{ & threshold};

  for ( const auto & emTauFragment : roibH->eMTauResult() ) {
    for ( const auto & roi : emTauFragment.roIVec() ) {
      uint32_t roIWord = roi.roIWord();
      if ( not ( LVL1::TrigT1CaloDefs::EMRoIWordType == roi.roIType() ) )  {
        ATH_MSG_VERBOSE( "Skipping RoI as it is not EM threshold " << roIWord );
        continue;
      }
      LVL1::RecEmTauRoI recRoI( roIWord, &thresholds );

      emRoIs->push_back( std::make_unique<TrigRoiDescriptor>(
        roIWord, 0u ,0u,
        recRoI.eta(), recRoI.eta()-m_roIWidth, recRoI.eta()+m_roIWidth,
        recRoI.phi(), recRoI.phi()-m_roIWidth, recRoI.phi()+m_roIWidth) );

      ATH_MSG_DEBUG("Decoded EM RoI at position " << *emRoIs->back() );

      Decision* decision = TrigCompositeUtils::newDecisionIn( decisionOutput.ptr(), hltSeedingNodeName() );
      decision->setObjectLink( initialRoIString(),
                               ElementLink<TrigRoiDescriptorCollection>(m_trigEMRoIsKey.key(), emRoIs->size()-1) );
      addDecisionID( HLT::Identifier( "HLT_EMTestChain" ), decision );
    }
  }

  return StatusCode::SUCCESS;
}
