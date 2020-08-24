/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigLoFRemovalHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigLongLivedParticlesHypo
//
// AUTHOR: A. Policicchio <apolici@cern.ch>
//
// ********************************************************************
//
#include <algorithm>
#include <list>
#include <iterator>
#include <sstream>
#include <cmath>
//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/Enums.h"

#include "TrigLongLivedParticlesHypo/TrigLoFRemovalHypo.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"





class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigLoFRemovalHypo::TrigLoFRemovalHypo(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {

  declareProperty("AcceptAll",      m_acceptAll=false);
  declareProperty("LoFCellContSize",      m_LoFCellContSize=4);

  // Monitored variables...
  declareMonitoredVariable("CutCounter", m_cutCounter);
  declareMonitoredVariable("CellContainerSize", m_CellContainerSize);

}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigLoFRemovalHypo::~TrigLoFRemovalHypo()
{  }

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
// Note that it is NOT NECESSARY to run the initialize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////
//

// ----------------------------------------------------------------------
HLT::ErrorCode TrigLoFRemovalHypo::hltInitialize()
  // ----------------------------------------------------------------------
{
  msg() << MSG::INFO << "in initialize()" << endmsg;

  
  m_accepted=0;
  m_rejected=0;
  m_errors=0;
  
  return HLT::OK;
  
}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigLoFRemovalHypo::hltFinalize(){
  // ----------------------------------------------------------------------

  msg() << MSG::INFO << "in finalize()" << endmsg;
  msg() << MSG::INFO << "Events accepted/rejected/errors:  "<< m_accepted <<" / "<<m_rejected<< " / "<< m_errors<< endmsg;
  return HLT::OK;

}


// ----------------------------------------------------------------------
HLT::ErrorCode TrigLoFRemovalHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  // ----------------------------------------------------------------------



  if(msgLvl() <= MSG::DEBUG) {
     msg() << MSG::DEBUG << " in hltExecute() " << endmsg;
  }
  m_cutCounter = -1;

  pass=false;

  //retrieve LoF cell container here 
  std::string cellCollKey  = "TrigCaloCellLoF";
  const CaloCellContainer* theLoFCellCont = 0;
  if( getFeature(outputTE, theLoFCellCont, cellCollKey) != HLT::OK) {
    msg() << MSG::WARNING << "Failed to get TrigLoFCells" << endmsg;
    return HLT::OK;
  }

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << " Retrieved a Cell Container of Size= " << theLoFCellCont->size() << endmsg;
  }

  //Use this to accept or reject events. 
  bool this_event_is_a_LoF_event = false;
  if(int(theLoFCellCont->size()) >= m_LoFCellContSize){
    this_event_is_a_LoF_event=true;
  }

  //monitoring
  m_CellContainerSize = (float)theLoFCellCont->size();


  m_cutCounter = 0;
  //Cut on LoF here 
  if(m_acceptAll || !this_event_is_a_LoF_event) {
    pass = true;
    m_accepted++;
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Not LoF event: accepted!" << endmsg;

  } else {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " LoF event: rejected!!" << endmsg;
    m_rejected++;
    m_cutCounter=1;
  }

  return HLT::OK;

}
