/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "HiveAlgBase.h"
#include <thread>

HiveAlgBase::HiveAlgBase( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_hes("HiveExSvc",name),
  m_ccs("CPUCrunchSvc",name) {}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

HiveAlgBase::~HiveAlgBase() {}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode HiveAlgBase::initialize() {
  ATH_MSG_DEBUG("initialize " << name() << " for " << this );

  // retrieve the CPUCrunchSvc if Alg chooses to Crunch instead of Sleep
  if (m_doCrunch) {
    if (m_ccs.retrieve().isFailure()) {
      ATH_MSG_ERROR("unable to retrieve the CPUCrunchSvc");
      return StatusCode::FAILURE;
    }
  }

  if (m_hes.retrieve().isFailure()) {
    ATH_MSG_ERROR("unable to retrieve the HiveExSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

unsigned int
HiveAlgBase::sleep() {

  // add a bit of variability to the sleep/crunch time
  unsigned int sleep = igen( m_time );

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

