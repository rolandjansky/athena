#include "HiveAlgBase.h"

#include "CxxUtils/make_unique.h"
#include <thread>
#include <chrono>



HiveAlgBase::HiveAlgBase( const std::string& name, 
                      ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_hes("HiveExSvc",name)
{

  declareProperty("Time",m_time=0);

}

HiveAlgBase::~HiveAlgBase() {}

StatusCode HiveAlgBase::initialize() {
  ATH_MSG_DEBUG("initialize " << name() << " for " << this );


  if (m_hes.retrieve().isFailure()) {
    ATH_MSG_ERROR("unable to retrieve the HiveExSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

int
HiveAlgBase::sleep() {

  int sleep = igen( m_time );

  ATH_MSG_INFO("  sleep for: " << sleep << " ms");
  std::this_thread::sleep_for(std::chrono::milliseconds( sleep ));

  m_hes->set( sleep );
  m_hes->add( name(),sleep );

  return sleep;

}

int
HiveAlgBase::sleep_for(int s) {

  int sleep = igen( s );

  ATH_MSG_INFO("  sleep for: " << sleep);

  m_hes->set( sleep );

  return sleep;

}
