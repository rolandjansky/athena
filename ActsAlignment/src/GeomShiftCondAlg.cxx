/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsAlignment/GeomShiftCondAlg.h"
#include "AthExHive/IASCIICondDbSvc.h"
#include "ActsAlignment/ShiftCondObj.h"

#include "StoreGate/WriteCondHandle.h"

#include "GaudiKernel/ServiceHandle.h"

#include "GaudiKernel/EventIDBase.h"
#include "GaudiKernel/EventIDRange.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include <thread>
#include <chrono>
#include <memory>

GeomShiftCondAlg::GeomShiftCondAlg( const std::string& name, 
            ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_cs("CondSvc",name)
{
}

GeomShiftCondAlg::~GeomShiftCondAlg() {}

StatusCode GeomShiftCondAlg::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  ATH_CHECK( m_evt.initialize() );

  if (m_cs.retrieve().isFailure()) {
    ATH_MSG_ERROR("unable to retrieve CondSvc");
  }

  //if (m_cds.retrieve().isFailure()) {
    //ATH_MSG_ERROR("unable to retrieve ASCIICondDbSvc");
  //}

  m_wchk.setDbKey(m_dbKey);

  if (m_wchk.initialize().isFailure()) {
    ATH_MSG_ERROR("unable to initialize WriteCondHandle with key" << m_wchk.key() );
    return StatusCode::FAILURE;
  }

  if (m_cs->regHandle(this, m_wchk).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_wchk.fullKey() 
                  << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode GeomShiftCondAlg::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

StatusCode GeomShiftCondAlg::execute() {
  ATH_MSG_DEBUG("execute " << name());

  SG::ReadHandle<EventInfo> evt( m_evt );
  if (!evt.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve EventInfo");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("   EventInfo:  r: " << evt->event_ID()->run_number()
                << " e: " << evt->event_ID()->event_number() );


  SG::WriteCondHandle<ShiftCondObj> wch(m_wchk);

  EventIDBase now(getContext().eventID());

  if (evt->event_ID()->event_number() == 10) {
      std::this_thread::sleep_for(std::chrono::milliseconds( 500 ));
  }

  // do we have a valid m_wch for current time?
  if ( wch.isValid(now) ) {
    ATH_MSG_DEBUG("CondHandle is already valid for " << now
		  << ". In theory this should not be called, but may happen"
		  << " if multiple concurrent events are being processed out of order.");

  } else {

    ATH_MSG_DEBUG("  CondHandle " << wch.key() 
                  << " not valid now (" << now << "). Getting new info for dbKey \"" 
                  << wch.dbKey() << "\" from CondDb");

    //IASCIICondDbSvc::dbData_t val;
    //if (m_cds->getRange(wch.dbKey(), getContext(), r, val).isFailure()) {
      //ATH_MSG_ERROR("  could not find dbKey \"" << wch.dbKey() 
        //<< "\" in CondSvc registry");
      //return StatusCode::FAILURE;
    //}

    ATH_MSG_ALWAYS("SG evt: " << *(evt->event_ID()));

    ATH_MSG_DEBUG("LB EventInfo = " << evt->event_ID()->lumi_block());
    ATH_MSG_DEBUG("LB context = " << getContext().eventID().lumi_block());

    unsigned int intvl_length = 1000;
    //unsigned int intvl_num = evt->event_ID()->event_number() / intvl_length + 1;
    unsigned int intvl_num = evt->event_ID()->lumi_block();
    std::cout << "intvl_num=" << intvl_num << std::endl;
    //int intvl_start = intvl_num * intvl_length;
    //int intvl_end = (intvl_num+1) * intvl_length;

    //double* shift = new double(intvl_num * 10);
    double val = intvl_num * 10;

    //EventIDBase start(1, intvl_start);
    //EventIDBase end(1, intvl_end);
    EventIDBase start(1, EventIDBase::UNDEFEVT);
    EventIDBase end(1, EventIDBase::UNDEFEVT);

    start.set_lumi_block(intvl_num);
    //start.set_time_stamp(0);
    
    end.set_lumi_block(intvl_num+1);
    //end.set_time_stamp(0);

    EventIDRange r(start, end);

    ATH_MSG_ALWAYS("now >= start: " << (now >= start));
    ATH_MSG_ALWAYS("now < end: " << (now < end));
    ATH_MSG_ALWAYS("in range manual: " << (now >= r.start() && now < r.stop() ));

    ATH_MSG_ALWAYS("Is now (" << now << ") in range r (" << r << ")? " << r.isInRange( now ));

    ATH_MSG_ALWAYS("make shift: range = " << r << " val = " << val );
    ATH_MSG_ALWAYS("start: " << start);
    ATH_MSG_ALWAYS("end: " << end);

    ShiftCondObj* cdo = new ShiftCondObj( val );
    if (wch.record(r, cdo).isFailure()) {
      ATH_MSG_ERROR("could not record shift " << wch.key() 
		    << " = " << *cdo
                    << " with EventRange " << r);
      return StatusCode::FAILURE;
    }

    // is it valid now?
    ATH_MSG_ALWAYS("isValid now? " << wch.isValid(now));


    ATH_MSG_INFO("recorded new shift " << wch.key() << " = " << *cdo 
		 << " with range " << r);
  }



  return StatusCode::SUCCESS;

}

