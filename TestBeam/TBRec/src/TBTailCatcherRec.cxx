/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TBTailCatcherRec.h"

#define WTC_ADC_MAX 1024


TBTailCatcherRec::TBTailCatcherRec(const std::string& name,
				 ISvcLocator* pSvcLocator) :
  AthAlgorithm(name,pSvcLocator)
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
  return StatusCode::SUCCESS;
}

StatusCode
TBTailCatcherRec::execute()
{
  ATH_MSG_DEBUG ( "In execute()" );

  TBTailCatcherRaw * tailcatchraw;
  StatusCode sc = evtStore()->retrieve(tailcatchraw, m_SGkey);
  if (sc.isFailure()){
    ATH_MSG_DEBUG ( "TBObjectReco: Retrieval of "<<m_SGkey<<" failed" );
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }else {
    
    if(m_tailcatch_calib.size()!=tailcatchraw->size()){
      ATH_MSG_ERROR ( "Calib factor number : "<<m_tailcatch_calib.size()
                      << " != "<<tailcatchraw->size()<<" = number of scints in TailCatcherRaw");
      setFilterPassed(false);
      return StatusCode::SUCCESS;
    }
    
    std::vector< TBScintillator* > theScints;

    int ind=0;
    for (const TBScintillatorRaw * scintraw : *tailcatchraw) {
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
    sc = evtStore()->record(tailcatch,"TailCatcher");
    if ( sc.isFailure( ) ) {
      ATH_MSG_FATAL ( "Cannot record TailCatcher " );
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
  return StatusCode::SUCCESS;
}
