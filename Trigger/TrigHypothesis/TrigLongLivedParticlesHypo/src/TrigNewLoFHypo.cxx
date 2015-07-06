/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include <list>
#include <iterator>
#include <sstream>
#include <cmath>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigOperationalInfo.h"

#include "TrigLongLivedParticlesHypo/TrigNewLoFHypo.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"

#include "CLHEP/Units/SystemOfUnits.h"


class ISvcLocator;

TrigNewLoFHypo::TrigNewLoFHypo(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {

  declareProperty("AcceptAll",       m_acceptAll=false);
  declareProperty("LoFCellContSize", m_LoFCellContSize=4);

  // Monitored variables...
  declareMonitoredVariable("CutCounter", m_cutCounter);
  declareMonitoredVariable("CellContainerSize", m_CellContainerSize);

}

TrigNewLoFHypo::~TrigNewLoFHypo() {}

HLT::ErrorCode TrigNewLoFHypo::hltInitialize() {

  msg() << MSG::INFO << "in initialize()" << endreq;
  
  m_accepted=0;
  m_rejected=0;
  m_errors=0;
  
  return HLT::OK;
}

HLT::ErrorCode TrigNewLoFHypo::hltFinalize(){

  msg() << MSG::INFO << "in finalize()" << endreq;
  msg() << MSG::INFO << "Events accepted/rejected/errors:  "<< m_accepted <<" / "<<m_rejected<< " / "<< m_errors<< endreq;
  return HLT::OK;

}


HLT::ErrorCode TrigNewLoFHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {

  if(msgLvl() <= MSG::DEBUG)
     msg() << MSG::DEBUG << " in hltExecute() " << endreq;

  m_cutCounter = -1;

  pass=false;

  //* Get TrigOperationalInfo *//
  std::vector<const TrigOperationalInfo*> m_vectorOperationalInfo;
  if (getFeatures(outputTE, m_vectorOperationalInfo, "CellLoFInfo") != HLT::OK) {
    if (msgLvl() <= MSG::WARNING) {
      msg() << MSG::WARNING << "Failed to get TrigOperationalInfo" << endreq;
    }
    return HLT::MISSING_FEATURE;
  }
  else {
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Number of TrigOperationalInfo objects: " << m_vectorOperationalInfo.size() << endreq;
    }
  }

  std::vector<const TrigOperationalInfo*>::const_iterator m_operationalInfo;
  for (m_operationalInfo=m_vectorOperationalInfo.begin(); m_operationalInfo!=m_vectorOperationalInfo.end(); ++m_operationalInfo) {   
    
    if ((*m_operationalInfo)->defined("CellLoF")==1) {
   
      unsigned int m_size = (*m_operationalInfo)->infos().first.size();   
      if (m_size!=1) {
        if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Too much information from the same TrigOperationalInfo object" << endreq;
        return HLT::NAV_ERROR;
      }
   
      m_numberCells = (*m_operationalInfo)->get("CellLoF");
   
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Number of calo cells passing LoF quality criteria: " << m_numberCells << endreq;
    }
  }

  bool this_event_is_a_LoF_event = false;
  if(int(m_numberCells) >= m_LoFCellContSize){
    this_event_is_a_LoF_event=true;
  }
  
  //monitoring
  m_CellContainerSize = (float)m_numberCells;

  m_cutCounter = 0;
  //Cut on LoF here 
  if(m_acceptAll || !this_event_is_a_LoF_event) {
    pass = true;
    m_accepted++;
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Not LoF event: accepted!" << endreq;

  } else {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " LoF event: rejected!!" << endreq;
    m_rejected++;
    m_cutCounter=1;
  }

  return HLT::OK;

}
