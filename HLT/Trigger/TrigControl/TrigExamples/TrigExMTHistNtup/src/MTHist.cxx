/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigExMTHistNtup/MTHist.h"

#include "GaudiKernel/ISvcLocator.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogram1D.h"

#include "StoreGate/DataHandle.h"
#include "GaudiKernel/ThreadGaudi.h"


/////////////////////////////////////////////////////////////////////////////

MTHist::MTHist(const std::string& name, ISvcLocator* pSvcLocator) :
   AthAlgorithm(name, pSvcLocator), 
   m_hist(0),
   m_previousEvent(0)
{}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MTHist::initialize() {

   StatusCode sc = StatusCode::SUCCESS;
   ATH_MSG_INFO("in initialize()");

#ifdef ATLAS_GAUDI_V21
   SmartIF<IService> tmp_msgSvc(msgSvc());
   if(tmp_msgSvc.isValid()) {
      ATH_MSG_INFO(" Algorithm = " << name() << " is connected to Message Service = "
          << tmp_msgSvc->name());
   }

   // Register the histogram
   SmartIF<IService> tmp_histSvc(histoSvc());
   if(tmp_histSvc.isValid()) {
      ATH_MSG_INFO(" Algorithm = " << name() << " is connected to Histogram Service = "
          << tmp_histSvc->name());
   }
#else
   Service* tmp_msgSvc = dynamic_cast<Service*> (msgSvc());
   if(tmp_msgSvc != 0) {
      ATH_MSG_INFO(" Algorithm = " << name() << " is connected to Message Service = "
          << tmp_msgSvc->name());
   }

   // Register the histogram
   Service* tmp_histSvc = dynamic_cast<Service*> (histoSvc());
   if(tmp_histSvc != 0) {
      ATH_MSG_INFO(" Algorithm = " << name() << " is connected to Histogram Service = "
          << tmp_histSvc->name());
   }
#endif

   m_hist = histoSvc()->book("/stat/simple1D/Dtest1","TestMTHist1D",100,0,100);
   if (0 == m_hist) {
      ATH_MSG_ERROR(" ERROR booking histogram");
      return StatusCode::FAILURE;
   }
   ATH_MSG_DEBUG("booking for the family : " << getGaudiThreadIDfromName(name()));

   return sc;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MTHist::execute() {

  ATH_MSG_INFO("in execute()");

  // Get the Event
  const EventInfo* pevt;
  if (StatusCode::SUCCESS != evtStore()->retrieve(pevt,""))
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

  ATH_MSG_INFO("in finalize()");

  return StatusCode::SUCCESS;
}
