/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file TRT_CalDbTool.cxx
 *  @brief Tool to manage TRT Conditions data during normal reconstruction
 *  @author Peter Hansen <phansen@nbi.dk>, Wouter Hulsberger <whulsber@cern.ch>
 **/

#include "TRT_CalDbTool.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "StoreGate/ReadCondHandle.h"


TRT_CalDbTool::TRT_CalDbTool( const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type, name, parent),
    m_trtid(0),
    m_detstore("DetectorStore",name),
    m_mutex{},
    m_Rtcache{},
    m_T0cache{},
    m_Errcache{},
    m_Slopecache{},
    m_isGEANT4(true),
    m_par_rtcontainerkey("/TRT/Calib/RT"),
    m_par_errcontainerkey("/TRT/Calib/errors2d"),
    m_par_slopecontainerkey("/TRT/Calib/slopes"),
    m_par_t0containerkey("/TRT/Calib/T0"),
    m_rtContainerG4(nullptr),
    m_errContainerG4(nullptr),
    m_slopeContainerG4(nullptr),
    m_t0ContainerG4(nullptr)
{
  declareProperty("DetectorStore",m_detstore);
  declareProperty("isGEANT4",m_isGEANT4);
}


StatusCode TRT_CalDbTool::initialize() 
{
  ATH_MSG_DEBUG( " in initialize " );

  // Get StoreGate access to DetectorStore
  if (StatusCode::SUCCESS!=m_detstore.retrieve()) {
    msg(MSG::FATAL) << "Unable to retrieve " << m_detstore.name() << endmsg;
    return StatusCode::FAILURE;
  }

  // Get the TRT ID helper
  StatusCode sc = m_detstore->retrieve(m_trtid,"TRT_ID");
  if(sc.isFailure()) {
    msg(MSG::FATAL) << "Problem retrieving TRTID helper" << endmsg;
    return StatusCode::FAILURE;
  }

  // Read keys

  ATH_CHECK( m_rtReadKey.initialize() );
  ATH_CHECK( m_errReadKey.initialize() );
  ATH_CHECK( m_slopeReadKey.initialize() );
  ATH_CHECK( m_t0ReadKey.initialize() );
 
  if(m_isGEANT4) {
    // processing GEANT4 simulation - revert to old non-MT style cond access

      msg(MSG::INFO) << "TRT_CalDbTool::initialize for simulation"  << endmsg;
      if(StatusCode::SUCCESS!=m_detstore->retrieve(m_rtContainerG4,m_par_rtcontainerkey)) {
        ATH_MSG_FATAL("Could not retrieve folder " << m_par_rtcontainerkey);
        return StatusCode::FAILURE;
      }


      if(StatusCode::SUCCESS!=m_detstore->retrieve(m_errContainerG4,m_par_errcontainerkey)) {
        ATH_MSG_FATAL("Could not retrieve folder " << m_par_errcontainerkey);
        return StatusCode::FAILURE;
      }

      if(StatusCode::SUCCESS!=m_detstore->retrieve(m_slopeContainerG4,m_par_slopecontainerkey)) {
        ATH_MSG_FATAL("Could not retrieve folder " << m_par_slopecontainerkey);
        return StatusCode::FAILURE;
      }

      if(StatusCode::SUCCESS!=m_detstore->retrieve(m_t0ContainerG4,m_par_t0containerkey)) {
        ATH_MSG_FATAL("Could not retrieve folder " << m_par_t0containerkey);
        return StatusCode::FAILURE;
      }

      if(StatusCode::SUCCESS!=m_detstore->retrieve(m_slopeContainerG4,m_par_slopecontainerkey)) {
        ATH_MSG_FATAL("Could not retrieve folder " << m_par_slopecontainerkey);
        return StatusCode::FAILURE;
      }


      if(StatusCode::SUCCESS!=m_detstore->retrieve(m_errContainerG4,m_par_errcontainerkey)) {
        ATH_MSG_FATAL("Could not retrieve folder " << m_par_errcontainerkey);
        return StatusCode::FAILURE;
      }


  } else {

     msg(MSG::INFO) << " TRT_CalDbTool::initialize for data "  << endmsg;
  }
  return StatusCode::SUCCESS;
}



StatusCode TRT_CalDbTool::finalize()
{
  ATH_MSG_DEBUG("TRT_CalDbTool finalize method called");
  return StatusCode::SUCCESS;
}


const TRT_CalDbTool::RtRelationContainer* TRT_CalDbTool::getRtContainer() const {

  if(!m_isGEANT4) {
    const EventContext& ctx{Gaudi::Hive::currentContext()};
    static const EventContext::ContextEvt_t invalidValue{EventContext::INVALID_CONTEXT_EVT};
    EventContext::ContextID_t slot{ctx.slot()};
    EventContext::ContextEvt_t evt{ctx.evt()};
    if (slot>=m_Rtcache.size()) {              // new slot?
      std::lock_guard<std::mutex> lock{m_mutex};
      m_Rtcache.resize(slot+1, invalidValue); // Store invalid event id in order to pass the next IF statement.
    }
    if (m_Rtcache[slot]!=evt) {                // not same event as last call
      std::lock_guard<std::mutex> lock{m_mutex};
      SG::ReadCondHandle<RtRelationContainer> rtc(m_rtReadKey); // find the right conditions
      if (not rtc.isValid()) {
       ATH_MSG_ERROR("Failed to retrieve " << m_rtReadKey.key());
      }
      m_condRt.set(*rtc);                                      // cache the pointer
      m_Rtcache[slot] = evt;                                   // store cached event id
    }
    return m_condRt.get();
  } else {
    return m_rtContainerG4;
  }
}

const TRT_CalDbTool::RtRelationContainer* TRT_CalDbTool::getErrContainer() const {


  if(!m_isGEANT4) {
    const EventContext& ctx{Gaudi::Hive::currentContext()};
    static const EventContext::ContextEvt_t invalidValue{EventContext::INVALID_CONTEXT_EVT};
    EventContext::ContextID_t slot{ctx.slot()};
    EventContext::ContextEvt_t evt{ctx.evt()};
    if (slot>=m_Errcache.size()) {              // new slot?
      std::lock_guard<std::mutex> lock{m_mutex};
      m_Errcache.resize(slot+1, invalidValue); // Store invalid event id in order to pass the next IF statement.
    }
    if (m_Errcache[slot]!=evt) {                // not same event as last call
      std::lock_guard<std::mutex> lock{m_mutex};
      SG::ReadCondHandle<RtRelationContainer> rtc(m_errReadKey); // find the right conditions
      if (not rtc.isValid()) {
        ATH_MSG_ERROR("Failed to retrieve " << m_errReadKey.key());
      }
      m_condErr.set(*rtc);                                      // cache the pointer
      m_Errcache[slot] = evt;                                   // store cached event id
    }
    return m_condErr.get();
  } else {

    return m_errContainerG4;
  }
}

const TRT_CalDbTool::RtRelationContainer* TRT_CalDbTool::getSlopeContainer() const {

  if(!m_isGEANT4) {
    const EventContext& ctx{Gaudi::Hive::currentContext()};
    static const EventContext::ContextEvt_t invalidValue{EventContext::INVALID_CONTEXT_EVT};
    EventContext::ContextID_t slot{ctx.slot()};
    EventContext::ContextEvt_t evt{ctx.evt()};
    if (slot>=m_Slopecache.size()) {              // new slot?
      std::lock_guard<std::mutex> lock{m_mutex};
      m_Slopecache.resize(slot+1, invalidValue); // Store invalid event id in order to pass the next IF statement.
    }
    if (m_Slopecache[slot]!=evt) {                // not same event as last call
      std::lock_guard<std::mutex> lock{m_mutex};
      SG::ReadCondHandle<RtRelationContainer> rtc(m_slopeReadKey); // find the right conditions
      if (not rtc.isValid()) {
        ATH_MSG_ERROR("Failed to retrieve " << m_slopeReadKey.key());
      }
      m_condSlope.set(*rtc);                                      // cache the pointer
      m_Slopecache[slot] = evt;                                   // store cached event id
    }
    return m_condSlope.get();
  } else {

    return m_slopeContainerG4;
  }

}

const TRT_CalDbTool::StrawT0Container* TRT_CalDbTool::getT0Container() const {

  if(!m_isGEANT4) {
    const EventContext& ctx{Gaudi::Hive::currentContext()};
    static const EventContext::ContextEvt_t invalidValue{EventContext::INVALID_CONTEXT_EVT};
    EventContext::ContextID_t slot{ctx.slot()};
    EventContext::ContextEvt_t evt{ctx.evt()};
    if (slot>=m_T0cache.size()) {              // new slot?
      std::lock_guard<std::mutex> lock{m_mutex};
      m_T0cache.resize(slot+1, invalidValue); // Store invalid event id in order to pass the next IF statement.
    }
    if (m_T0cache[slot]!=evt) {                // not same event as last call
      std::lock_guard<std::mutex> lock{m_mutex};
      SG::ReadCondHandle<StrawT0Container> rtc(m_t0ReadKey); // find the right conditions
      if (not rtc.isValid()) {
        ATH_MSG_ERROR("Failed to retrieve " << m_t0ReadKey.key());
      }
      m_condT0.set(*rtc);                                      // cache the pointer
      m_T0cache[slot] = evt;                                   // store cached event id
    }
    return m_condT0.get();
  } else {
    return m_t0ContainerG4;
  }

}



double TRT_CalDbTool::driftRadius(const double& time, float& t0, const Identifier& ident,bool& found) const
{
  // Returns a drift radius, a t0 and a success indicator,
  // given an identifier and a time given by (leading_edge_bin+0.5)*3.125ns
  found=true;
  t0 = this->getT0(ident);
  const TRTCond::RtRelation* rtr = getRtRelation(ident) ;
  double radius = 0;
  if (rtr != 0)
     radius = rtr->radius( time - t0 );
  else
     msg(MSG::FATAL) << " cannot find an rt-relation for TRT layer_or_wheel " <<  m_trtid->layer_or_wheel(ident) << " Please check IOV ranges " << endmsg ;
  
  if(msgLvl(MSG::VERBOSE)) msg() << " time " << time << " t0 " << t0 << " t " << time-t0
                                 << " radius " << radius << endmsg;
  //
  if(      radius<0 ) radius=0 ;
  else if( radius>2.) radius=2.;

  // add protection for the turnover:
  if (time - t0 > 55){
    		if(msgLvl(MSG::VERBOSE)) msg() << " time " << time << " t0 " << t0 << " t " << time-t0  << " > 55, check Rt derivative" << endmsg;
		// Check Second Derivative.
		if (rtr != 0){
			if (rtr->drdt( time - t0 ) < 0 ){
	    		if(msgLvl(MSG::VERBOSE)) msg() << " time " << time << " t0 " << t0 << " t " << time-t0  << " and rt derivative: " <<  rtr->drdt( time - t0 )   << endmsg;
			radius=2.;
			}
		}
  }
  return radius;
}

double TRT_CalDbTool::driftError( const double& time, const Identifier& ident,bool& found) const
{
  // Returns an error on drift radius and a success indicator,
  // given an identifier and a drift-time in ns
  found=true;
  const TRTCond::RtRelation* rtr = getErrors(ident) ;
  double error=0.;
  if(rtr) {
    error = rtr->radius( time );
  } else {
    found=false;
    return 0;
  }
  if(msgLvl(MSG::VERBOSE)) msg() << " time " << time  
				 << " error on radius " << error << endmsg;
  return error;
}

double TRT_CalDbTool::driftSlope( const double& time, const Identifier& ident,bool& found) const
{
  // Returns an error on drift radius and a success indicator,
  // given an identifier and a drift-time in ns
  found=true;
  const TRTCond::RtRelation* rtr = getSlopes(ident) ;
  double slope=0.;
  if(rtr) {
    slope = rtr->radius( time );
  } else {
    found=false;
    return 0;
  }
  if(msgLvl(MSG::VERBOSE)) msg() << " time " << time
                                 << " slope on radius " << slope << endmsg;
  return slope;
}




