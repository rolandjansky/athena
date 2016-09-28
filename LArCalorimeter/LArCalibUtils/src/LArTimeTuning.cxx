/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArTimeTuning.h"

#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include "LArRawEvent/LArDigitContainer.h"
#include "TBEvent/TBPhase.h"

#include "LArElecCalib/ILArPedestal.h"
#include "LArElecCalib/ILArOFC.h"
#include "LArElecCalib/ILArOFCTool.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "LArIdentifier/LArOnlineID.h"

#include "LArRawConditions/LArGlobalTimeOffset.h"
#include "LArRawConditions/LArFEBTimeOffset.h"
#include "LArRawConditions/LArCellTimeOffset.h"

#include <ctype.h>


using CLHEP::megahertz;


LArTimeTuning::LArTimeTuning (const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_onlineHelper(0),
  m_DataLocation("FREE"),
  m_AdcCut(1300),
  m_AdcMax(4095),
  m_Nevents(0),
  m_NaboveThreshold(0),
  m_Nconv(0),  
  m_maxIterations(4),
  m_OFCTimePrecision(0.5),
  m_SamplingPeriodeUpperLimit(0),
  m_SamplingPeriodeLowerLimit(0),
  m_nIterAverage(0),
  m_larCablingSvc(0),
  m_emId(0),
  m_gain(),
  m_scope(GLOBAL)
{
  declareProperty("DataLocation", m_DataLocation );
  
  declareProperty("ADCCut", m_AdcCut );
  declareProperty("ADCMax", m_AdcMax );
  
  declareProperty("MaxIterations",      m_maxIterations);
  declareProperty("OFCTimePrecision",   m_OFCTimePrecision);
  declareProperty("SaveRemainingError", m_saveRemErr = false);
  declareProperty("Scope",              m_scopeStr="global");  
  declareProperty("InitialTimeSampleShift",m_initialTimeSampleShift = 0);
  declareProperty("NOFCTimeBins",       m_NOFCTimeBins = 24);
  m_NOFCPhases = m_NOFCTimeBins;
  declareProperty("NOFCPhases",         m_NOFCPhases);
  declareProperty("SamplingPeriod",     m_SamplingPeriode = 1/(40.08*megahertz) );
  declareProperty("OFCTimeBin",         m_OFCTimeBin	  = m_SamplingPeriode/m_NOFCTimeBins );
  declareProperty("UseOFCPhase",        m_useOFCphase	= false);
  declareProperty("PhaseInversion",     m_phaseInv	= true);
  declareProperty("BinHalfOffset",      m_binHalfOffset = true);
  declareProperty("AllowTimeSampleJump",m_allowTimeJump = false);
  declareProperty("SkipSaturatingCells",m_skipSaturCell = true);
  declareProperty("CorrectionSign",     m_corrSign = +1);
  declareProperty("GainSelection",      m_gainSel  = "NO");
  declareProperty("LayerSelection",     m_layerSel = -1  );
  //StoreGateKeys
  declareProperty("GlobalTimeOffsetOutKey",m_globalTimeOffsetOut = "GlobalTimeOffset");
  declareProperty("FebTimeOffsetOutKey",   m_febTimeOffsetOut    = "FebTimeOffset");
  declareProperty("CellTimeOffsetOutKey",  m_cellTimeOffsetOut   = "CellTimeOffset");
}

LArTimeTuning::~LArTimeTuning() 
{}

StatusCode LArTimeTuning::initialize(){
  ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );

  // Retrieve LArCablingService
  ToolHandle<LArCablingService> larCablingSvc("LArCablingService");
  ATH_CHECK( larCablingSvc.retrieve() );

  const CaloIdManager *caloIdMgr=CaloIdManager::instance() ;
  m_emId = caloIdMgr->getEM_ID();
  if (!m_emId) {
    ATH_MSG_ERROR ( "Could not access lar EM ID helper" );
    return StatusCode::FAILURE;
  }
  
  for (std::string::size_type i=0;i<m_scopeStr.length();i++)
    m_scopeStr[i]=toupper(m_scopeStr[i]);

  if (m_scopeStr=="GLOBAL")
    m_scope=GLOBAL;
  else if (m_scopeStr=="FEB")
    m_scope=FEB;
  else if (m_scopeStr=="PHASE")
    m_scope=PHASE;
  else if (m_scopeStr=="CELL")
    m_scope=CELL;
  else {
    ATH_MSG_ERROR ( "Unknown scope '" << m_scopeStr << "'" );
    return StatusCode::FAILURE;
  }

  for (std::string::size_type i=0;i<m_gainSel.length();i++)
    m_gainSel[i]=toupper(m_gainSel[i]);
  
  if ( m_gainSel=="LOW" ) 
     m_gain = CaloGain::LARLOWGAIN;
  else if ( m_gainSel=="MEDIUM")
     m_gain = CaloGain::LARMEDIUMGAIN;
  else if ( m_gainSel=="HIGH" )
     m_gain = CaloGain::LARHIGHGAIN;
  else 
    m_gainSel = "NO";
  
  //Validity range for a set of OFC's. If the time shift is larger than this number,
  //we make a ADC sample jump (e.g. from [0,5] to [1,6]. The second half of the uppermost 
  //bin should already be rounded to the 0th bin of the following ADC sample.
  
  if(  m_binHalfOffset ) {
    m_SamplingPeriodeUpperLimit=m_SamplingPeriode-m_SamplingPeriode/(2*m_NOFCTimeBins);
    m_SamplingPeriodeLowerLimit=-m_SamplingPeriode/(2*m_NOFCTimeBins);
  } else {
    m_SamplingPeriodeUpperLimit = m_SamplingPeriode;
    m_SamplingPeriodeLowerLimit = 0;
  }

  ATH_MSG_INFO ( " *** Number of OFC time bins per sampling periode = " << m_NOFCTimeBins );
  ATH_MSG_INFO ( " *** Sampling Periode = " << m_SamplingPeriode << "ns" );
  ATH_MSG_INFO ( " *** Sampling Periode Limits: ( " << m_SamplingPeriodeLowerLimit
                 << ", " << m_SamplingPeriodeUpperLimit << " ) ns" );
  
  m_nIterAverage = 0 ; 

  return StatusCode::SUCCESS;
}



StatusCode LArTimeTuning::execute() {
  m_Nevents++;

  const LArDigitContainer* larDigitContainer=NULL;
  const ILArPedestal* larPedestal;
  const ILArOFC* larOFC;
  
  TBPhase *theTBPhase;

  LArGlobalTimeOffset *larGlobalTimeOffset;
  LArGlobalTimeOffset *larGlobalTimeOffset_corr;
  
  LArFEBTimeOffset    *larFebTimeOffset;
  LArFEBTimeOffset    *larFebTimeOffset_corr;
  
  LArGlobalTimeOffset dummyGlobalTimeOffset;
  LArFEBTimeOffset    dummyFebTimeOffset;
  
  WeightedAverageMAP  FebTimeAverage;
  WeightedAverageMAP  CellTimeMap;
 
  // Time Offsets
  float  phaseTime = 0;
  double globalOFCTime = 0;
  double globalADCSum  = 0;
  
  std::map<HWIdentifier,double> cellTimeOffset;

  // DigitContainer
  ATH_CHECK( evtStore()->retrieve(larDigitContainer,m_DataLocation) );
  
  ATH_CHECK( detStore()->retrieve(larPedestal) );
  ATH_CHECK( detStore()->retrieve(larOFC) );

  StatusCode sc=detStore()->retrieve(larGlobalTimeOffset);
  if (sc.isFailure()) {
    ATH_MSG_VERBOSE ( "Failed to retrieve LArGlobalTimeOffset. Set to zero." );
    larGlobalTimeOffset = &dummyGlobalTimeOffset;
  }

  if (m_scope==GLOBAL) {
    larGlobalTimeOffset_corr = new LArGlobalTimeOffset();
    ATH_CHECK( evtStore()->record(larGlobalTimeOffset_corr,m_globalTimeOffsetOut) );
  }
  else
    larGlobalTimeOffset_corr = &dummyGlobalTimeOffset;

  sc=detStore()->retrieve(larFebTimeOffset);
  if (sc.isFailure()) {
    ATH_MSG_VERBOSE ( "Failed to retrieve FEB to FEB Timing. Set to zero." );
    larFebTimeOffset = &dummyFebTimeOffset;
  }    

  if (m_scope==FEB) {
    //Get Feb to Feb Time Offset
    larFebTimeOffset_corr = new LArFEBTimeOffset();
    ATH_CHECK( evtStore()->record(larFebTimeOffset_corr,m_febTimeOffsetOut) );
  }
  else
    larFebTimeOffset_corr = &dummyFebTimeOffset;

  if (m_scope!=PHASE) {
    //Get TB TDC Phase    
    sc = evtStore()->retrieve(theTBPhase,"TBPhase");
    if (sc.isFailure()) {
      // this should be only a 'warning', since TBPhase can miss due to Guard Cut...
      ATH_MSG_WARNING ( "cannot allocate TBPhase with key <TBPhase>. Exiting.");
      if (m_scope==GLOBAL) {
        larGlobalTimeOffset_corr->setTimeOffset(-999);
      }
      if (m_scope==FEB) {
        WeightedAverageMAP::const_iterator it=FebTimeAverage.begin();
        WeightedAverageMAP::const_iterator it_e=FebTimeAverage.end();
        for (;it!=it_e;it++) {
	  larFebTimeOffset_corr->setTimeOffset(it->first,-999);
        }
      }
      return StatusCode::SUCCESS;
    } 
    phaseTime=theTBPhase->getPhase();
    if (m_phaseInv) phaseTime = m_SamplingPeriode - phaseTime ;
    ATH_MSG_DEBUG ( " *** Phase = " << phaseTime );
  } 
  
  
  int nIter = 0;
  int nCells = 0;
  float err_timePeak;
  float err_Peak;

  do { // Iteration, outermost loop
    
    ATH_MSG_DEBUG ( "Iteration " << nIter );
    nIter++;
    
    globalOFCTime = 0;
    globalADCSum  = 0;
    FebTimeAverage.clear();
    nCells = 0;
    err_timePeak = 0;
    err_Peak     = 0;
    
    LArDigitContainer::const_iterator cell_it=larDigitContainer->begin();
    LArDigitContainer::const_iterator cell_it_e=larDigitContainer->end();
    
    //Loop over all cells
    for (;cell_it!=cell_it_e;cell_it++) {
      
      int OFCTimeBin=0;
      
      const std::vector<short>& samples=(*cell_it)->samples();
      const unsigned nSamples=samples.size(); 
      const HWIdentifier chid=(*cell_it)->channelID();
      const CaloGain::CaloGain gain=(*cell_it)->gain();
      const HWIdentifier febid=m_onlineHelper->feb_Id(chid);

      if ( m_layerSel >=0 ) {
        Identifier id = m_larCablingSvc->cnvToIdentifier(chid);
        int layer = m_emId->sampling(id);
        if ( layer != m_layerSel ) {	   
	   continue; //Ignore this cell, not corresponding to layer selection
	}   
      }

      // check if Gain correspond to selection (in case we don't want to mix the gains...)
      if ( m_gainSel != "NO" ) {
        if ( gain != m_gain ) continue ; //Ignore this cell, not corresponding to gain selection
      }
      
      // check if a ADC sample is above ADC cut
      std::vector<short>::const_iterator sIt=samples.begin();
      std::vector<short>::const_iterator sIt_e=samples.end();
      for (;sIt!=sIt_e;sIt++) {
	if (*sIt>m_AdcCut)
	  break;
      }
      if (sIt==sIt_e) continue; //Ignore this cell, below adc cut
      
      // check for saturating cell, if selected skip them
      int nSatur=-1 ;
      if ( m_skipSaturCell ) {
        for (;sIt!=sIt_e;sIt++) {
	  if (*sIt>=m_AdcMax) {
	    nSatur++ ;	  
	    break ;
	  }
        }
      }
      if ( m_skipSaturCell && nSatur>-1 ) {
        ATH_MSG_WARNING ( "Saturation on channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
                          << " (FEB 0x" << MSG::hex << febid.get_compact() << MSG::dec 
                          << "), sample " << nSatur << " (" << *sIt << "). Skipping channel." );
        continue; //Ignore this cell, saturation on at least one sample
      }
      
      //Calculate initial timeOffset:
      float timeOffsetSum = larGlobalTimeOffset->TimeOffset()   +
	                    larFebTimeOffset->TimeOffset(febid) +
			    phaseTime;
      if ( m_useOFCphase )  timeOffsetSum += larOFC->timeOffset(chid,gain) ;
      
      //Add corrections:
      if (m_scope==GLOBAL) {
	ATH_MSG_DEBUG ( " GlobCorr=" << larGlobalTimeOffset_corr->TimeOffset() );
	timeOffsetSum += larGlobalTimeOffset_corr->TimeOffset();
      }
      else if (m_scope==FEB) {
	ATH_MSG_DEBUG ( " FEBCorr=" << larFebTimeOffset_corr->TimeOffset(febid) );
	timeOffsetSum += larFebTimeOffset_corr->TimeOffset(febid);
      }
      else if (m_scope==CELL) {
	ATH_MSG_DEBUG ( " CellCorr=" << CellTimeMap[chid].timePeak );
	timeOffsetSum += CellTimeMap[chid].timePeak;
      }
      
      int timeSampleShift = m_initialTimeSampleShift;
      int timeOffsetSteps = (int)(timeOffsetSum/m_OFCTimeBin);
      
      ATH_MSG_VERBOSE ( "Time offests sum = " << timeOffsetSum << " ( = " << timeOffsetSteps << " steps)" );
      
      if ( m_allowTimeJump && timeOffsetSum >= m_NOFCPhases*m_OFCTimeBin ) {
	ATH_MSG_VERBOSE ( "Time Sample jump: +1" );
	timeSampleShift += 1;
	timeOffsetSum   -= m_NOFCTimeBins*m_OFCTimeBin ;
      } else if ( m_allowTimeJump && timeOffsetSum < 0 ) {
	ATH_MSG_VERBOSE ( "Time Sample jump: -1" );
	timeSampleShift -= 1;
	timeOffsetSum   += m_NOFCTimeBins*m_OFCTimeBin ;
      }
      
      /*
      if (m_allowTimeJump && ( timeOffsetSum > m_NOFCPhases*m_OFCTimeBin || timeOffsetSum < 0 ) ) {
	log << MSG::ERROR << "Time offset out of range for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
	                  << " (FEB 0x" << MSG::hex << febid.get_compact() << MSG::dec << "). Found " 
			  << timeOffsetSum << ",  expected ( 0 - " << m_NOFCPhases*m_OFCTimeBin << ") ns. Skipping channel." << endmsg;
	continue;
      }
      */

      if (m_allowTimeJump && ( timeOffsetSteps > m_NOFCPhases || timeOffsetSteps < 0 ) ) {
	ATH_MSG_ERROR ( "Time offset steps out of range for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
	                  << " (FEB 0x" << MSG::hex << febid.get_compact() << MSG::dec << "). Found " 
                        << timeOffsetSteps <<", expected ( 0 - " << m_NOFCPhases << ") steps. Skipping channel." );
	continue;
      }

      
      if (m_allowTimeJump && timeSampleShift<0) {
	ATH_MSG_ERROR ( "Negative time sample shift (" << timeSampleShift << ") found for channel 0x" 
	                  << MSG::hex << chid.get_compact() << MSG::dec << " (FEB 0x" 	                  
                        << MSG::hex << febid.get_compact() << MSG::dec << "). Skipping channel." );
	continue;
      }      
      
      // Pedestal for this channel & gain:
      float pedestal=larPedestal->pedestal(chid,gain);
      if (pedestal <= (1.0+LArElecCalib::ERRORCODE)) {
	ATH_MSG_DEBUG ( "No pedestal found for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
                        << " Skipping channel." );
	continue;
      }
      //Assume there is only one pedestal value, even the ILArPedestal object returns a vector<float>
      const float pedestalAverage=pedestal;
      
      // Get OFCs from Database
            OFCTimeBin = (int) (  timeOffsetSum / m_OFCTimeBin ) ;
      
      if ( !m_phaseInv ) // if not done with PhaseTime at the beginning, invert time bin for OFC bin selection
         OFCTimeBin = ( m_NOFCTimeBins - 1 ) - OFCTimeBin;
      // do not use the following: 24<PhaseTime<25 you always get OFCTimeBin = -1!
      //else 
      //   OFCTimeBin -= 1 ;
	 
      ATH_MSG_VERBOSE ( " *** OFCTimeBin = " << OFCTimeBin );
      
      if ( OFCTimeBin < 0 ) {
        ATH_MSG_ERROR ( " OFCTimeBin = " << OFCTimeBin 
                        << ". Skipping channel." );
	continue;
      } else if( OFCTimeBin >= m_NOFCPhases ) {
        ATH_MSG_ERROR ( "OFCTimeBin = " << OFCTimeBin 
                        << ". Skipping channel." );
	continue;
      }
      
      ILArOFC::OFCRef_t ofc_a=(larOFC->OFC_a(chid,gain,OFCTimeBin));
      ILArOFC::OFCRef_t ofc_b=(larOFC->OFC_b(chid,gain,OFCTimeBin));
      
      //Check if we have OFC for this channel and time bin
      if (ofc_a.size()==0) {
	ATH_MSG_DEBUG ( "No OFC's found for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
                        << " TimeShift=" << timeOffsetSum <<". Skipping channel." );
	continue;
      } 
      
      //Check if we have OFC for this channel and time bin
      if (ofc_a.size()+timeSampleShift>nSamples || ofc_b.size()+timeSampleShift>nSamples) {
	if (timeSampleShift==0)
	  ATH_MSG_DEBUG ( "Found LArDigit with " << nSamples << " samples, but OFCs for " 
                          << ofc_a.size() << " samples. Skipping Channel ");
	else //have time sample shift
	  ATH_MSG_DEBUG ( "After time sample shift of " << timeSampleShift << ", " << nSamples-timeSampleShift
                          << " samples left, but have OFCs for " << ofc_a.size() << " samples. Skipping Channel ");
	continue;
      } 
      nCells++;
      
      //Now apply Optimal Filtering to get ADC peak and timing (tau)
      double ADCPeak = 0;
      for (unsigned i=0;i<(ofc_a.size());++i) 
	ADCPeak += (samples[i+timeSampleShift]-pedestalAverage)*ofc_a.at(i);
      double tauPeak = 0;
      for (unsigned i=0;i<(ofc_b.size());++i) 
	tauPeak += (samples[i+timeSampleShift]-pedestalAverage)*ofc_b.at(i);
      
      ATH_MSG_DEBUG ( "Channel: " << MSG::hex << chid.get_compact() << MSG::dec 
                      << " - TimeOffsetSum = " << timeOffsetSum 
                      << " - TimeSampleShift = " << timeSampleShift << " timeOffsetSteps=" <<timeOffsetSteps 
                      << " Corr: " << tauPeak/ADCPeak << " Peak=" << (int)ADCPeak << " G=" << (*cell_it)->gain() );

      //remaining error (for loop conditions)
      err_Peak     += ADCPeak;
      err_timePeak += fabs(tauPeak);

      //Global time offset
      globalOFCTime+=(double)m_corrSign*tauPeak; // *SIGN*
      globalADCSum+=ADCPeak;
      if (m_scope==FEB) {
        FebTimeAverage[febid].timePeak+=(double)m_corrSign*tauPeak; // *SIGN*
	FebTimeAverage[febid].peak+=ADCPeak;
      } else if (m_scope==CELL) {
	const float oldcorrection  = CellTimeMap[chid].timePeak;
	const float correction     = tauPeak/ADCPeak;
	CellTimeMap[chid].timePeak = oldcorrection + (double)m_corrSign * correction; // *SIGN*
	CellTimeMap[chid].peak     = ADCPeak;
      }
      
    } // End loop over all cells

    if (nCells==0) {
      ATH_MSG_WARNING ( "No cell above threshold for this event." );
      break;
    }
    
    if (globalADCSum!=0)
        globalOFCTime=globalOFCTime/globalADCSum;
    
    // What to do with obtained number?
    if (m_scope==GLOBAL) {
      const float oldoffset=larGlobalTimeOffset_corr->TimeOffset();
      const float correction=globalOFCTime;
      larGlobalTimeOffset_corr->setTimeOffset(oldoffset-correction);
    }
    else if (m_scope==FEB) {
      WeightedAverageMAP::const_iterator it=FebTimeAverage.begin();
      WeightedAverageMAP::const_iterator it_e=FebTimeAverage.end();
      for (;it!=it_e;it++) {
	const float oldoffset=larFebTimeOffset_corr->TimeOffset(it->first);
	const float correction=it->second.get();
	larFebTimeOffset_corr->setTimeOffset(it->first,oldoffset-correction);
      }
    }
    else if (m_scope==PHASE) {
      phaseTime-=globalOFCTime;
    }
    
    ATH_MSG_DEBUG ( " " );
    ATH_MSG_DEBUG ( " ==> End of iteration " << nIter << ", remaining Error = " << err_timePeak/err_Peak );
    ATH_MSG_DEBUG ( " " );
  
  } while (err_timePeak/err_Peak>m_OFCTimePrecision && nIter<m_maxIterations);
  
  
  if (nCells==0) { //No cell above threshold
    if (m_scope==GLOBAL) 
      larGlobalTimeOffset_corr->setTimeOffset(-999);
    if (m_scope==FEB) {
      WeightedAverageMAP::const_iterator it=FebTimeAverage.begin();
      WeightedAverageMAP::const_iterator it_e=FebTimeAverage.end();
      for (;it!=it_e;it++) {
	larFebTimeOffset_corr->setTimeOffset(it->first,-999);
      }
    }
    return StatusCode::SUCCESS;
  }
  
  //Got some cells....
  m_NaboveThreshold++;
  
  if (m_maxIterations>1 && nIter==m_maxIterations && fabs(globalOFCTime)>m_OFCTimePrecision) //Iteration failed
  
    ATH_MSG_VERBOSE ( "Iteration failed. Remaining time offset=" << globalOFCTime );
  
  else { // Iteration converged
    
    ATH_MSG_VERBOSE ( "Iteration converged. Remaining time offset = " << globalOFCTime );
    m_Nconv++;
    m_nIterAverage += nIter;
    
    if ( m_saveRemErr ) { // Should I consider remaining correction?
       if (m_scope==GLOBAL) {
          const float oldoffset=larGlobalTimeOffset_corr->TimeOffset();
          larGlobalTimeOffset_corr->setTimeOffset(oldoffset+(double)m_corrSign*err_timePeak/err_Peak); // *SIGN*   
       } 
       else if (m_scope==FEB) {
         WeightedAverageMAP::const_iterator it=FebTimeAverage.begin();
         WeightedAverageMAP::const_iterator it_e=FebTimeAverage.end();
         for (;it!=it_e;it++) {
	   const float oldoffset=larFebTimeOffset_corr->TimeOffset(it->first);
	   larFebTimeOffset_corr->setTimeOffset(it->first,oldoffset+(double)m_corrSign*err_timePeak/err_Peak); // *SIGN*
         }
       }
    }
    
    if(m_scope==PHASE) { 
       const short phaseIndex=(int)round(phaseTime/m_NOFCTimeBins);
       TBPhase* thePhase=new TBPhase(phaseTime,phaseIndex);
       sc=evtStore()->record(thePhase,"TBPhase");
       if (sc.isFailure())
 	 ATH_MSG_ERROR ( "Could not record TBPhase with key 'TBPhase' to StoreGate" );
    }     
    else if (m_scope==CELL) { //Fill cell time offset map member variable
      WeightedAverageMAP::const_iterator it=CellTimeMap.begin();
      WeightedAverageMAP::const_iterator it_e=CellTimeMap.end();
      for (;it!=it_e;it++) {
	const HWIdentifier chid=it->first;
	m_CellTimeAverage[chid].timePeak+=(double)m_corrSign*it->second.timePeak*it->second.peak; // *SIGN*
	m_CellTimeAverage[chid].peak+=it->second.peak;
      }
    }

     
  } //end else iteration failure
  
  return StatusCode::SUCCESS;
}


StatusCode LArTimeTuning::stop()
{
 if (m_scope==CELL) {
   LArCellTimeOffset *cellTimeOffset=new LArCellTimeOffset();
   WeightedAverageMAP::const_iterator it=m_CellTimeAverage.begin();
   WeightedAverageMAP::const_iterator it_e=m_CellTimeAverage.end();
   for (;it!=it_e;it++) {
     cellTimeOffset->setTimeOffset(it->first,it->second.get());
     ATH_MSG_DEBUG ( "Ch = " << std::hex << it->first.get_compact() << std::dec
                     << " Time = " << cellTimeOffset->TimeOffset(it->first) );
   }
   StatusCode sc=evtStore()->record(cellTimeOffset,m_cellTimeOffsetOut);
   if (sc.isFailure())
     ATH_MSG_ERROR ( "Could not record LArCellTimeOffset with key '" << m_cellTimeOffsetOut << "' to StoreGate" );

 }
 
 if ( m_Nconv > 0 ) m_nIterAverage /= m_Nconv ;
 ATH_MSG_INFO ( "===============================================================" );
 ATH_MSG_INFO ( "Summary:  " << m_NaboveThreshold << " out of " << m_Nevents 
                << " events have at least one cell above threshold (ADC cut = " << m_AdcCut << ")");
 ATH_MSG_INFO ( "Summary: iteration converged for " << m_Nconv << " out of " << m_NaboveThreshold << " events above threshold" );
 ATH_MSG_INFO ( "Summary: average number of iteration(s) = " << m_nIterAverage );
 ATH_MSG_INFO ( "===============================================================" );
 
 return StatusCode::SUCCESS;
}
