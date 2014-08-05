/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "StoreGate/StoreGateSvc.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TBRec/TBPhaseRec.h"

#include "TBEvent/TBTDCRaw.h"
#include "TBEvent/TBTDCRawCont.h"
#include "TBEvent/TBPhase.h"
#include "TBEvent/TBEventInfo.h"

#include "PathResolver/PathResolver.h"

#include <algorithm>
#include <cmath>

#include <iostream>
#include <fstream>

using CLHEP::ns;

TBPhaseRec::TBPhaseRec(const std::string& name,
				 ISvcLocator* pSvcLocator) :
  Algorithm(name,pSvcLocator)
  , m_StoreGate(0)
  , m_delta(25.0*ns)
  , m_timeBins(25)
  , m_TBPhaseKey("TBPhase")
  , m_neverReturnFailure(false)
  , m_guardValue(-1.) // in TDC counts
  , m_nTDC(0)
 {
  // job options
   declareProperty("CalibFileName",m_calib_filename="");
   m_tdcNames.resize(0);
   m_tdcToTime.resize(0);
   m_tdcwac.resize(0);
   m_tdcMin.resize(0);
   declareProperty("TDCNames",           m_tdcNames);
   declareProperty("TDCToTime",          m_tdcToTime);
   declareProperty("TDCwac",             m_tdcwac);
   declareProperty("TDCMin",             m_tdcMin);
   declareProperty("TTCClockPeriod",     m_delta);
   declareProperty("TimeBins",           m_timeBins);
   declareProperty("TBPhaseKey",         m_TBPhaseKey);
   declareProperty("NeverReturnFailure", m_neverReturnFailure);  
   declareProperty("GuardCutValue",      m_guardValue);
   m_runnumber = 0;
}

TBPhaseRec::~TBPhaseRec()
{ }

StatusCode
TBPhaseRec::initialize()
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

  // check consistency of jobOptions, set defaults if required
  float default_tdc2time = 0.050*ns;
  float default_tdcwac   = 0.;
  float default_tdcMin   = 0.;

  m_nTDC = m_tdcNames.size() ;
  if ( m_nTDC == 0 ) {
    log << MSG::ERROR << "Empty list of TDC names" << endreq ;
    return StatusCode::FAILURE ;
  }

  if (m_nTDC != (int)m_tdcToTime.size()) {  // check consistency of joboption vectors
    if (m_tdcToTime.size() == 0) {
      m_tdcToTime.resize(m_nTDC) ;
      for (int k = 0; k < m_nTDC; k++) m_tdcToTime[k] = default_tdc2time;
    } else if (m_tdcToTime.size() == 1) {
      m_tdcToTime.resize(m_nTDC);
      float tdc2time = m_tdcToTime[0];
      for (int k = 0; k < m_nTDC; k++) m_tdcToTime[k] = tdc2time;
    } else {
      log << MSG::FATAL << "Nunber of TDCs not equal to nunber of tdc2time constants" << endreq ;
      return StatusCode::FAILURE ;
    }
  }

  if (m_nTDC != (int)m_tdcwac.size()) {  // check consistency of joboption vectors
    if (m_tdcwac.size() == 0) {
      m_tdcwac.resize(m_nTDC) ;
      for (int k = 0; k < m_nTDC; k++) m_tdcwac[k] = default_tdcwac;
    } else if (m_tdcwac.size() == 1) {
      m_tdcwac.resize(m_nTDC);
      float tdcwac = m_tdcwac[0];
      for (int k = 0; k < m_nTDC; k++) m_tdcwac[k] = tdcwac;
    } else {
      log << MSG::FATAL << "Nunber of TDCs not equal to nuunber of wac constants" << endreq ;
      return StatusCode::FAILURE ;
    }
  }

  if (m_nTDC != (int)m_tdcMin.size()) {  // check consistency of joboption vectors
    if (m_tdcMin.size() == 0) {
      m_tdcMin.resize(m_nTDC) ;
      for (int k = 0; k < m_nTDC; k++) m_tdcMin[k] = default_tdcMin;
    } else if (m_tdcMin.size() == 1) {
      m_tdcMin.resize(m_nTDC);
      float tdcMin = m_tdcMin[0];
      for (int k = 0; k < m_nTDC; k++) m_tdcMin[k] = tdcMin;
    } else {
      log << MSG::FATAL << "Nunber of TDCs not equal to nuunber of tdcMin constants" << endreq ;
      return StatusCode::FAILURE ;
    }
  }

  log << MSG::INFO
      << "TTCClockPeriod = " << m_delta/ns << " ns"
      << endreq;
  for (int k = 0; k < m_nTDC; k++) {
    log << MSG::INFO
        << "\042" + m_tdcNames[k] + "\042"
        << "  PhaseTDCToTime = " << m_tdcToTime[k]/ns << " ns/TDC"
        << "  PhaseTDCwac    = " << m_tdcwac[k]
        << "  PhaseTDCMin    = " << m_tdcMin[k]
        << endreq;
  }

  m_phaseReco.resize(m_nTDC);
  m_tdcRaw.resize(m_nTDC);
  
  if ( m_guardValue > 0. )
    log << MSG::INFO <<  " Cut events using guard value: " << m_guardValue << endreq ;
  
  return StatusCode::SUCCESS ;
}

StatusCode TBPhaseRec::execute()
{

  ////////////////////////////
  // Re-Allocating Services //
  ////////////////////////////
  MsgStream log(messageService(),name());
  StatusCode sc;

  log << MSG::VERBOSE << "In execute()" << endreq;

  // Get run number...
  unsigned int thisrun=0;
  EventID *thisEvent;           
  const EventInfo* thisEventInfo;
  sc=m_StoreGate->retrieve(thisEventInfo);
  if (sc!=StatusCode::SUCCESS){
    log << MSG::WARNING << "No EventInfo object found! Can't read run number!" << endreq;
    log << MSG::WARNING << "     => can't get calib constant. Exit" << endreq;
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  } else {
    thisEvent=thisEventInfo->event_ID();
    thisrun = thisEvent->run_number();
  }
  
  // ... and get new calib constants (only if calibration constant file has been specified!)
  if( thisrun != m_runnumber && m_calib_filename != "" ) {
     m_runnumber= thisrun;
     if ( getnewcalib() == StatusCode::FAILURE ) {
        setFilterPassed(false);
        return StatusCode::SUCCESS;
     }
  }
  
  TBTDCRawCont * tdcRawCont;
  sc = m_StoreGate->retrieve(tdcRawCont, "TDCRawCont");
  if (sc.isFailure()) {
    log << MSG::ERROR << "TBObjectReco: Retrieval of TDCRawCont failed" << endreq;
    if (!m_neverReturnFailure) {
       setFilterPassed(false);
    }
    return StatusCode::SUCCESS;
  }

  std::vector<float> tdcQuality;
  std::vector<float> tdcdTtoWAC;
  tdcQuality.resize(m_nTDC);
  tdcdTtoWAC.resize(m_nTDC);
  for (int k = 0; k < m_nTDC; k++) {
    m_phaseReco[k] = unknown;
    m_tdcRaw[k]    = 0;
    tdcQuality[k]  = 0.;
    tdcdTtoWAC[k]  = 0.;
  }
  
  // Loop over TDC's
  int tdcFound = 0;
  int tdcFoundAndOK = 0;
  for (TBTDCRawCont::const_iterator it_tdc = tdcRawCont->begin(); it_tdc != tdcRawCont->end(); it_tdc++) {
    const TBTDCRaw* tdcRaw = (*it_tdc);
    std::string tdcName = tdcRaw->getDetectorName();
    int tdcIndex = 0;
    for (; tdcIndex < m_nTDC; tdcIndex++) {
      if (tdcName == m_tdcNames[tdcIndex]) break;
    }
    if (tdcIndex < m_nTDC) {  // found the sought tdc
      tdcFound++;

      // get the raw tdc value
      m_tdcRaw[tdcIndex] = tdcRaw->getTDC();
      
      if (m_tdcRaw[tdcIndex] <= 0 || tdcRaw->isOverflow() || tdcRaw->isUnderThreshold()) {
        // bad tdc
        if (m_tdcRaw[tdcIndex] <= 0) 
          log << MSG::DEBUG << "Bad TDC" << tdcIndex << "; value = " << m_tdcRaw[tdcIndex] << endreq;
        if (tdcRaw->isOverflow()) 
          log << MSG::DEBUG << "Bad TDC" << tdcIndex << " is overflow" << endreq;
        if (tdcRaw->isUnderThreshold()) 
          log << MSG::DEBUG << "Bad TDC" << tdcIndex << " is underthreshold" << endreq;
      } else {  
        // found andvalid tdc 
        tdcFoundAndOK++;

        // compute a quality factor used to decide which tdc to  use for the phase
        // lowest distance (tdc units) to tdcMin or tdcMax or tdcwac
        
        float dTotdcMin = fabs(m_tdcRaw[tdcIndex] - m_tdcMin[tdcIndex]);
        float tdcMax = m_tdcMin[tdcIndex] + m_delta/fabs(m_tdcToTime[tdcIndex]);
        float dTotdcMax = fabs(m_tdcRaw[tdcIndex] - tdcMax);
        float dTotdcwac = fabs(m_tdcRaw[tdcIndex] - m_tdcwac[tdcIndex]);
        float dtemp = (dTotdcMin < dTotdcMax) ? dTotdcMin : dTotdcMax;
        tdcQuality[tdcIndex] = (dtemp < dTotdcwac) ? dtemp : dTotdcwac;
        tdcdTtoWAC[tdcIndex] = m_tdcRaw[tdcIndex] - m_tdcwac[tdcIndex];
        
        log << MSG::DEBUG
            << "TDC" << tdcIndex << " value = " << m_tdcRaw[tdcIndex]
            << "; tdc quality"
            << ": to tdcMin = " << dTotdcMin
            << "; to tdcMax = " << dTotdcMax
            << "; to tdcwac = " << dTotdcwac
            << "; final = " << tdcQuality[tdcIndex]
            << endreq;
        
        // compute the phase
        m_phaseReco[tdcIndex] = computePhase(tdcIndex);

        log << MSG::DEBUG
            << "TDC" << tdcIndex << " value = " << m_tdcRaw[tdcIndex]
            << "; reconstructed phase = " << m_phaseReco[tdcIndex]/ns << " ns"
            << endreq;
      }

      if (tdcFound == m_nTDC) break ; // exit container loop if all TDCs found
    }
  }
  
  if (tdcFound == 0 || tdcFoundAndOK == 0) {
    // Check if this is a random trigger and set tdc=12.5 ns
    // retrieve Event Info
    const TBEventInfo* theEventInfo;
    StatusCode checkOut = m_StoreGate->retrieve(theEventInfo,"TBEventInfo");
    if ( checkOut.isFailure() )
      {
	log << MSG::ERROR
	    << "cannot retrieve TBEventInfo from StoreGate"
	    << endreq;
        setFilterPassed(false);
	return StatusCode::SUCCESS;
      }
    else
      {
	log << MSG::DEBUG
	    << "TBEventInfo object found in TDS"
	    << endreq;
      }
    int evtType = theEventInfo->getEventType();
    log << MSG::DEBUG << "Event Type found " << evtType << endreq;
    if (evtType != 1) {    // Not a physics trigger found
      float phase = 12.5;
      short phaseInd = (short)floor(phase/m_delta * (float)m_timeBins);
      float tdc_to_wac = 100.;
      TBPhase* theTBPhase = new TBPhase(phase, phaseInd, tdc_to_wac);
      sc = m_StoreGate->record(theTBPhase, m_TBPhaseKey);
      if (sc.isFailure( )) {
	log << MSG::FATAL << "Cannot record TBPhase" << endreq;
        setFilterPassed(false); // always return failure for this one!
        return StatusCode::SUCCESS;
      }
      return StatusCode::SUCCESS;
    }
    if (tdcFound == 0) {
      log << MSG::ERROR << "no TDCs found in StoreGate" << endreq;
    } else {
      log << MSG::ERROR << "no valid TDC data found" << endreq;
    }
    if (!m_neverReturnFailure) {
       setFilterPassed(false);
    }
    return StatusCode::SUCCESS;
  }
  if (tdcFound < m_nTDC || tdcFoundAndOK < m_nTDC) {
    if (tdcFound < m_nTDC) {
      log << MSG::WARNING << "not all the requested TDCs were found in StoreGate" << endreq;
    } else {
      log << MSG::WARNING << "not all the requested TDCs contained valid data" << endreq;
    }
  }
  
  // choose/compute best phase, compute corresponding best phaseind, create TBPhase instance
  int tdcBestIndex = 0;
  float QMax = 0.;
  for (int k = 0; k < m_nTDC; k++) {
    if (tdcQuality[k] > QMax) {
      QMax = tdcQuality[k];
      tdcBestIndex = k;
    }
  }
  float bestPhase = m_phaseReco[tdcBestIndex];

  log << MSG::DEBUG
      << "best quality for TDC" << tdcBestIndex
      << ", with reconstructed phase = " << bestPhase/ns << " ns"
      << endreq;

  // If the tdc is working properly and if the TBPhaseRec jobOptions correctly calibrate the tdc,
  // then this timeSampleShift should be zero always.
  int timeSampleShift = (int)floor(bestPhase/m_delta);
  if (timeSampleShift != 0) {
    log << MSG::WARNING
        << "TBPhaseRec time sample shift non zero: " << timeSampleShift
        << endreq;
  }
  
  // the phase is between 0 and the ttcClockPeriod
  float phase = bestPhase - timeSampleShift*m_delta;  // "wrapped around" phase in case of shift

  // the phase index must be between 0 and m_timeBins-1
  short phaseInd = (short)floor(phase/m_delta * (float)m_timeBins);
  if (phaseInd < 0 || phaseInd > m_timeBins - 1) {
    log << MSG::ERROR
        << "Phase " << phase/ns << " ns "
        << "has phase index " << phaseInd 
        << " outside of the bounds [0," << m_timeBins-1 << "]"
        << endreq;
    if (!m_neverReturnFailure) {
       setFilterPassed(false);
    }
    return StatusCode::SUCCESS;
  }

  // guard region cut
  if ( m_guardValue > 0. && fabs(tdcdTtoWAC[tdcBestIndex]) < m_guardValue ) {
    log << MSG::ERROR
        << "Phase " << phase/ns << " ns "
        << "has TDC-WAC " << tdcdTtoWAC[tdcBestIndex]
        << " inside the guard region [0," << m_guardValue  << "]"
        << endreq;
    if (!m_neverReturnFailure) {
       setFilterPassed(false);
    }
    return StatusCode::SUCCESS;
  } 

  log << MSG::DEBUG
      << "Phase = " << phase/ns << " ns; "
      << "phase index = " << phaseInd
      << endreq;

  // create and store the TBPhase
  // TBPhase* theTBPhase = new TBPhase(phase, phaseInd);
  TBPhase* theTBPhase = new TBPhase(phase, phaseInd, tdcdTtoWAC[tdcBestIndex] );
  
  sc = m_StoreGate->record(theTBPhase, m_TBPhaseKey);
  if (sc.isFailure( )) {
    log << MSG::FATAL << "Cannot record TBPhase" << endreq;
    setFilterPassed(false);  // always return failure for this one!
    return StatusCode::SUCCESS;
  }
  
  return StatusCode::SUCCESS;
  
}

StatusCode TBPhaseRec::finalize() {
  return StatusCode::SUCCESS;
}

float TBPhaseRec::computePhase(int tdcIndex) {

   float phaseReco = m_tdcToTime[tdcIndex] * ((float)m_tdcRaw[tdcIndex] - m_tdcwac[tdcIndex]);
   if (m_tdcToTime[tdcIndex] > 0.) {
     if ((float)m_tdcRaw[tdcIndex] < m_tdcwac[tdcIndex]) phaseReco += m_delta;
   } else {
     if ((float)m_tdcRaw[tdcIndex] > m_tdcwac[tdcIndex]) phaseReco += m_delta;
   }

   return phaseReco ;
}



StatusCode TBPhaseRec::getnewcalib() {
  //
  // Get calib constant from an ASCII file with the following structure :
  // 
  // runnumber
  // tdcnumber TDCToTime TDCwac TDCMin
  // ...

  MsgStream log(messageService(),name());
  log << MSG::INFO << "Get TDC calibration constants for run " << m_runnumber<< endreq;
  
  int tdcnumber= m_tdcNames.size();

  m_tdcToTime.clear(); m_tdcToTime.resize(tdcnumber);
  m_tdcwac.clear();    m_tdcwac.resize(tdcnumber);
  m_tdcMin.clear();    m_tdcMin.resize(tdcnumber);

  // find file in share directory
  m_calib_filename = PathResolver::find_file (m_calib_filename, "DATAPATH");

  std::ifstream calibfile;
  calibfile.open(m_calib_filename.c_str());
  if (!calibfile.good()) {
    log << MSG::INFO << "Problem with calibration file "<< m_calib_filename << endreq;
    return StatusCode::FAILURE;
  }

  int pos = -1 ;  
  int runnumber = 0, prevrunnumber = 0;

  /* 
  
    Parsing of TDC calibration constants file
    
    Loop is exited when a run number bigger then the current is found: calibration constants of
    previous available run will be used, last step is used  only to define interval of validity
    (TDC calibration constant obtained from a run are considered valid _from_ that run (included)
    since the next one used for TDC calibration)

    If the calibration constants file has only one line, or provide calibration constants for run
    numbers smaller than the current one, or a empty line is found, the last read TDC calibration
    constants are used but a warning message is issued.

  */

  while ( !calibfile.eof() ) {
     prevrunnumber = runnumber ;
     runnumber = -1 ;     
     calibfile >> runnumber;
     if ( runnumber==-1 ) { // reached an empty line
       log << MSG::WARNING << "Empty line found in " << m_calib_filename << endreq; 
       calibfile.clear();
       runnumber = prevrunnumber ;
       break ;
     }
     if ( runnumber > int(m_runnumber) ) break ;
     pos = calibfile.tellg();
     for(int j=0; j<tdcnumber+1; j++) calibfile.ignore(5000,'\n'); // discard next lines
  } 
    
  // this take care of the case in which only one line is present in the file,
  // and the corresponding run number is greater than the current one
  if ( pos == -1 ) { 
     pos = calibfile.tellg(); 
     prevrunnumber = runnumber ;
  }
  // Now we should have found a good set of constant (the ones following pos)
  log << MSG::DEBUG << "Position in file stream = "<< pos << endreq;  
  calibfile.seekg(pos);
  
  log << MSG::INFO << "TDC calibration constants obtained from run " << prevrunnumber << endreq;
  if ( prevrunnumber != runnumber ) {
     log << MSG::INFO << "valid for run interval " << prevrunnumber << " - "  << runnumber << endreq; 
  } else {
     log << MSG::WARNING << "TDC calibration constants could not be optimal... "<< endreq; 
  }
  
  for(int j=0;j<tdcnumber;j++) {
      int tdcn;
      calibfile >> tdcn; 
      calibfile >> m_tdcToTime[j];
      calibfile >> m_tdcwac[j];  
      calibfile >> m_tdcMin[j];  
      log << MSG::INFO << " * TDC n. " << tdcn << endreq;   
      log << MSG::INFO << "   - TDCToTime = " << m_tdcToTime[j] << endreq;    
      log << MSG::INFO << "   - TDCwac    = " << m_tdcwac[j] << endreq;	 
      log << MSG::INFO << "   - TDCMin    = " << m_tdcMin[j] << endreq;      
  }
  
  calibfile.close();
  
  return StatusCode::SUCCESS;
  
}
