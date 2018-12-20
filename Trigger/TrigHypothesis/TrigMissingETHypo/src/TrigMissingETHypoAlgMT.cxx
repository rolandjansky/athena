/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigMissingETHypoAlgMT.h"

#include "GaudiKernel/Property.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthenaMonitoring/MonitoredScope.h"


using namespace TrigCompositeUtils;

using xAOD::TrigMissingETContainer;

//!< Converts from MeV to GeV and them log10, preserving the sign, the minimum dictated by monitoring histograms
float TrigMissingETHypoAlgMT::toLogGeV( const float& x, const float& fallback = 0, const float& epsilon = 1.189 ) {
  const float absXGeV =  std::fabs( x * 1.e-3 );
  if ( absXGeV < epsilon ) 
    return fallback;
  return std::copysign( std::log10( absXGeV ), x );
}

//!< converts to from MeV to GeV if above threshold, else falback value
float TrigMissingETHypoAlgMT::toLinGeV( const float& x, const float& fallback = 0, const float& epsilon = 1e-6 ) {
    const float xGeV = x * 1.e-3;
  if ( xGeV < epsilon ) 
    return fallback;
  return xGeV;
}


TrigMissingETHypoAlgMT::TrigMissingETHypoAlgMT( const std::string& name, 
				      ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ) {}

TrigMissingETHypoAlgMT::~TrigMissingETHypoAlgMT() {}

StatusCode TrigMissingETHypoAlgMT::finalize() {   
  return StatusCode::SUCCESS;
}


StatusCode TrigMissingETHypoAlgMT::initialize() {
  ATH_MSG_INFO ( "Initializing " << name() << "..." );

  CHECK( m_hypoTools.retrieve() );
  CHECK( m_metKey.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode TrigMissingETHypoAlgMT::execute( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );

  // read in the met container
  auto h_met = SG::makeHandle(m_metKey, context );
  ATH_MSG_DEBUG("Retrieving MET from: " << h_met.key());
  ATH_CHECK(h_met.isValid());
  const TrigMissingETContainer* metContainer = h_met.get();

  ATH_MSG_DEBUG("The MET value is " << round(std::hypot((*metContainer)[0]->ex(), (*metContainer)[0]->ey())/1000.0 ));
  
  // read in the previous Decisions made before running this hypo Alg.
  auto h_prevDecisions = SG::makeHandle(decisionInput(), context );
  if( not h_prevDecisions.isValid() ) {//implicit
    ATH_MSG_DEBUG( "No implicit RH for previous decisions "<<  decisionInput().key()<<": is this expected?" );
    return StatusCode::SUCCESS;      
  }
  ATH_MSG_DEBUG( "Decisions being read from " << decisionInput().key() );
  ATH_MSG_DEBUG( "Running with "<< h_prevDecisions->size() <<" implicit ReadHandles for previous decisions");
  ATH_CHECK(h_prevDecisions.isValid());
  auto prevDecisions = h_prevDecisions.get();

  // Make a new Decisions container which will contain the previous
  // decisions, and the one for this hypo.
  auto newDecisions = std::make_unique<DecisionContainer>();
  auto aux = std::make_unique<DecisionAuxContainer>();
  newDecisions->setStore(aux.get());


  // Make trigger decisions and save to "newDecisions"
  CHECK(decide(metContainer, newDecisions, prevDecisions));
  fillMonitoringHistograms(metContainer);

  // Write out decisions
  auto outputHandle = SG::makeHandle(decisionOutput(), context);
  CHECK( outputHandle.record( std::move( newDecisions ), std::move( aux ) ) );
  ATH_MSG_DEBUG ( "Exit with "<<outputHandle->size() <<" decisions");


  // debug....
  if ( msgLvl(MSG::DEBUG)) {
    TrigCompositeUtils::DecisionIDContainer allPassingIDs;
    for ( auto decisionObject: *outputHandle )  {
      TrigCompositeUtils::decisionIDs( decisionObject, allPassingIDs );
    }
    
    for ( TrigCompositeUtils::DecisionID id : allPassingIDs ) {
      ATH_MSG_DEBUG( " +++ " << HLT::Identifier( id ) );
    }
  }

  return StatusCode::SUCCESS;
}



StatusCode TrigMissingETHypoAlgMT::decide(const xAOD::TrigMissingETContainer* metContainer,
                         std::unique_ptr<DecisionContainer>& nDecisions,
                         const DecisionContainer* oDecisions) const{

  ATH_MSG_DEBUG("Deciding" << name() );
  auto previousDecision = (*oDecisions)[0];
  auto newdecision = TrigCompositeUtils::newDecisionIn(nDecisions.get());

  
  const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs{
    TrigCompositeUtils::decisionIDs(previousDecision).begin(), 
      TrigCompositeUtils::decisionIDs( previousDecision ).end()
      };

  //bool allPassed = true;
  for (const auto& tool: m_hypoTools) {
    auto decisionId = tool->getId();
    if (TrigCompositeUtils::passed(decisionId.numeric(), previousDecisionIDs)){
      ATH_MSG_DEBUG("Passed previous trigger");
      bool pass;
      CHECK(tool->decide(metContainer, pass));
      if (pass) {
        ATH_MSG_DEBUG("Passed HypoTools");
    		TrigCompositeUtils::addDecisionID(decisionId, newdecision);
      } else ATH_MSG_DEBUG("Didn't pass HypoTools");
    } ATH_MSG_DEBUG("Didn't pass previous trigger");
  }

  // if (allPassed) fillMonitoringHistograms(metContainer);
  
  return StatusCode::SUCCESS;
}


StatusCode TrigMissingETHypoAlgMT::fillMonitoringHistograms(const xAOD::TrigMissingETContainer* metContainer) const
{
  if (metContainer->size()!=1) return StatusCode::FAILURE;
  auto met = (*metContainer)[0];

  using namespace Monitored;
  auto EF_MEx_log = MonitoredScalar::declare( "EF_MEx_log", toLogGeV( met->ex() ) );
  auto EF_MEy_log = MonitoredScalar::declare( "EF_MEy_log", toLogGeV( met->ey() ) );
  auto EF_MEz_log = MonitoredScalar::declare( "EF_MEz_log", toLogGeV( met->ez() ) );
  auto EF_MET_log = MonitoredScalar::declare( "EF_MET_log", toLogGeV( std::hypot( met->ex(), met->ey() ) ) );
  auto EF_ME_log    = MonitoredScalar::declare( "EF_ME_log",  toLogGeV( std::hypot( std::hypot(met->ex(), met->ey()), met->ez() ) ) );
  auto EF_SumEt_log = MonitoredScalar::declare( "EF_SumEt_log", toLogGeV( met->sumEt() )  );
  auto EF_SumE_log  = MonitoredScalar::declare( "EF_SumE_log",  toLogGeV( met->sumE(), -9e9 ) );
  
  auto EF_MEx_lin = MonitoredScalar::declare( "EF_MEx_lin", toLinGeV( met->ex() ) );
  auto EF_MEy_lin = MonitoredScalar::declare( "EF_MEy_lin", toLinGeV( met->ey() ) );
  auto EF_MEz_lin = MonitoredScalar::declare( "EF_MEz_lin", toLinGeV( met->ez() ) );
  auto EF_MET_lin = MonitoredScalar::declare( "EF_MET_lin", toLinGeV( std::hypot( met->ex(), met->ey() ) ) );          
  auto EF_ME_lin  = MonitoredScalar::declare( "EF_ME_lin",  toLinGeV( std::hypot( std::hypot(met->ex(), met->ey()), met->ez() ) ) );
  auto EF_SumEt_lin =  MonitoredScalar::declare( "EF_SumEt_lin", toLinGeV( met->sumEt() ) );
  auto EF_SumE_lin  = MonitoredScalar::declare( "EF_SumE_lin",  toLinGeV( met->sumE(), -9e9 ) );
  auto EF_XS        = MonitoredScalar::declare( "EF_XS", toLinGeV( std::hypot( met->ex(), met->ey() ) ) / toLinGeV( met->sumEt() ) );
  auto EF_MET_phi   = MonitoredScalar::declare( "EF_MET_phi",   std::atan2( met->ey(), met->ex() ) );

  auto monitorIt = MonitoredScope::declare( m_monTool, 
              EF_MEx_log, EF_MEy_log, EF_MEz_log, EF_MET_log, EF_ME_log, EF_ME_log, EF_SumE_log,
              EF_MEx_lin, EF_MEy_lin, EF_MEz_lin, EF_MET_lin, EF_ME_lin, EF_SumEt_lin, EF_SumE_lin, 
              EF_XS, EF_MET_phi);

  return StatusCode::SUCCESS;
  
}


