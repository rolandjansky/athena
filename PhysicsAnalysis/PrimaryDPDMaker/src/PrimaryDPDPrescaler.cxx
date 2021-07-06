/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "PrimaryDPDMaker/PrimaryDPDPrescaler.h"
#include "AthenaKernel/RNGWrapper.h"
#include "CLHEP/Random/RandFlat.h"
#include <sstream>

PrimaryDPDPrescaler::PrimaryDPDPrescaler(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthReentrantAlgorithm(name, pSvcLocator){
  declareProperty("AcceptAlgs",     m_theAcceptAlgNames );
  declareProperty("RequireAlgs",    m_theRequireAlgNames );
}

StatusCode PrimaryDPDPrescaler::initialize() {
  ATH_MSG_DEBUG( "in initialize()" );
  ATH_CHECK( m_rndmSvc.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode PrimaryDPDPrescaler::execute(const EventContext& ctx) const {
  if (m_prescale <= 1.){
      ATH_MSG_DEBUG("No prescale active. Do nothing");
      return StatusCode::SUCCESS;
  }
  
  ATHRNG::RNGWrapper* rngWrapper = m_rndmSvc->getEngine(this);
  rngWrapper->setSeed( name(), ctx );
  CLHEP::HepRandomEngine* rndmEngine= rngWrapper->getEngine(ctx);
  
  
  const double  random_number = CLHEP::RandFlat::shoot( rndmEngine, 0, 1 );
  if (random_number  * m_prescale > 1.){
      ATH_MSG_DEBUG("The event "<<ctx<<" exceeds the prescale");
      setFilterPassed(false, ctx);
      ++m_vetoed;
  } else {
      ATH_MSG_DEBUG("The event "<<ctx<<" is accepted");
      ++m_accepted;
      setFilterPassed(true,ctx);
  }
  return StatusCode::SUCCESS;
}

StatusCode PrimaryDPDPrescaler::finalize()
{
  ATH_MSG_DEBUG( "***************************************************************");
  ATH_MSG_DEBUG( "Total processed events    : " << m_accepted + m_vetoed);
  ATH_MSG_DEBUG( "Accepted events           : " << m_accepted );
  ATH_MSG_DEBUG( "Vetoed events             : " << m_vetoed );
  ATH_MSG_DEBUG( "Nominal prescale/ratio    : " << m_prescale << " / " << 1./m_prescale );
  if( (m_accepted+m_vetoed)>0 ){
    ATH_MSG_DEBUG( "Accept ratio              : " << 1.*m_accepted/(m_accepted+m_vetoed) );
  }
  else{ ATH_MSG_DEBUG( "Accept ratio              : " << 0 ); }
  ATH_MSG_DEBUG( "File rollover             : " << m_rollover );
  ATH_MSG_DEBUG( "***************************************************************" ); 
  return StatusCode::SUCCESS;
}

