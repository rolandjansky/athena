/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JRoIsUnpackingTool.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "AthenaMonitoring/Monitored.h"
#include "TrigConfL1Data/CTPConfig.h"

JRoIsUnpackingTool::JRoIsUnpackingTool( const std::string& type, 
                                        const std::string& name, 
                                        const IInterface* parent ) 
  : RoIsUnpackingToolBase(type, name, parent),
    m_configSvc( "TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name ) {
} 

StatusCode JRoIsUnpackingTool::initialize() {
 
  ATH_CHECK( RoIsUnpackingToolBase::initialize() );
  ATH_CHECK( m_configSvc.retrieve() );
  ATH_CHECK( m_trigRoIsKey.initialize() );
  ATH_CHECK( m_recRoIsKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode JRoIsUnpackingTool::updateConfiguration( const IRoIsUnpackingTool::SeedingMap& seeding) {
  using namespace TrigConf;  

  ATH_CHECK( decodeMapping( [](const TriggerThreshold* th){ return th->ttype() == L1DataDef::JET; }, 
			    m_configSvc->ctpConfig()->menu().itemVector(),
			    seeding ) );

  m_jetThresholds.clear();  
  ATH_CHECK( copyThresholds(m_configSvc->thresholdConfig()->getThresholdVector( L1DataDef::JET ), m_jetThresholds ) );
  ATH_CHECK( copyThresholds(m_configSvc->thresholdConfig()->getThresholdVector( L1DataDef::JF ), m_jetThresholds ) );
  ATH_CHECK( copyThresholds(m_configSvc->thresholdConfig()->getThresholdVector( L1DataDef::JB ), m_jetThresholds ) );

  if ( m_jetThresholds.empty() ) {
    ATH_MSG_WARNING( "No JET thresholds configured" );
  } else {
    ATH_MSG_INFO( "Configured " << m_jetThresholds.size() << " thresholds" );
  }


  
  return StatusCode::SUCCESS;
}


StatusCode JRoIsUnpackingTool::unpack( const EventContext& ctx,
				       const ROIB::RoIBResult& roib,
				       const HLT::IDSet& activeChains ) const {
  using namespace TrigCompositeUtils;
  // create and record the collections needed
  SG::WriteHandle<TrigRoiDescriptorCollection> handle1 = createAndStoreNoAux(m_trigRoIsKey, ctx ); 
  auto trigRoIs = handle1.ptr();
  SG::WriteHandle< DataVector<LVL1::RecJetRoI> > handle2 = createAndStoreNoAux( m_recRoIsKey, ctx );
  auto recRoIs = handle2.ptr();
  SG::WriteHandle<DecisionContainer> handle3 = createAndStore(m_decisionsKey, ctx ); 
  auto decisionOutput = handle3.ptr();


  auto decision  = TrigCompositeUtils::newDecisionIn( decisionOutput );  
  decision->setObjectLink( "initialRoI", ElementLink<TrigRoiDescriptorCollection>( m_fsRoIKey, 0 ) );
  auto roiEL = decision->objectLink<TrigRoiDescriptorCollection>( "initialRoI" );
  CHECK( roiEL.isValid() );
  ATH_MSG_DEBUG("Linked new Decision to the FS roI");
  ATH_MSG_DEBUG("Now get jet L1 thresholds from RoIB");

  // RoIBResult contains vector of jet fragments
  for ( auto& jetFragment : roib.jetEnergyResult() ) {
    for ( auto& roi : jetFragment.roIVec() ) {
      uint32_t roIWord = roi.roIWord();      
      if ( not ( LVL1::TrigT1CaloDefs::JetRoIWordType == roi.roIType() ) )  {
	ATH_MSG_DEBUG( "Skipping RoI as it is not JET threshold " << roIWord <<" Type "<< roi.roIType() );
	continue;
      }
      
      auto recRoI = new LVL1::RecJetRoI( roIWord, &m_jetThresholds );
      recRoIs->push_back( recRoI );

      /* TDOD, decide if we need this collection at all here, now keep filling of it commented out
      //decision->setObjectLink( "initialRecRoI", ElementLink<DataVector<LVL1::RecJetRoI>>( m_recRoIsKey.key(), recRoIs->size()-1 ) );
      auto trigRoI = new TrigRoiDescriptor( roIWord, 0u ,0u,
					    recRoI->eta(), recRoI->eta()-m_roIWidth, recRoI->eta()+m_roIWidth,
					    recRoI->phi(), recRoI->phi()-m_roIWidth, recRoI->phi()+m_roIWidth );
      trigRoIs->push_back( trigRoI );
      */
      ATH_MSG_DEBUG( "RoI word: 0x" << MSG::hex << std::setw( 8 ) << roIWord << MSG::dec );      
      
      for ( auto th: m_jetThresholds ) {
	ATH_MSG_VERBOSE( "Checking if the threshold " << th->name() << " passed" );
	if ( recRoI->passedThreshold( th->thresholdNumber() ) ) {
	  ATH_MSG_DEBUG( "Passed Threshold name " << th->name() );
	  addChainsToDecision( HLT::Identifier( th->name() ), decision, activeChains );
	  ATH_MSG_DEBUG( "Labeled object with chains: " << [&](){ 
	      TrigCompositeUtils::DecisionIDContainer ids; 
	      TrigCompositeUtils::decisionIDs( decision, ids ); 
	      return std::vector<TrigCompositeUtils::DecisionID>( ids.begin(), ids.end() ); }() );
	}
      }
      

    }     
  }

  TrigCompositeUtils::uniqueDecisionIDs( decision);
 

  if ( msgLvl(MSG::DEBUG) ) {
    for ( auto roi: *trigRoIs ) {
      ATH_MSG_DEBUG( "RoI Eta: " << roi->eta() << " Phi: " << roi->phi() << " RoIWord: " << roi->roiWord() );
    }
  }

  // monitoring
  {
    auto RoIsCount = Monitored::Scalar( "count", trigRoIs->size() );
    auto RoIsPhi   = Monitored::Collection( "phi", *trigRoIs, &TrigRoiDescriptor::phi );
    auto RoIsEta   = Monitored::Collection( "eta", *trigRoIs, &TrigRoiDescriptor::eta );
    Monitored::Group( m_monTool,  RoIsCount, RoIsEta, RoIsPhi );
  }

  ATH_MSG_DEBUG( "Number of decision IDs associated with FS RoI: " <<  TrigCompositeUtils::decisionIDs( decision ).size()  );


  return StatusCode::SUCCESS; // what else
}
