/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

#include "TBBeamQualityTrackingTool.h"
#include "TBEvent/TBTrack.h"

#include <string>
#include <ios>
#include <algorithm>

TBBeamQualityTrackingTool::TBBeamQualityTrackingTool(const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent)
  : TBBeamQualityTool(type,name,parent)
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
      << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode TBBeamQualityTrackingTool::accept(const std::vector<std::string>& particles)
{//accept
  
  MsgStream log(msgSvc(),name());
  
   log << MSG::DEBUG << "Asked for : "<<particles.size()<<" particles"<<endmsg;
  
  // Retrieve Beam Track and select on chi2 values in x and y

  if (m_chi2cut_u>0. && m_chi2cut_v>0.) {

    TBTrack *thisTBTrack = nullptr;
    ATH_CHECK( evtStore()->retrieve(thisTBTrack,"Track") );
    if (thisTBTrack->getChi2_u() > m_chi2cut_u || thisTBTrack->getChi2_v() > m_chi2cut_v ) {
      log << MSG::DEBUG
          << "Rejecting event with chi2(u,v) = "
          << thisTBTrack->getChi2_u()
          << ", "
          << thisTBTrack->getChi2_v()
          << endmsg;
      return StatusCode::FAILURE;
    } else {
      return StatusCode::SUCCESS;
    }
  } else { 	
    if (m_chi2cut_u==0. && m_chi2cut_v==0.) {
      return StatusCode::SUCCESS;
    } else {
      log << MSG::WARNING << "Improper Track Chi2 Cuts Applied ! : "
	  << "Must specfiy cuts for both X and Y fits"
	  << endmsg;
      return StatusCode::FAILURE;
    }
  }
  // NOT REACHED
}
  
