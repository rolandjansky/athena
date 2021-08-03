/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
// HLTSeeding includes
#include "EMRoIsUnpackingTool.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "AthenaMonitoringKernel/Monitored.h"

EMRoIsUnpackingTool::EMRoIsUnpackingTool( const std::string& type,
                                          const std::string& name, 
                                          const IInterface* parent ) 
  : RoIsUnpackingToolBase(type, name, parent),
    m_configSvc( "TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name )
{
}

using namespace TrigCompositeUtils;


StatusCode EMRoIsUnpackingTool::initialize() {

  ATH_CHECK( RoIsUnpackingToolBase::initialize() );
  ATH_CHECK( m_configSvc.retrieve() );
  ATH_CHECK( m_trigRoIsKey.initialize() );
  ATH_CHECK( m_recRoIsKey.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode EMRoIsUnpackingTool::start() {
  ATH_CHECK( decodeMapping( [](const std::string& name ){ return name.find("EM") == 0 or name.find(getProbeThresholdName("EM")) == 0; } ) );
  return StatusCode::SUCCESS;
}

StatusCode EMRoIsUnpackingTool::updateConfiguration() {
  m_emThresholds.clear();
  using namespace TrigConf;
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
  SG::WriteHandle<DecisionContainer> handle4 = createAndStore(m_decisionsKeyProbe, ctx ); 
  auto decisionOutputProbe = handle4.ptr();

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

      // The hltSeedingNodeName denotes an initial node with no parents
      Decision* decisionMain = TrigCompositeUtils::newDecisionIn( decisionOutput, hltSeedingNodeName() ); 
      Decision* decisionProbe = TrigCompositeUtils::newDecisionIn( decisionOutputProbe, hltSeedingNodeName() );

      std::vector<unsigned> passedThresholdIDs;

      for ( auto th: m_emThresholds ) {
        ATH_MSG_VERBOSE( "Checking if the threshold " << th->name() << " passed" );
        if ( recRoI->passedThreshold( th->thresholdNumber() ) ) {
          passedThresholdIDs.push_back( HLT::Identifier( th->name() ) );
          const std::string thresholdProbeName = getProbeThresholdName(th->name());
          ATH_MSG_DEBUG( "Passed Threshold names " << th->name() << " and " << thresholdProbeName);
          addChainsToDecision( HLT::Identifier( th->name() ), decisionMain, activeChains );
          addChainsToDecision( HLT::Identifier( thresholdProbeName ), decisionProbe, activeChains );
        }
      }

      decisionMain->setDetail( "thresholds", passedThresholdIDs );
      decisionMain->setObjectLink( initialRoIString(), ElementLink<TrigRoiDescriptorCollection>( m_trigRoIsKey.key(), trigRoIs->size()-1 ) );
      decisionMain->setObjectLink( initialRecRoIString(), ElementLink<DataVector<LVL1::RecEmTauRoI>>( m_recRoIsKey.key(), recRoIs->size()-1 ) );

      decisionProbe->setDetail( "thresholds", passedThresholdIDs );
      decisionProbe->setObjectLink( initialRoIString(), ElementLink<TrigRoiDescriptorCollection>( m_trigRoIsKey.key(), trigRoIs->size()-1 ) );
      decisionProbe->setObjectLink( initialRecRoIString(), ElementLink<DataVector<LVL1::RecEmTauRoI>>( m_recRoIsKey.key(), recRoIs->size()-1 ) );
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

