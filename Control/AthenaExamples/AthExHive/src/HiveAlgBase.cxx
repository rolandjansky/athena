/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "HiveAlgBase.h"
#include <thread>

#include "AthenaKernel/RNGWrapper.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandFlat.h"

HiveAlgBase::HiveAlgBase( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_hes("HiveExSvc",name),
  m_ccs("CPUCrunchSvc",name),
  m_rngSvc("AthRNGSvc", name) {}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

HiveAlgBase::~HiveAlgBase() {}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode HiveAlgBase::initialize() {
  ATH_MSG_DEBUG("initialize " << name() << " for " << this );

  // retrieve the CPUCrunchSvc if Alg chooses to Crunch instead of Sleep
  if (m_doCrunch) {
    ATH_CHECK( m_ccs.retrieve() );
  }

  ATH_CHECK( m_hes.retrieve() );
  ATH_CHECK( m_rngSvc.retrieve() );

  return StatusCode::SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

unsigned int
HiveAlgBase::sleep() {

  auto ctx = Gaudi::Hive::currentContext();
  ATHRNG::RNGWrapper* rngWrapper = m_rngSvc->getEngine(this);
  rngWrapper->setSeed( name(), ctx );

  // add a bit of variability to the sleep/crunch time
  const unsigned int sleep = CLHEP::RandFlat::shoot(rngWrapper->getEngine(ctx), 0, m_time);

  if (m_doCrunch) {
    ATH_MSG_INFO("  crunch for: " << sleep << " ms");
    m_ccs->crunch_for( std::chrono::milliseconds(sleep) );
  } else {
    ATH_MSG_INFO("  sleep for: " << sleep << " ms");
    std::this_thread::sleep_for(std::chrono::milliseconds( sleep ));
  }

  // accumulate data in the HiveExSvc
  m_hes->add( name(),sleep );

  return sleep;

}

