/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"

#include "TBRec/TBScintillatorRec.h"

#include "TBEvent/TBScintillatorCont.h"
#include "TBEvent/TBScintillatorRawCont.h"

//#include "GaudiKernel/AlgFactory.h"
// static const AlgFactory<TBScintillatorRec> s_factory;
// const IAlgFactory& TBScintillatorRecFactory = s_factory;



TBScintillatorRec::TBScintillatorRec(const std::string& name,
				 ISvcLocator* pSvcLocator) :
  Algorithm(name,pSvcLocator),
  m_StoreGate(0)
 {
  // job options

   declareProperty("SGkey",  m_SGkey="ScintillatorRawCont");
   declareProperty("SGrecordkey",  m_SGrecordkey="ScintillatorCont");

   declareProperty("SCINTnames",  m_scint_names);
   declareProperty("SCINTtimecalib",  m_scint_timecalib);
   declareProperty("SCINTtimeped",  m_scint_ped);


 }

TBScintillatorRec::~TBScintillatorRec()
{ }

StatusCode
TBScintillatorRec::initialize()
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
  
  log << MSG::DEBUG << "Size of knowns scintillator list = " <<m_scint_names.size() << endreq;
  return sc;
}

StatusCode
TBScintillatorRec::execute()
{

  ////////////////////////////
  // Re-Allocating Services //
  ////////////////////////////
  MsgStream log(messageService(),name());
  StatusCode sc;

    log << MSG::DEBUG << "In execute()" << endreq;

  TBScintillatorRawCont * scintrawCont;
  sc = m_StoreGate->retrieve(scintrawCont, m_SGkey);
  if (sc.isFailure()){
    log << MSG::DEBUG << "TBObjectReco: Retrieval of "<<m_SGkey<<" failed" << endreq;
    
  }else {

    TBScintillatorCont * scintCont = new TBScintillatorCont();
    TBScintillatorRawCont::const_iterator it_scint   = scintrawCont->begin();
    TBScintillatorRawCont::const_iterator last_scint   = scintrawCont->end();
    
    // Loop over Sintillators
    for(;it_scint != last_scint;it_scint++){
      const TBScintillatorRaw * scintraw = (*it_scint);
      std::string name = scintraw->getDetectorName();
      unsigned int ind=0;
      // Find calibration index for this Scint
  
      while(ind<m_scint_names.size()) 
	{
	  if(name==m_scint_names[ind]) break; 
	  else ind++;
	}
      if(ind==m_scint_names.size()) {
	log<<MSG::DEBUG<< "No calibrations for Scintillator " <<name<<endreq;
	continue;
      }
      
      TBScintillator * scint = new TBScintillator();
      scint->setDetectorName(name);
      scint->setSignal(1.*scintraw->getADCSignal());
      scint->setTimeSignal(m_scint_timecalib[ind]*(scintraw->getTDCSignal()-m_scint_ped[ind]));
      
      log << MSG::DEBUG << name << "  ADC=" << scintraw->getADCSignal() << "  TDC="<<scintraw->getTDCSignal()<< "  ADCOf=" << scintraw->isADCOverflow()<< "  TDCOf=" << scintraw->isTDCOverflow()<<endreq;
      
      scint->setSignalOverflow(scintraw->isADCOverflow());
      scint->setTimeOverflow(scintraw->isTDCOverflow());


      scintCont->push_back(scint);
    }
    
    
    sc = m_StoreGate->record(scintCont,m_SGrecordkey);
    if ( sc.isFailure( ) ) {
      log << MSG::FATAL << "Cannot record ScintCont" << endreq;
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
TBScintillatorRec::finalize()
{

  ////////////////////////////
  // Re-Allocating Services //
  ////////////////////////////


  return StatusCode::SUCCESS;
}
