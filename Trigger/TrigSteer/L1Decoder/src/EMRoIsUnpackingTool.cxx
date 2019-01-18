/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// L1Decoder includes
#include "EMRoIsUnpackingTool.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "AthenaMonitoring/Monitored.h"
#include "TrigConfL1Data/CTPConfig.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
EMRoIsUnpackingTool::EMRoIsUnpackingTool( const std::string& type, 
                                          const std::string& name, 
                                          const IInterface* parent ) 
  : RoIsUnpackingToolBase(type, name, parent),
    m_configSvc( "TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name )
{
}


StatusCode EMRoIsUnpackingTool::initialize() {

  CHECK( RoIsUnpackingToolBase::initialize() );
  CHECK( m_configSvc.retrieve() );
  CHECK( m_trigRoIsKey.initialize() );
  CHECK( m_recRoIsKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode EMRoIsUnpackingTool::updateConfiguration( const IRoIsUnpackingTool::SeedingMap& seeding ) {
  using namespace TrigConf;
  
  ATH_CHECK( decodeMapping( [](const TriggerThreshold* th){ return th->ttype() == L1DataDef::EM; }, 
			    m_configSvc->ctpConfig()->menu().itemVector(),
			    seeding ) );

  m_emThresholds.clear();

  const ThresholdConfig* thresholdConfig = m_configSvc->thresholdConfig();
  auto filteredThresholds= thresholdConfig->getThresholdVector( L1DataDef::EM );
  ATH_MSG_DEBUG( "Number of filtered thresholds " << filteredThresholds.size() );
  for ( auto th :  filteredThresholds ) {
    if ( th != nullptr ) {
      ATH_MSG_INFO( "Found threshold in the configuration: " << th->name() << " of ID: " << HLT::Identifier( th->name() ).numeric() ); 
      m_emThresholds.push_back( th );
    } else {
      ATH_MSG_DEBUG( "Nullptr to the threshold" ); 
    }
  }
  
  if ( m_emThresholds.empty() ) {
    ATH_MSG_WARNING( "No EM thresholds configured" );
  } else {
    ATH_MSG_INFO( "Configured " << m_emThresholds.size() << " thresholds" );
  }

  

  return StatusCode::SUCCESS;
}


StatusCode EMRoIsUnpackingTool::finalize()
{
  return StatusCode::SUCCESS;
}


StatusCode EMRoIsUnpackingTool::unpack( const EventContext& ctx,
					const ROIB::RoIBResult& roib,
					const HLT::IDSet& activeChains ) const {
  using namespace TrigCompositeUtils;

  // create and record the collections needed
  SG::WriteHandle<TrigRoiDescriptorCollection> handle1 = createAndStoreNoAux(m_trigRoIsKey, ctx ); 
  auto trigRoIs = handle1.ptr();
  SG::WriteHandle< DataVector<LVL1::RecEmTauRoI> > handle2 = createAndStoreNoAux( m_recRoIsKey, ctx );
  auto recRoIs = handle2.ptr();
  SG::WriteHandle<DecisionContainer> handle3 = createAndStore(m_decisionsKey, ctx ); 
  auto decisionOutput = handle3.ptr();

  // RoIBResult contains vector of EM fragments
  for ( auto& emTauFragment : roib.eMTauResult() ) {
    for ( auto& roi : emTauFragment.roIVec() ) {
      uint32_t roIWord = roi.roIWord();      
      if ( not ( LVL1::TrigT1CaloDefs::EMRoIWordType == roi.roIType() ) )  {
	ATH_MSG_DEBUG( "Skipping RoI as it is not EM threshold " << roIWord );
	continue;
      }
      
      auto recRoI = new LVL1::RecEmTauRoI( roIWord, &m_emThresholds );
      recRoIs->push_back( recRoI );
      
      auto trigRoI = new TrigRoiDescriptor( roIWord, 0u ,0u,
					    recRoI->eta(), recRoI->eta()-m_roIWidth, recRoI->eta()+m_roIWidth,
					    recRoI->phi(), recRoI->phi()-m_roIWidth, recRoI->phi()+m_roIWidth );
      trigRoIs->push_back( trigRoI );
			  
      ATH_MSG_DEBUG( "RoI word: 0x" << MSG::hex << std::setw( 8 ) << roIWord << MSG::dec );      

      auto decision  = TrigCompositeUtils::newDecisionIn( decisionOutput );

      std::vector<unsigned> passedThresholdIDs;
      for ( auto th: m_emThresholds ) {
	
	ATH_MSG_VERBOSE( "Checking if the threshold " << th->name() << " passed" );
	if ( recRoI->passedThreshold( th->thresholdNumber() ) ) {
	  passedThresholdIDs.push_back( HLT::Identifier( th->name() ) );
	  ATH_MSG_DEBUG( "Passed Threshold name " << th->name() );
	  addChainsToDecision( HLT::Identifier( th->name() ), decision, activeChains );
	  ATH_MSG_DEBUG( "Labeled object with chains: " << [&](){ 
	      TrigCompositeUtils::DecisionIDContainer ids; 
	      TrigCompositeUtils::decisionIDs( decision, ids ); 
	      return std::vector<TrigCompositeUtils::DecisionID>( ids.begin(), ids.end() ); }() );
	}
      }
      decision->setDetail( "thresholds", passedThresholdIDs );
      decision->setObjectLink( "initialRoI", ElementLink<TrigRoiDescriptorCollection>( m_trigRoIsKey.key(), trigRoIs->size()-1 ) );
      decision->setObjectLink( "initialRecRoI", ElementLink<DataVector<LVL1::RecEmTauRoI>>( m_recRoIsKey.key(), recRoIs->size()-1 ) );
    }     
  }
  for ( auto roi: *trigRoIs ) {
    ATH_MSG_DEBUG( "RoI Eta: " << roi->eta() << " Phi: " << roi->phi() << " RoIWord: " << roi->roiWord() );
  }

  // monitoring
  {
    auto RoIsCount = Monitored::Scalar( "count", trigRoIs->size() );
    auto RoIsPhi   = Monitored::Collection( "phi", *trigRoIs, &TrigRoiDescriptor::phi );
    auto RoIsEta   = Monitored::Collection( "eta", *trigRoIs, &TrigRoiDescriptor::eta );
    Monitored::Group( m_monTool,  RoIsCount, RoIsEta, RoIsPhi );
  }

  ATH_MSG_DEBUG( "Unpacked " <<  trigRoIs->size() << " RoIs" );

  return StatusCode::SUCCESS; // what else
  

  
}

