/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "TBPhaseRec.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TBEvent/TBTDCRaw.h"
#include "TBEvent/TBTDCRawCont.h"
#include "TBEvent/TBPhase.h"
#include "TBEvent/TBEventInfo.h"

#include "PathResolver/PathResolver.h"
#include "AthenaKernel/Units.h"

#include <algorithm>
#include <cmath>

#include <iostream>
#include <fstream>

using Athena::Units::ns;

TBPhaseRec::TBPhaseRec(const std::string& name,
				 ISvcLocator* pSvcLocator) :
  AthAlgorithm(name,pSvcLocator)
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
  // check consistency of jobOptions, set defaults if required
  float default_tdc2time = 0.050*ns;
  float default_tdcwac   = 0.;
  float default_tdcMin   = 0.;

  m_nTDC = m_tdcNames.size() ;
  if ( m_nTDC == 0 ) {
    ATH_MSG_ERROR ( "Empty list of TDC names" );
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
      ATH_MSG_FATAL ( "Nunber of TDCs not equal to nunber of tdc2time constants" );
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
      ATH_MSG_FATAL ( "Nunber of TDCs not equal to nuunber of wac constants" );
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
      ATH_MSG_FATAL ( "Nunber of TDCs not equal to nuunber of tdcMin constants" );
      return StatusCode::FAILURE ;
    }
  }

  ATH_MSG_INFO
    ( "TTCClockPeriod = " << m_delta/ns << " ns" );
  for (int k = 0; k < m_nTDC; k++) {
    ATH_MSG_INFO
      ( "\042" + m_tdcNames[k] + "\042"
        << "  PhaseTDCToTime = " << m_tdcToTime[k]/ns << " ns/TDC"
        << "  PhaseTDCwac    = " << m_tdcwac[k]
        << "  PhaseTDCMin    = " << m_tdcMin[k] );
  }

  m_phaseReco.resize(m_nTDC);
  m_tdcRaw.resize(m_nTDC);
  
  if ( m_guardValue > 0. )
    ATH_MSG_INFO (  " Cut events using guard value: " << m_guardValue );
  
  return StatusCode::SUCCESS ;
}

StatusCode TBPhaseRec::execute()
{
  ATH_MSG_VERBOSE ( "In execute()" );

  // Get run number...
  unsigned int thisrun=0;
  const EventInfo* thisEventInfo = nullptr;
  StatusCode sc=evtStore()->retrieve(thisEventInfo);
  if (sc!=StatusCode::SUCCESS){
    ATH_MSG_WARNING ( "No EventInfo object found! Can't read run number!" );
    ATH_MSG_WARNING ( "     => can't get calib constant. Exit" );
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  } else {
    thisrun = thisEventInfo->event_ID()->run_number();
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
  sc = evtStore()->retrieve(tdcRawCont, "TDCRawCont");
  if (sc.isFailure()) {
    ATH_MSG_ERROR ( "TBObjectReco: Retrieval of TDCRawCont failed" );
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
  for (const TBTDCRaw* tdcRaw : *tdcRawCont) {
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
          ATH_MSG_DEBUG ( "Bad TDC" << tdcIndex << "; value = " << m_tdcRaw[tdcIndex] );
        if (tdcRaw->isOverflow()) 
          ATH_MSG_DEBUG ( "Bad TDC" << tdcIndex << " is overflow" );
        if (tdcRaw->isUnderThreshold()) 
          ATH_MSG_DEBUG ( "Bad TDC" << tdcIndex << " is underthreshold" );
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
        
        ATH_MSG_DEBUG
          ( "TDC" << tdcIndex << " value = " << m_tdcRaw[tdcIndex]
            << "; tdc quality"
            << ": to tdcMin = " << dTotdcMin
            << "; to tdcMax = " << dTotdcMax
            << "; to tdcwac = " << dTotdcwac
            << "; final = " << tdcQuality[tdcIndex] );
        
        // compute the phase
        m_phaseReco[tdcIndex] = computePhase(tdcIndex);

        ATH_MSG_DEBUG
          ( "TDC" << tdcIndex << " value = " << m_tdcRaw[tdcIndex]
            << "; reconstructed phase = " << m_phaseReco[tdcIndex]/ns << " ns"
            );
      }

      if (tdcFound == m_nTDC) break ; // exit container loop if all TDCs found
    }
  }
  
  if (tdcFound == 0 || tdcFoundAndOK == 0) {
    // Check if this is a random trigger and set tdc=12.5 ns
    // retrieve Event Info
    const TBEventInfo* theEventInfo;
    StatusCode checkOut = evtStore()->retrieve(theEventInfo,"TBEventInfo");
    if ( checkOut.isFailure() )
      {
	ATH_MSG_ERROR ( "cannot retrieve TBEventInfo from StoreGate" );
        setFilterPassed(false);
	return StatusCode::SUCCESS;
      }
    else
      {
	ATH_MSG_DEBUG ( "TBEventInfo object found in TDS" );
      }
    int evtType = theEventInfo->getEventType();
    ATH_MSG_DEBUG ( "Event Type found " << evtType );
    if (evtType != 1) {    // Not a physics trigger found
      float phase = 12.5;
      short phaseInd = (short)floor(phase/m_delta * (float)m_timeBins);
      float tdc_to_wac = 100.;
      TBPhase* theTBPhase = new TBPhase(phase, phaseInd, tdc_to_wac);
      sc = evtStore()->record(theTBPhase, m_TBPhaseKey);
      if (sc.isFailure( )) {
	ATH_MSG_FATAL ( "Cannot record TBPhase" );
        setFilterPassed(false); // always return failure for this one!
        return StatusCode::SUCCESS;
      }
      return StatusCode::SUCCESS;
    }
    if (tdcFound == 0) {
      ATH_MSG_ERROR ( "no TDCs found in StoreGate" );
    } else {
      ATH_MSG_ERROR ( "no valid TDC data found" );
    }
    if (!m_neverReturnFailure) {
       setFilterPassed(false);
    }
    return StatusCode::SUCCESS;
  }
  if (tdcFound < m_nTDC || tdcFoundAndOK < m_nTDC) {
    if (tdcFound < m_nTDC) {
      ATH_MSG_WARNING ( "not all the requested TDCs were found in StoreGate" );
    } else {
      ATH_MSG_WARNING ( "not all the requested TDCs contained valid data" );
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

  ATH_MSG_DEBUG
    ( "best quality for TDC" << tdcBestIndex
      << ", with reconstructed phase = " << bestPhase/ns << " ns" );

  // If the tdc is working properly and if the TBPhaseRec jobOptions correctly calibrate the tdc,
  // then this timeSampleShift should be zero always.
  int timeSampleShift = (int)floor(bestPhase/m_delta);
  if (timeSampleShift != 0) {
    ATH_MSG_WARNING
      ( "TBPhaseRec time sample shift non zero: " << timeSampleShift );
  }
  
  // the phase is between 0 and the ttcClockPeriod
  float phase = bestPhase - timeSampleShift*m_delta;  // "wrapped around" phase in case of shift

  // the phase index must be between 0 and m_timeBins-1
  short phaseInd = (short)floor(phase/m_delta * (float)m_timeBins);
  if (phaseInd < 0 || phaseInd > m_timeBins - 1) {
    ATH_MSG_ERROR
      ( "Phase " << phase/ns << " ns "
        << "has phase index " << phaseInd 
        << " outside of the bounds [0," << m_timeBins-1 << "]" );
    if (!m_neverReturnFailure) {
       setFilterPassed(false);
    }
    return StatusCode::SUCCESS;
  }

  // guard region cut
  if ( m_guardValue > 0. && fabs(tdcdTtoWAC[tdcBestIndex]) < m_guardValue ) {
    ATH_MSG_ERROR
      ( "Phase " << phase/ns << " ns "
        << "has TDC-WAC " << tdcdTtoWAC[tdcBestIndex]
        << " inside the guard region [0," << m_guardValue  << "]" );
    if (!m_neverReturnFailure) {
       setFilterPassed(false);
    }
    return StatusCode::SUCCESS;
  } 

  ATH_MSG_DEBUG
    ( "Phase = " << phase/ns << " ns; "
      << "phase index = " << phaseInd );

  // create and store the TBPhase
  // TBPhase* theTBPhase = new TBPhase(phase, phaseInd);
  TBPhase* theTBPhase = new TBPhase(phase, phaseInd, tdcdTtoWAC[tdcBestIndex] );
  
  sc = evtStore()->record(theTBPhase, m_TBPhaseKey);
  if (sc.isFailure( )) {
    ATH_MSG_FATAL ( "Cannot record TBPhase" );
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

  ATH_MSG_INFO ( "Get TDC calibration constants for run " << m_runnumber);
  
  int tdcnumber= m_tdcNames.size();

  m_tdcToTime.clear(); m_tdcToTime.resize(tdcnumber);
  m_tdcwac.clear();    m_tdcwac.resize(tdcnumber);
  m_tdcMin.clear();    m_tdcMin.resize(tdcnumber);

  // find file in share directory
  m_calib_filename = PathResolver::find_file (m_calib_filename, "DATAPATH");

  std::ifstream calibfile;
  calibfile.open(m_calib_filename.c_str());
  if (!calibfile.good()) {
    ATH_MSG_INFO ( "Problem with calibration file "<< m_calib_filename );
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
       ATH_MSG_WARNING ( "Empty line found in " << m_calib_filename );
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
  ATH_MSG_DEBUG ( "Position in file stream = "<< pos );
  calibfile.seekg(pos);
  
  ATH_MSG_INFO ( "TDC calibration constants obtained from run " << prevrunnumber );
  if ( prevrunnumber != runnumber ) {
    ATH_MSG_INFO ( "valid for run interval " << prevrunnumber << " - "  << runnumber );
  } else {
    ATH_MSG_WARNING ( "TDC calibration constants could not be optimal... ");
  }
  
  for(int j=0;j<tdcnumber;j++) {
      int tdcn;
      calibfile >> tdcn; 
      calibfile >> m_tdcToTime[j];
      calibfile >> m_tdcwac[j];  
      calibfile >> m_tdcMin[j];  
      ATH_MSG_INFO ( " * TDC n. " << tdcn );
      ATH_MSG_INFO ( "   - TDCToTime = " << m_tdcToTime[j] );
      ATH_MSG_INFO ( "   - TDCwac    = " << m_tdcwac[j] );
      ATH_MSG_INFO ( "   - TDCMin    = " << m_tdcMin[j] );
  }
  
  calibfile.close();
  
  return StatusCode::SUCCESS;
  
}
