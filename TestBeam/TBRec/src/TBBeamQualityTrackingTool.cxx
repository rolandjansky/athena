/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#####################################################
//#                                                   #
//# File   : TBBeamQualityTrackingTool.cxx            #
//# Package: TBRec                                    #
//#                                                   #
//# Author : P.Krieger <krieger@physics.utoronto.ca>  #
//# Based on existing BQ tools from M.Bieri           #
//# Last Modified: Jun 30/2005                        #
//##################################################### 

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/AlgTool.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/Property.h"

#include "TBEvent/TBTrack.h"

#include "TBRec/TBBeamQualityTrackingTool.h"

#include <string>
#include <ios>
#include <algorithm>

TBBeamQualityTrackingTool::TBBeamQualityTrackingTool(const std::string& name,
						       const std::string& type,
						       const IInterface* parent)
  : TBBeamQualityTool(name,type,parent),
    m_StoreGate(nullptr)
{  

  // defaults are 0, which means no cut.
  // if cutting, must specify both.
  declareInterface<TBBeamQualityTool>(this);
  declareProperty("Chi2Cut_x",m_chi2cut_u=0.);
  declareProperty("Chi2Cut_y",m_chi2cut_v=0.);

}

TBBeamQualityTrackingTool::~TBBeamQualityTrackingTool()
{ }

StatusCode TBBeamQualityTrackingTool::initializeTool()
{
   
  MsgStream log(msgSvc(),name());
  
  log << MSG::DEBUG
      << "initializing TBBeamQualityTrackingTool"
      << endreq;

  return StatusCode::SUCCESS;
}

StatusCode TBBeamQualityTrackingTool::accept(std::vector<std::string> m_particles)
{//accept
  
  MsgStream log(msgSvc(),name());
  
  StatusCode sc = service("StoreGateSvc",m_StoreGate);
  if ( sc.isFailure() )
    {
      log << MSG::ERROR
          << "Cannot alllocate StoreGate service!"
          << endreq;
    }
   log << MSG::DEBUG << "Asked for : "<<m_particles.size()<<" particles"<<endreq;
  
  // Retrieve Beam Track and select on chi2 values in x and y

  if (m_chi2cut_u>0. && m_chi2cut_v>0.) {

    TBTrack *thisTBTrack;
    sc = m_StoreGate->retrieve(thisTBTrack,"Track");
    if (sc.isFailure()){
      log << MSG::WARNING
	  << "Retrieval of TBTrack failed"
	  << endreq;
      return StatusCode::FAILURE;
    } else {
      //          log << MSG::INFO
      //    << "Retrieved track for quality check"
      //    << "chi2(u,v) = "
      //    << thisTBTrack->getChi2_u()
      //    << ", "
      //    << thisTBTrack->getChi2_v()
      //    << endreq;
      if (thisTBTrack->getChi2_u() > m_chi2cut_u || thisTBTrack->getChi2_v() > m_chi2cut_v ) {
	log << MSG::DEBUG
	    << "Rejecting event with chi2(u,v) = "
	    << thisTBTrack->getChi2_u()
	    << ", "
	    << thisTBTrack->getChi2_v()
	    << endreq;
	return StatusCode::FAILURE;
      } else {
	return StatusCode::SUCCESS;
      }
    }
  } else { 	
    if (m_chi2cut_u==0. && m_chi2cut_v==0.) {
      return StatusCode::SUCCESS;
    } else {
      log << MSG::WARNING << "Improper Track Chi2 Cuts Applied ! : "
	  << "Must specfiy cuts for both X and Y fits"
	  << endreq;
      return StatusCode::FAILURE;
    }
  }
  // NOT REACHED
}
  
