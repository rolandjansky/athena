#ifdef REENTRANT_GAUDI

#include "HiveAlgR.h"
#include "CxxUtils/make_unique.h"
#include "GaudiKernel/ServiceHandle.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include <thread>
#include <chrono>
#include <ctime>

DECLARE_COMPONENT(HiveAlgR)

HiveAlgR::HiveAlgR( const std::string& name, 
		    ISvcLocator* pSvcLocator ) : 
  ::AthReentrantAlgorithm( name, pSvcLocator )
  ,m_evt("McEventInfo")
  ,m_wrh1("ar1")
{
  
  declareProperty("Key_W1",m_wrh1);
  declareProperty("EvtInfo",m_evt);

}

HiveAlgR::~HiveAlgR() {}

StatusCode HiveAlgR::initialize() {

  info() << "initialize: " << index() << endmsg;

  ATH_CHECK( m_wrh1.initialize() );
  ATH_CHECK( m_evt.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode HiveAlgR::finalize() {
  info() << "finalize: " << index() << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode HiveAlgR::execute_r(const EventContext& ctx) const {

  info() << "execute_R: " << index() << " on " << ctx << endmsg;


  SG::ReadHandle<EventInfo> evt(m_evt);
  ATH_MSG_INFO("   EventInfo:  r: " << evt->event_ID()->run_number()
               << " e: " << evt->event_ID()->event_number() );


  SG::WriteHandle<HiveDataObj> wh1(m_wrh1);
  ATH_CHECK( wh1.record( CxxUtils::make_unique<HiveDataObj> 
                         ( HiveDataObj(10000 + 
                                       evt->event_ID()->event_number()*100 )))
             );
  ATH_MSG_INFO("  write: " << wh1.key() << " = " << wh1->val() );

  return StatusCode::SUCCESS;

}

#endif // REENTRANT_GAUDI
