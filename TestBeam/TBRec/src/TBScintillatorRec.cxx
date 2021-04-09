/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "TBScintillatorRec.h"

#include "TBEvent/TBScintillatorCont.h"
#include "TBEvent/TBScintillatorRawCont.h"


TBScintillatorRec::TBScintillatorRec(const std::string& name,
				 ISvcLocator* pSvcLocator) :
  AthAlgorithm(name,pSvcLocator)
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
  ATH_MSG_DEBUG ( "Size of knowns scintillator list = " <<m_scint_names.size() );
  return StatusCode::SUCCESS;
}

StatusCode
TBScintillatorRec::execute()
{
  ATH_MSG_DEBUG ( "In execute()" );

  TBScintillatorRawCont * scintrawCont;
  StatusCode sc = evtStore()->retrieve(scintrawCont, m_SGkey);
  if (sc.isFailure()){
    ATH_MSG_DEBUG ( "TBObjectReco: Retrieval of "<<m_SGkey<<" failed" );
    
  }else {

    TBScintillatorCont * scintCont = new TBScintillatorCont();
    
    // Loop over Scintillators
    for (const TBScintillatorRaw * scintraw : *scintrawCont) {
      std::string name = scintraw->getDetectorName();
      unsigned int ind=0;
      // Find calibration index for this Scint
  
      while(ind<m_scint_names.size()) 
	{
	  if(name==m_scint_names[ind]) break; 
	  else ind++;
	}
      if(ind==m_scint_names.size()) {
	ATH_MSG_DEBUG( "No calibrations for Scintillator " <<name);
	continue;
      }
      
      TBScintillator * scint = new TBScintillator();
      scint->setDetectorName(name);
      scint->setSignal(1.*scintraw->getADCSignal());
      scint->setTimeSignal(m_scint_timecalib[ind]*(scintraw->getTDCSignal()-m_scint_ped[ind]));
      
      ATH_MSG_DEBUG ( name << "  ADC=" << scintraw->getADCSignal() << "  TDC="<<scintraw->getTDCSignal()<< "  ADCOf=" << scintraw->isADCOverflow()<< "  TDCOf=" << scintraw->isTDCOverflow());
      
      scint->setSignalOverflow(scintraw->isADCOverflow());
      scint->setTimeOverflow(scintraw->isTDCOverflow());


      scintCont->push_back(scint);
    }
    
    
    sc = evtStore()->record(scintCont,m_SGrecordkey);
    if ( sc.isFailure( ) ) {
      ATH_MSG_FATAL ( "Cannot record ScintCont" );
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
  return StatusCode::SUCCESS;
}
