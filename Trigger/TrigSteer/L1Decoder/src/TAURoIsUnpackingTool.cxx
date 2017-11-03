/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// L1Decoder includes
#include "TAURoIsUnpackingTool.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "AthenaMonitoring/MonitoredScope.h"


/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
TAURoIsUnpackingTool::TAURoIsUnpackingTool( const std::string& type, 
                                          const std::string& name, 
                                          const IInterface* parent ) 
  : RoIsUnpackingToolBase(type, name, parent),
    m_configSvc( "TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name )
{
}


StatusCode TAURoIsUnpackingTool::initialize() {

  CHECK( RoIsUnpackingToolBase::initialize() );
  CHECK( m_configSvc.retrieve() );
  CHECK( m_trigRoIsKey.initialize() );
  CHECK( m_recRoIsKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode TAURoIsUnpackingTool::updateConfiguration() {
  using namespace TrigConf;

  m_emThresholds.clear();

  const ThresholdConfig* thresholdConfig = m_configSvc->thresholdConfig();
  auto filteredThresholds= thresholdConfig->getThresholdVector( L1DataDef::TAU );
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
    ATH_MSG_WARNING( "No TAU thresholds configured" );
  } else {
    ATH_MSG_INFO( "Configured " << m_emThresholds.size() << " thresholds" );
  }

  

  return StatusCode::SUCCESS;
}


StatusCode TAURoIsUnpackingTool::finalize()
{
  return StatusCode::SUCCESS;
}


StatusCode TAURoIsUnpackingTool::unpack( const EventContext& ctx,
          const ROIB::RoIBResult& roib,
          const HLT::IDSet& activeChains ) const {
  using namespace TrigCompositeUtils;
  auto decisionOutput = std::make_unique<DecisionContainer>();
  auto decisionAux    = std::make_unique<DecisionAuxContainer>();
  decisionOutput->setStore( decisionAux.get() );  
  auto trigRoIs = std::make_unique< TrigRoiDescriptorCollection >();
  auto recRoIs  = std::make_unique< DataVector<LVL1::RecEmTauRoI> >();


  // RoIBResult contains vector of TAU fragments
  for ( auto& emTauFragment : roib.eMTauResult() ) {
    for ( auto& roi : emTauFragment.roIVec() ) {
      uint32_t roIWord = roi.roIWord();      
      if ( not ( LVL1::TrigT1CaloDefs::TauRoIWordType == roi.roIType() ) )  {
  ATH_MSG_DEBUG( "Skipping RoI as it is not TAU threshold " << roIWord );
  continue;
      }
      
      auto recRoI = new LVL1::RecEmTauRoI( roIWord, &m_emThresholds );
      recRoIs->push_back( recRoI );
      
      auto trigRoI = new TrigRoiDescriptor( roIWord, 0u ,0u,
              recRoI->eta(), recRoI->eta()-m_roIWidth, recRoI->eta()+m_roIWidth,
              recRoI->phi(), recRoI->phi()-m_roIWidth, recRoI->phi()+m_roIWidth );
      trigRoIs->push_back( trigRoI );
        
      ATH_MSG_DEBUG( "RoI word: 0x" << MSG::hex << std::setw( 8 ) << roIWord << MSG::dec );      

      auto decision  = TrigCompositeUtils::newDecisionIn( decisionOutput.get() );
      
      for ( auto th: m_emThresholds ) {
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
      

      // TODO would be nice to have this. Requires modifying the TC class: decision->setDetail( "Thresholds", passedThresholds ); // record passing threshold names ( for easy debugging )            
      decision->setObjectLink( "initialRoI", ElementLink<TrigRoiDescriptorCollection>( m_trigRoIsKey.key(), trigRoIs->size()-1 ) );
      decision->setObjectLink( "initialRecRoI", ElementLink<DataVector<LVL1::RecEmTauRoI>>( m_recRoIsKey.key(), recRoIs->size()-1 ) );
    }     
  }
  for ( auto roi: *trigRoIs ) {
    ATH_MSG_DEBUG( "RoI Eta: " << roi->eta() << " Phi: " << roi->phi() << " RoIWord: " << roi->roiWord() );
  }
  // monitoring
  {
    using namespace Monitored;
    auto RoIsCount = MonitoredScalar::declare( "count", trigRoIs->size() );
    auto RoIsPhi   = MonitoredCollection::declare( "phi", *trigRoIs.get(), &TrigRoiDescriptor::phi );
    auto RoIsEta   = MonitoredCollection::declare( "eta", *trigRoIs.get(), &TrigRoiDescriptor::eta );
    MonitoredScope::declare( m_monTool,  RoIsCount, RoIsEta, RoIsPhi );
  }

  ATH_MSG_DEBUG( "Unpacked " <<  trigRoIs->size() << " RoIs" );
  // recording
  {
    SG::WriteHandle<TrigRoiDescriptorCollection> handle( m_trigRoIsKey, ctx );
    CHECK( handle.record ( std::move( trigRoIs ) ) );
  }
  {
    SG::WriteHandle<DataVector<LVL1::RecEmTauRoI>> handle( m_recRoIsKey, ctx );
    CHECK( handle.record( std::move( recRoIs ) ) );    
  }
  {
    auto handle = SG::makeHandle( m_decisionsKey, ctx );
    CHECK ( handle.record( std::move( decisionOutput ), std::move( decisionAux )  ) );
  }

  return StatusCode::SUCCESS; // what else
  

  
}
