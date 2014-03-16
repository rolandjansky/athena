/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigExMTHistNtup/MTHist.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogram1D.h"

#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ThreadGaudi.h"


/////////////////////////////////////////////////////////////////////////////

MTHist::MTHist(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator), m_hist(0), m_StoreGateSvc(0), m_previousEvent(0)
{
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MTHist::initialize(){

  StatusCode sc = StatusCode::SUCCESS;
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;

#ifdef ATLAS_GAUDI_V21
  SmartIF<IService> tmp_msgSvc(msgSvc());
  if(tmp_msgSvc.isValid()) {
    log << MSG::INFO << " Algorithm = " << name() << " is connected to Message Service = "
        << tmp_msgSvc->name() << endreq;
  }

  // Register the histogram
  SmartIF<IService> tmp_histSvc(histoSvc());
  if(tmp_histSvc.isValid()) {
    log << MSG::INFO << " Algorithm = " << name() << " is connected to Histogram Service = "
        << tmp_histSvc->name() << endreq;
  }
#else
  Service* tmp_msgSvc = dynamic_cast<Service*> (msgSvc());
  if(tmp_msgSvc != 0) {
    log << MSG::INFO << " Algorithm = " << name() << " is connected to Message Service = "
        << tmp_msgSvc->name() << endreq;
  }

  // Register the histogram
  Service* tmp_histSvc = dynamic_cast<Service*> (histoSvc());
  if(tmp_histSvc != 0) {
    log << MSG::INFO << " Algorithm = " << name() << " is connected to Histogram Service = "
        << tmp_histSvc->name() << endreq;
  }
#endif

  m_hist = histoSvc()->book("/stat/simple1D/Dtest1","TestMTHist1D",100,0,100);
  if (0 == m_hist) {
    log << MSG::ERROR << " ERROR booking histogram" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::DEBUG << "booking for the family : " << getGaudiThreadIDfromName(name()) << endreq;
  // Locate the StoreGateSvc
  sc = service("StoreGateSvc", m_StoreGateSvc);
  if (!sc.isSuccess()) {
    log << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
    return sc;
  } else {
    log << MSG::INFO << " Algorithm = " << name() << " is connected to StoreGate Service = "
        << m_StoreGateSvc->name() << endreq;
  }
  return sc;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MTHist::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  // Get the Event
  const EventInfo* pevt;
  if (StatusCode::SUCCESS != m_StoreGateSvc->retrieve(pevt,""))
    return StatusCode::FAILURE;

  int event = pevt->event_ID()->event_number();
  //int run = pevt->event_ID()->run_number();
  
  // fill the histogram
  m_hist->fill( float(event-m_previousEvent) , 1.);
  m_previousEvent = event;



  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MTHist::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;

  return StatusCode::SUCCESS;
}
