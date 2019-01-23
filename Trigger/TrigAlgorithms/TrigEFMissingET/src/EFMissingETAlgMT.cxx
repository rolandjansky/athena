/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include <cmath>
#include "xAODTrigMissingET/TrigMissingETAuxContainer.h"
#include "TrigEFMissingET/EFMissingETHelper.h"
#include "AthenaMonitoring/Monitored.h"
#include "TrigEFMissingET/EFMissingETAlgMT.h"



EFMissingETAlgMT::EFMissingETAlgMT( const std::string& name, 
				    ISvcLocator* pSvcLocator ) 
  : AthReentrantAlgorithm( name, pSvcLocator ) {}

EFMissingETAlgMT::~EFMissingETAlgMT() {}

StatusCode EFMissingETAlgMT::initialize() {
  CHECK( m_metTools.retrieve() );
  CHECK( m_metContainerKey.initialize() );
  CHECK( m_monTool.retrieve() );
  CHECK( m_helperTool.retrieve() );
  return StatusCode::SUCCESS;
}


//!< Converts from MeV to GeV and them log10, preserving the sign, the minimum dictated by monitoring histograms
double EFMissingETAlgMT::toLogGeV( double x, double fallback , double epsilon ) {
  const double absXGeV =  std::fabs( x * 1.e-3 );
  if ( absXGeV < epsilon ) 
    return fallback;
  return std::copysign( std::log10( absXGeV ), x );
}

//!< converts to from MeV to GeV if above threshold, else falback value
double EFMissingETAlgMT::toLinGeV( double x, double fallback , double epsilon ) {
    const double xGeV = x * 1.e-3;
  if ( xGeV < epsilon ) 
    return fallback;
  return xGeV;
}


StatusCode EFMissingETAlgMT::execute( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );
  auto totalTimer = Monitored::Timer( "TIME_Total" );
  auto loopTimer = Monitored::Timer( "TIME_Loop" );

  auto metContainer = std::make_unique<xAOD::TrigMissingETContainer>();
  auto metAuxContainer = std::make_unique<xAOD::TrigMissingETAuxContainer>();
  metContainer->setStore( metAuxContainer.get() );

  auto met = new xAOD::TrigMissingET();
  metContainer->push_back( met );

  TrigEFMissingEtHelper metHelper(42);
  
  loopTimer.start();
  for ( auto& t: m_metTools ) {
    ATH_MSG_DEBUG( "Invoking tool " << t->name() << " to update the MET obejct" );
    t->update( met, &metHelper );
  }
  loopTimer.stop();

  ATH_CHECK( m_helperTool->executeMT(met, &metHelper) );

  auto EF_MEx_log = Monitored::Scalar( "EF_MEx_log", toLogGeV( met->ex() ) );
  auto EF_MEy_log = Monitored::Scalar( "EF_MEy_log", toLogGeV( met->ey() ) );
  auto EF_MEz_log = Monitored::Scalar( "EF_MEz_log", toLogGeV( met->ez() ) );
  auto EF_MET_log = Monitored::Scalar( "EF_MET_log", toLogGeV( std::hypot( met->ex(), met->ey() ) ) );
  auto EF_ME_log    = Monitored::Scalar( "EF_ME_log",  toLogGeV( std::hypot( std::hypot(met->ex(), met->ey()), met->ez() ) ) );
  auto EF_SumEt_log = Monitored::Scalar( "EF_SumEt_log", toLogGeV( met->sumEt() )  );
  auto EF_SumE_log  = Monitored::Scalar( "EF_SumE_log",  toLogGeV( met->sumE(), -9e9 ) );
  
  auto EF_MEx_lin = Monitored::Scalar( "EF_MEx_lin", toLinGeV( met->ex() ) );
  auto EF_MEy_lin = Monitored::Scalar( "EF_MEy_lin", toLinGeV( met->ey() ) );
  auto EF_MEz_lin = Monitored::Scalar( "EF_MEz_lin", toLinGeV( met->ez() ) );
  auto EF_MET_lin = Monitored::Scalar( "EF_MET_lin", toLinGeV( std::hypot( met->ex(), met->ey() ) ) );
  auto EF_ME_lin  = Monitored::Scalar( "EF_ME_lin",  toLinGeV( std::hypot( std::hypot(met->ex(), met->ey()), met->ez() ) ) );
  auto EF_SumEt_lin =  Monitored::Scalar( "EF_SumEt_lin", toLinGeV( met->sumEt() ) );
  auto EF_SumE_lin  = Monitored::Scalar( "EF_SumE_lin",  toLinGeV( met->sumE(), -9e9 ) );
  auto EF_XS        = Monitored::Scalar( "EF_XS", toLinGeV( std::hypot( met->ex(), met->ey() ) ) / toLinGeV( met->sumEt() ) );
  auto EF_MET_phi   = Monitored::Scalar( "EF_MET_phi",   std::atan2( met->ey(), met->ex() ) );
 
  ATH_MSG_INFO("Event MET: "  << std::hypot( met->ex(), met->ey() ) << " MeV");

  auto monitorIt = Monitored::Group( m_monTool,
					    totalTimer, loopTimer,
					    EF_MEx_log, EF_MEy_log, EF_MEz_log, EF_MET_log, EF_ME_log, EF_ME_log, EF_SumE_log,
					    EF_MEx_lin, EF_MEy_lin, EF_MEz_lin, EF_MET_lin, EF_ME_lin, EF_SumEt_lin, EF_SumE_lin, 
					    EF_XS, EF_MET_phi);
    
  auto handle = SG::makeHandle( m_metContainerKey, context );
  CHECK( handle.record( std::move( metContainer ), std::move ( metAuxContainer ) ) );
  return StatusCode::SUCCESS;
}


