/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// L1Decoder includes
#include "RerunRoIsUnpackingTool.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"




RerunRoIsUnpackingTool::RerunRoIsUnpackingTool( const std::string& type, 
						const std::string& name, 
						const IInterface* parent ) 
  :  RoIsUnpackingToolBase( type, name, parent ) { }

RerunRoIsUnpackingTool::~RerunRoIsUnpackingTool() {}

StatusCode RerunRoIsUnpackingTool::initialize() {  
  CHECK( RoIsUnpackingToolBase::initialize() );
  CHECK( m_sourceKey.initialize() );
  CHECK( m_decisionsKey.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode RerunRoIsUnpackingTool::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode RerunRoIsUnpackingTool::updateConfiguration() {
  return StatusCode::SUCCESS;
}

StatusCode RerunRoIsUnpackingTool::unpack(const EventContext& ctx,
					  const ROIB::RoIBResult& ,
					  const HLT::IDSet& activeChains) const {
  using namespace TrigCompositeUtils;
  auto decisionOutput = std::make_unique<DecisionContainer>();
  auto decisionAux    = std::make_unique<DecisionAuxContainer>();
  decisionOutput->setStore( decisionAux.get() );  
  
  auto sourceHandle = SG::makeHandle( m_sourceKey, ctx );

  for ( auto sourceDecision: *sourceHandle.cptr() ) {
    if ( allFailed( sourceDecision ) ) {

      // this is RoI in which no Chain was interested, a candidate to re-enable
      // we need to obtain thresholds and see if the RoI is associated 
      // with any chain in activeChains set 
      if ( not sourceDecision->hasDetail< std::vector<unsigned> > ("thresholds") ) {
	ATH_MSG_DEBUG("No threshold passed, can not re-enable this RoI for re-running");
	continue;
      }
      
      std::vector<unsigned> thresholds;
      sourceDecision->getDetail( "thresholds", thresholds );
      
      ATH_MSG_DEBUG( "Thresholds in this RoI " << thresholds );
      auto decision = newDecisionIn( decisionOutput.get() );
      for ( auto th: thresholds ) {
	ATH_MSG_DEBUG( "Activating because there are some chains to be rerun seeded from the threshold of ID " << th );
	addChainsToDecision( th, decision, activeChains );    
      }
      copyLinks( sourceDecision, decision );

      ATH_MSG_DEBUG( "Candidate for re-running " << ( allFailed( decision ) ? "not to be re-run" : "for re-running")  );      

      
    }
  }
  
  auto handle = SG::makeHandle( m_decisionsKey, ctx );
  CHECK ( handle.record( std::move( decisionOutput ), std::move( decisionAux )  ) );
  


  return StatusCode::SUCCESS;
}



