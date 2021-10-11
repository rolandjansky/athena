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

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "StoreGate/ReadCondHandle.h"


TRT_CalDbTool::TRT_CalDbTool( const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type, name, parent),
    m_trtId(nullptr)
{}


StatusCode TRT_CalDbTool::initialize() 
{
  ATH_MSG_DEBUG( " in initialize " );


  // Get the TRT ID helper
  StatusCode sc = detStore()->retrieve(m_trtId,"TRT_ID");
  if(sc.isFailure()) {
    ATH_MSG_FATAL("Problem retrieving TRTID helper");
    return StatusCode::FAILURE;
  }

  // Read keys

  ATH_CHECK( m_rtReadKey.initialize() );
  ATH_CHECK( m_errReadKey.initialize() );
  ATH_CHECK( m_slopeReadKey.initialize() );
  ATH_CHECK( m_t0ReadKey.initialize() );
 
  ATH_MSG_DEBUG(" TRT_CalDbTool::initialized ");

  return StatusCode::SUCCESS;
}



StatusCode TRT_CalDbTool::finalize()
{
  ATH_MSG_DEBUG("TRT_CalDbTool finalize method called");
  return StatusCode::SUCCESS;
}


const TRT_CalDbTool::RtRelationContainer* TRT_CalDbTool::getRtContainer() const {

  SG::ReadCondHandle<RtRelationContainer> rtc(m_rtReadKey);
  return *rtc;
}

const TRT_CalDbTool::RtRelationContainer* TRT_CalDbTool::getErrContainer() const {

  SG::ReadCondHandle<RtRelationContainer> rtc(m_errReadKey);
  return *rtc;
}

const TRT_CalDbTool::RtRelationContainer* TRT_CalDbTool::getSlopeContainer() const {

  SG::ReadCondHandle<RtRelationContainer> rtc(m_slopeReadKey); // find the right conditions
  return *rtc;
}

const TRT_CalDbTool::StrawT0Container* TRT_CalDbTool::getT0Container() const {

  SG::ReadCondHandle<StrawT0Container> rtc(m_t0ReadKey); // find the right conditions
  return *rtc;
}



double TRT_CalDbTool::driftRadius(const double& time, float& t0, const Identifier& ident,bool& found) const
{
  // Returns a drift radius, a t0 and a success indicator,
  // given an identifier and a time given by (leading_edge_bin+0.5)*3.125ns
  found=true;
  t0 = this->getT0(ident);
  const TRTCond::RtRelation* rtr = getRtRelation(ident) ;
  double radius = 0;
  if (rtr != nullptr)
     radius = rtr->radius( time - t0 );
  else
    ATH_MSG_FATAL(" cannot find an rt-relation for TRT layer_or_wheel " <<  m_trtId->layer_or_wheel(ident) << " Please check IOV ranges ");
  
  ATH_MSG_VERBOSE(" time " << time << " t0 " << t0 << " t " << time-t0 << " radius " << radius);
  //
  if(      radius<0 ) radius=0 ;
  else if( radius>2.) radius=2.;

  // add protection for the turnover:
  if (time - t0 > 55){
    ATH_MSG_VERBOSE(" time " << time << " t0 " << t0 << " t " << time-t0  << " > 55, check Rt derivative");
    // Check Second Derivative.
    if (rtr != nullptr){
      if (rtr->drdt( time - t0 ) < 0 ){
	ATH_MSG_VERBOSE(" time " << time << " t0 " << t0 << " t " << time-t0  << " and rt derivative: " <<  rtr->drdt( time - t0 ));
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
  ATH_MSG_VERBOSE(" time " << time  << " error on radius " << error);
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
  ATH_MSG_VERBOSE(" time " << time << " slope on radius " << slope);
  return slope;
}




