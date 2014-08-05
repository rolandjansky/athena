/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define WTC_ADC_MAX 1024
#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"

#include "TBRec/TBTailCatcherRec.h"

//#include "GaudiKernel/AlgFactory.h"
// static const AlgFactory<TBObjectRecoAlgo> s_factory;
// const IAlgFactory& TBObjectRecoAlgoFactory = s_factory;





TBTailCatcherRec::TBTailCatcherRec(const std::string& name,
				 ISvcLocator* pSvcLocator) :
  Algorithm(name,pSvcLocator),
  m_StoreGate(0)
 {
  // job options

   declareProperty("SGkey",  m_SGkey="TailCatcherRaw");
   declareProperty("TailCatcherCalib",  m_tailcatch_calib);
   declareProperty("TailCatcherPed",  m_tailcatch_ped);

 }

TBTailCatcherRec::~TBTailCatcherRec()
{ }

StatusCode
TBTailCatcherRec::initialize()
{
  ///////////////////////
  // Allocate Services //
  ///////////////////////

  // message service
  MsgStream log(messageService(),name());
  StatusCode sc;
  
  sc = service( "StoreGateSvc", m_StoreGate);
  
  if( sc.isFailure() ) {
    log << MSG::FATAL << name() 
  	<< ": Unable to locate Service StoreGateSvc" << endreq;
    return sc;
  }
  
  

  return sc;
}

StatusCode
TBTailCatcherRec::execute()
{

  ////////////////////////////
  // Re-Allocating Services //
  ////////////////////////////
  MsgStream log(messageService(),name());
  StatusCode sc;

    log << MSG::DEBUG << "In execute()" << endreq;

  TBTailCatcherRaw * tailcatchraw;
  sc = m_StoreGate->retrieve(tailcatchraw, m_SGkey);
  if (sc.isFailure()){
    log << MSG::DEBUG << "TBObjectReco: Retrieval of "<<m_SGkey<<" failed" << endreq;
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }else {
    
    if(m_tailcatch_calib.size()!=tailcatchraw->size()){
      log << MSG::ERROR << "Calib factor number : "<<m_tailcatch_calib.size();
      log << " != "<<tailcatchraw->size()<<" = number of scints in TailCatcherRaw"<<endreq;
      setFilterPassed(false);
      return StatusCode::SUCCESS;
    }
    
    std::vector< TBScintillator* > theScints;
    
    TBTailCatcherRaw::const_iterator it_scint   = tailcatchraw->begin();
    TBTailCatcherRaw::const_iterator last_scint   = tailcatchraw->end();
    int ind=0;
    for(;it_scint != last_scint;it_scint++){
      const TBScintillatorRaw * scintraw = (*it_scint);
      std::string name = scintraw->getDetectorName();
      
      TBScintillator * scint = new TBScintillator();
      scint->setDetectorName(name);
      // No time signal in TalCatcher, only ADC
      unsigned int adc = scintraw->getADCSignal();
      if (scintraw->isADCOverflow()) adc = WTC_ADC_MAX;
      scint->setSignal(m_tailcatch_calib[ind]*(adc - m_tailcatch_ped[ind]));
      
      scint->setSignalOverflow(scintraw->isADCOverflow());
      
      theScints.push_back(scint);
      ind++;
    }
    TBTailCatcher * tailcatch = new TBTailCatcher(tailcatchraw->getDetectorName(),tailcatchraw->isOverflow(),theScints);
    sc = m_StoreGate->record(tailcatch,"TailCatcher");
    if ( sc.isFailure( ) ) {
      log << MSG::FATAL << "Cannot record TailCatcher " << endreq;
    }
  }
  if ( sc.isFailure( ) ) {
     setFilterPassed(false);
  } else {
     setFilterPassed(true);
  }
  return StatusCode::SUCCESS;
}


StatusCode 
TBTailCatcherRec::finalize()
{

  ////////////////////////////
  // Re-Allocating Services //
  ////////////////////////////


  return StatusCode::SUCCESS;
}
