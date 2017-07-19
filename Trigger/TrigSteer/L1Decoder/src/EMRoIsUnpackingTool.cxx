/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// L1Decoder includes
#include "EMRoIsUnpackingTool.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"


/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
EMRoIsUnpackingTool::EMRoIsUnpackingTool( const std::string& type, 
					  const std::string& name, 
					  const IInterface* parent ) 
  : AthAlgTool ( type, name, parent ),
    m_configSvc( "TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name ),
    m_monTool( "GenericMonitoringTool/MonTool", this ) {

  declareProperty( "Decisions", m_decisionsKey="EMRoIDecisions", "Decisions for each RoI" );
  declareProperty( "ThresholdToChainMapping", m_thresholdToChainProperty, "Mapping from the threshold name to chain in the form: 'EM3 : HLT_e5', 'EM3 : HLT_e5tight', ..., ( note spaces )" );
  declareProperty( "OutputTrigRoIs", m_trigRoIsKey="EMRoIs", "Name of the RoIs object produced by the unpacker" );
  declareProperty( "OutputRecRoIs", m_recRoIsKey="RecEMRoIs", "Name of the RoIs object produced by the unpacker" );
  declareProperty( "RoIWidth", m_roIWidth = 0.1, "Size of RoI in eta/ phi" );
  declareProperty( "MonTool", m_monTool=ToolHandle<GenericMonitoringTool>( "", this ), "Monitoring tool" );
}


EMRoIsUnpackingTool::~EMRoIsUnpackingTool(){
}


StatusCode EMRoIsUnpackingTool::initialize() {  
  CHECK( m_configSvc.retrieve() );
  CHECK( m_decisionsKey.initialize() );
  CHECK( m_trigRoIsKey.initialize() );
  CHECK( m_recRoIsKey.initialize() );
  if ( not m_monTool.name().empty() ) 
    CHECK( m_monTool.retrieve() );

  if ( decodeMapping().isFailure() ) {
    ATH_MSG_ERROR( "Failed to decode threshold to chains mapping, is the format th : chain?" );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode EMRoIsUnpackingTool::updateConfiguration() {
  using namespace TrigConf;

  const ThresholdConfig* thresholdConfig = m_configSvc->thresholdConfig();
  auto filteredThresholds= thresholdConfig->getThresholdVector( L1DataDef::EM );
  ATH_MSG_DEBUG( "Number of filtered thresholds " << filteredThresholds.size() );
  for ( auto th :  filteredThresholds ) {
    if ( th != nullptr ) {
      ATH_MSG_DEBUG( "Found threshold in the configuration: " << th->name() << " of ID: " << HLT::Identifier( th->name() ).numeric() ); 
      m_emThresholds.push_back( th );
    } else {
      ATH_MSG_DEBUG( "Nullptr to the threshold" ); 
    }
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
  auto decisionOutput = std::make_unique<DecisionContainer>();
  auto decisionAux    = std::make_unique<DecisionAuxContainer>();
  decisionOutput->setStore( decisionAux.get() );  
  auto trigRoIs = std::make_unique< TrigRoiDescriptorCollection >();
  auto recRoIs  = std::make_unique< DataVector<LVL1::RecEmTauRoI> >();


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

      auto decision  = TrigCompositeUtils::newDecisionIn( decisionOutput.get() );
      
      for ( auto th: m_emThresholds ) {
	ATH_MSG_VERBOSE( "Checking if the threshold " << th->name() << " passed" );
	if ( recRoI->passedThreshold( th->thresholdNumber() ) ) {
	  ATH_MSG_DEBUG( "Passed Threshold name " << th->name() );
	  addChainsToDecision( HLT::Identifier( th->name() ), decision, activeChains );
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

  ATH_MSG_DEBUG( "Unpackked " <<  trigRoIs->size() << " RoIs" );
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

