/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBRec/CBNTAA_TBTPValidation.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/INTupleSvc.h"

#include "TBEvent/TBADCRawCont.h"
#include "TBEvent/TBTDCRawCont.h"
#include "TBEvent/TBPhase.h"
#include "TBEvent/TBEventInfo.h"
#include "TBEvent/TBBPCCont.h"
#include "TBEvent/TBLArDigitContainer.h"
#include "TBEvent/TBScintillatorCont.h"
#include "TBEvent/TBTailCatcher.h"
#include "TBEvent/TBTDC.h"
#include "TBEvent/TBTrack.h"
#include "TBEvent/TBTriggerPatternUnit.h"
#include "TBEvent/TBMWPCCont.h"


CBNTAA_TBTPValidation::CBNTAA_TBTPValidation(const std::string & name, ISvcLocator * pSvcLocator) : CBNT_TBRecBase(name, pSvcLocator)
{
  declareProperty("NeverReturnFailure", m_neverReturnFailure=false);
	
  // TBADCRawCont
  declareProperty("ContainerKey1",m_containerKey1="ADCRawCont");
	m_adc=NULL;
	m_tbDetectorName_TBADCRawCont=NULL;
	m_overflow_TBADCRawCont=NULL;
  // TBTDCRawCont
	declareProperty("ContainerKey2",m_containerKey2="TDCRawCont");
	m_tdc_raw=NULL;
	m_underThreshold_raw=NULL;
	m_tbDetectorName_TBTDCRawCont=NULL;
	m_overflow_TBTDCRawCont=NULL;
  // TBPhase
	declareProperty("ContainerKey3",m_containerKey3="TBPhase");
	m_phase=NULL;
	m_phaseind=NULL;
	m_dTtoWAC=NULL;
  // TBEventInfo
	declareProperty("m_containerKey4",m_containerKey4="TBEventInfo");
	m_ev_number=NULL;
	m_ev_clock=NULL;
	m_ev_type=NULL;
	m_run_num=NULL;
	m_beam_moment=NULL;
	m_beam_part=NULL;
	m_cryoX=NULL;
	m_cryoAngle=NULL;
	m_tableY=NULL;
  // TBBPCCont
	declareProperty("m_containerKey5",m_containerKey5="BPCCont");
	m_xPos=NULL;
	m_yPos=NULL;
	m_xErr=NULL;
	m_yErr=NULL;
	m_xPulse=NULL;
	m_yPulse=NULL;
	m_hitnumber=NULL;
	m_xPosOverflow=NULL;
	m_yPosOverflow=NULL;
	m_xPulseOverflow=NULL;
	m_yPulseOverflow=NULL;
	m_overflowSetFlag=NULL;
	m_tbDetectorName_TBBPCCont=NULL;
	m_overflow_TBBPCCont=NULL;
  // TBLArDigitContainer
	declareProperty("m_containerKey6",m_containerKey6="FREE");
	m_channelID=NULL;
	m_gain=NULL;
	m_nSamples=NULL;
	m_samples=NULL;
  // TBScintillatorCont
	declareProperty("m_containerKey7",m_containerKey7="ScintillatorCont");
	m_signal_scint=NULL;
	m_time_signal_scint=NULL;
	m_signal_overflow_scint=NULL;
	m_time_overflow_scint=NULL;
	m_tbDetectorName_TBScintillatorCont=NULL;
	m_overflow_TBScintillatorCont=NULL;
  // TBTailCatcher
	declareProperty("m_containerKey8",m_containerKey8="TailCatcher");
	m_signals=NULL;
	m_signal_tCatch=NULL;
	m_time_signal_tCatch=NULL;
	m_signal_overflow_tCatch=NULL;
	m_time_overflow_tCatch=NULL;
	m_tbDetectorName_TBTailCatcher=NULL;
	m_overflow_TBTailCatcher=NULL;
  // TBTDC 
 	declareProperty("m_containerKey9",m_containerKey9="TBTDC");
	m_tdc_TBTDC=NULL;
	m_tdcmin_TBTDC=NULL;
	m_scale_TBTDC=NULL;
	m_phase_TBTDC=NULL;
  // TBTrack 
  declareProperty("m_containerKey10",m_containerKey10="Track");
	m_hitNumberU=NULL;
	m_hitNumberV=NULL;
	m_residualu=NULL;
	m_residualv=NULL;
	m_chi2=NULL;
	m_chi2u=NULL;
	m_chi2v=NULL;
	m_angle=NULL;
	m_uslope=NULL;
	m_vslope=NULL;
	m_uintercept=NULL;
	m_vintercept=NULL;
	m_cryou=NULL;
	m_cryov=NULL;
	m_cryow=NULL;
  // TBTriggerPatternUnit
  declareProperty("m_containerKey11",m_containerKey11="TBTrigPat");
	m_triggerWord=NULL;
	m_triggers=NULL;
  // TBMWPCCont
	declareProperty("m_containerKey12",m_containerKey12="MWPCCont");
	m_cPos=NULL;
	m_cErr=NULL;
	m_isX=NULL;
	m_clusterSize_c=NULL;
	m_cPosOverflow=NULL;
	m_tbDetectorName_TBMWPCCont=NULL;
	m_overflow_TBMWPCCont=NULL;
}


CBNTAA_TBTPValidation::~CBNTAA_TBTPValidation() 
{//Clean up arrays of ntuple entries (if they have been booked)

  // TBADCRawCont
  if (m_adc) delete m_adc;
  if (m_tbDetectorName_TBADCRawCont) delete m_tbDetectorName_TBADCRawCont;
  if (m_overflow_TBADCRawCont) delete m_overflow_TBADCRawCont;
  // TBTDCRawCont
  if (m_tdc_raw) delete m_tdc_raw;
	if (m_underThreshold_raw) delete m_underThreshold_raw;
	if (m_tbDetectorName_TBTDCRawCont) delete m_tbDetectorName_TBTDCRawCont;
	if (m_overflow_TBTDCRawCont) delete m_overflow_TBTDCRawCont;
  // TBPhase
	if (m_phase) delete m_phase;
	if (m_phaseind) delete m_phaseind;
	if (m_dTtoWAC) delete m_dTtoWAC;
  // TBEventInfo
	if (m_ev_number) delete  m_ev_number;
	if (m_ev_clock) delete  m_ev_clock;
	if (m_ev_type) delete  m_ev_type;
	if (m_run_num) delete  m_run_num;
	if (m_beam_moment) delete  m_beam_moment;
	if (m_beam_part) delete  m_beam_part;
	if (m_cryoX) delete  m_cryoX;
	if (m_cryoAngle) delete  m_cryoAngle;
	if (m_tableY) delete  m_tableY;
  // TBBPCCont
	if (m_xPos) delete  m_xPos;
	if (m_yPos) delete  m_yPos;
	if (m_xErr) delete  m_xErr;
	if (m_yErr) delete  m_yErr;
	if (m_xPulse) delete  m_xPulse;
	if (m_yPulse) delete  m_yPulse;
	if (m_hitnumber) delete  m_hitnumber;
	if (m_xPosOverflow) delete  m_xPosOverflow;
	if (m_yPosOverflow) delete  m_yPosOverflow;
	if (m_xPulseOverflow) delete  m_xPulseOverflow;
	if (m_yPulseOverflow) delete  m_yPulseOverflow;
	if (m_overflowSetFlag) delete  m_overflowSetFlag;
	if (m_tbDetectorName_TBBPCCont) delete  m_tbDetectorName_TBBPCCont;
	if (m_overflow_TBBPCCont) delete  m_overflow_TBBPCCont;
  // TBLArDigitContainer
	if (m_channelID) delete  m_channelID;
	if (m_gain) delete  m_gain;
	if (m_nSamples) delete  m_nSamples;
	if (m_samples) delete  m_samples;
  // TBScintillatorCont
	if (m_signal_scint) delete  m_signal_scint;
	if (m_time_signal_scint) delete  m_time_signal_scint;
	if (m_signal_overflow_scint) delete  m_signal_overflow_scint;
	if (m_time_overflow_scint) delete  m_time_overflow_scint;
	if (m_tbDetectorName_TBScintillatorCont) delete  m_tbDetectorName_TBScintillatorCont;
	if (m_overflow_TBScintillatorCont) delete  m_overflow_TBScintillatorCont;
  // TBTailCatcher
	if (m_signals) delete  m_signals;
	if (m_signal_tCatch) delete  m_signal_tCatch;
	if (m_time_signal_tCatch) delete  m_time_signal_tCatch;
	if (m_signal_overflow_tCatch) delete  m_signal_overflow_tCatch;
	if (m_time_overflow_tCatch) delete  m_time_overflow_tCatch;
	if (m_tbDetectorName_TBTailCatcher) delete  m_tbDetectorName_TBTailCatcher;
	if (m_overflow_TBTailCatcher) delete  m_overflow_TBTailCatcher;
  // TBTDC 
	if (m_tdc_TBTDC) delete  m_tdc_TBTDC;
	if (m_tdcmin_TBTDC) delete  m_tdcmin_TBTDC;
	if (m_scale_TBTDC) delete  m_scale_TBTDC;
	if (m_phase_TBTDC) delete  m_phase_TBTDC;
  // TBTrack 
	if (m_hitNumberU) delete  m_hitNumberU;
	if (m_hitNumberV) delete  m_hitNumberV;
	if (m_residualu) delete  m_residualu;
	if (m_residualv) delete  m_residualv;
	if (m_chi2) delete  m_chi2;
	if (m_chi2u) delete  m_chi2u;
	if (m_chi2v) delete  m_chi2v;
	if (m_angle) delete  m_angle;
	if (m_uslope) delete  m_uslope;
	if (m_vslope) delete  m_vslope;
	if (m_uintercept) delete  m_uintercept;
	if (m_vintercept) delete  m_vintercept;
	if (m_cryou) delete  m_cryou;
	if (m_cryov) delete  m_cryov;
	if (m_cryow) delete  m_cryow;
  // TBTriggerPatternUnit
	if (m_triggerWord) delete  m_triggerWord;
	if (m_triggers) delete  m_triggers;
  // TBMWPCCont
	if (m_cPos) delete  m_cPos;
	if (m_cErr) delete  m_cErr;
	if (m_isX) delete  m_isX;
	if (m_clusterSize_c) delete  m_clusterSize_c;
	if (m_cPosOverflow) delete  m_cPosOverflow;
	if (m_tbDetectorName_TBMWPCCont) delete  m_tbDetectorName_TBMWPCCont;
	if (m_overflow_TBMWPCCont) delete  m_overflow_TBMWPCCont;
}



StatusCode CBNTAA_TBTPValidation::CBNT_initialize()
{
  ATH_MSG_DEBUG ( "in initialize()" );

  IToolSvc* toolSvc = nullptr;
  ATH_CHECK( service( "ToolSvc",toolSvc  ) );

  // TBADCRawCont
  addBranch("TBTPValid_TBADCRaw_m_adc",m_adc);
  addBranch("TBTPValid_TBADCRaw_tbDetectorName",m_tbDetectorName_TBADCRawCont);
  addBranch("TBTPValid_TBADCRaw_overflow",m_overflow_TBADCRawCont);
  // TBTDCRawCont
  addBranch("TBTPValid_TBTDCRaw_tdc",m_tdc_raw);
  addBranch("TBTPValid_TBTDCRaw_underThreshold",m_underThreshold_raw);
  addBranch("TBTPValid_TBTDCRaw_tbDetectorName",m_tbDetectorName_TBTDCRawCont);
  addBranch("TBTPValid_TBTDCRaw_overflow",m_overflow_TBTDCRawCont);
  // TBPhase
  addBranch("TBTPValid_TBPhase_phase",m_phase);
  addBranch("TBTPValid_TBPhase_phaseind",m_phaseind);
  addBranch("TBTPValid_TBPhase_dTtoWAC",m_dTtoWAC);
  // TBEventInfo
	addBranch("TBTPValid_TBEventInfo_ev_number",  m_ev_number);
	addBranch("TBTPValid_TBEventInfo_ev_clock",  m_ev_clock);
	addBranch("TBTPValid_TBEventInfo_ev_type",  m_ev_type);
	addBranch("TBTPValid_TBEventInfo_run_num",  m_run_num);
	addBranch("TBTPValid_TBEventInfo_beam_moment",  m_beam_moment);
	addBranch("TBTPValid_TBEventInfo_beam_part",  m_beam_part);
	addBranch("TBTPValid_TBEventInfo_cryoX",  m_cryoX);
	addBranch("TBTPValid_TBEventInfo_cryoAngle",  m_cryoAngle);
	addBranch("TBTPValid_TBEventInfo_tableY",  m_tableY);
  // TBBPCCont
	addBranch("TBTPValid_TBBPCCont_xPos",  m_xPos);
	addBranch("TBTPValid_TBBPCCont_yPos",  m_yPos);
	addBranch("TBTPValid_TBBPCCont_xErr",  m_xErr);
	addBranch("TBTPValid_TBBPCCont_yErr",  m_yErr);
	addBranch("TBTPValid_TBBPCCont_xPulse",  m_xPulse);
	addBranch("TBTPValid_TBBPCCont_yPulse",  m_yPulse);
	addBranch("TBTPValid_TBBPCCont_hitnumber",  m_hitnumber);
	addBranch("TBTPValid_TBBPCCont_xPosOverflow",  m_xPosOverflow);
	addBranch("TBTPValid_TBBPCCont_yPosOverflow",  m_yPosOverflow);
	addBranch("TBTPValid_TBBPCCont_xPulseOverflow",  m_xPulseOverflow);
	addBranch("TBTPValid_TBBPCCont_yPulseOverflow",  m_yPulseOverflow);
	addBranch("TBTPValid_TBBPCCont_overflowSetFlag",  m_overflowSetFlag);
	addBranch("TBTPValid_TBBPCCont_tbDetectorName",  m_tbDetectorName_TBBPCCont);
	addBranch("TBTPValid_TBBPCCont_overflow",  m_overflow_TBBPCCont);
  // TBLArDigitContainer
	addBranch("TBTPValid_TBLArDigitContainer_channelID",  m_channelID);
	addBranch("TBTPValid_TBLArDigitContainer_gain",  m_gain);
	addBranch("TBTPValid_TBLArDigitContainer_nSamples",  m_nSamples);
	addBranch("TBTPValid_TBLArDigitContainer_samples",  m_samples);
  // TBScintillatorCont
	addBranch("TBTPValid_TBScintillatorCont_signal",  m_signal_scint);
	addBranch("TBTPValid_TBScintillatorCont_time_signal",  m_time_signal_scint);
	addBranch("TBTPValid_TBScintillatorCont_signal_overflow",  m_signal_overflow_scint);
	addBranch("TBTPValid_TBScintillatorCont_time_overflow",  m_time_overflow_scint);
	addBranch("TBTPValid_TBScintillatorCont_tbDetectorName",  m_tbDetectorName_TBScintillatorCont);
	addBranch("TBTPValid_TBScintillatorCont_overflow",  m_overflow_TBScintillatorCont);
  // TBTailCatcher
	addBranch("TBTPValid_TBTailCatcher_signals",  m_signals);
	addBranch("TBTPValid_TBTailCatcher_signal",  m_signal_tCatch);
	addBranch("TBTPValid_TBTailCatcher_time_signal",  m_time_signal_tCatch);
	addBranch("TBTPValid_TBTailCatcher_signal_overflow",  m_signal_overflow_tCatch);
	addBranch("TBTPValid_TBTailCatcher_time_overflow",  m_time_overflow_tCatch);
	addBranch("TBTPValid_TBTailCatcher_tbDetectorName",  m_tbDetectorName_TBTailCatcher);
	addBranch("TBTPValid_TBTailCatcher_overflow",  m_overflow_TBTailCatcher);
  // TBTDC 
	addBranch("TBTPValid_TBTDC_tdc",  m_tdc_TBTDC);
	addBranch("TBTPValid_TBTDC_tdcmin",  m_tdcmin_TBTDC);
	addBranch("TBTPValid_TBTDC_scale",  m_scale_TBTDC);
	addBranch("TBTPValid_TBTDC_phase",  m_phase_TBTDC);
  // TBTrack 
	addBranch("TBTPValid_TBTrack_hitNumberU",  m_hitNumberU);
	addBranch("TBTPValid_TBTrack_hitNumberV",  m_hitNumberV);
	addBranch("TBTPValid_TBTrack_residualu",  m_residualu);
	addBranch("TBTPValid_TBTrack_residualv",  m_residualv);
	addBranch("TBTPValid_TBTrack_chi2",  m_chi2);
	addBranch("TBTPValid_TBTrack_chi2u",  m_chi2u);
	addBranch("TBTPValid_TBTrack_chi2v",  m_chi2v);
	addBranch("TBTPValid_TBTrack_angle",  m_angle);
	addBranch("TBTPValid_TBTrack_uslope",  m_uslope);
	addBranch("TBTPValid_TBTrack_vslope",  m_vslope);
	addBranch("TBTPValid_TBTrack_uintercept",  m_uintercept);
	addBranch("TBTPValid_TBTrack_vintercept",  m_vintercept);
	addBranch("TBTPValid_TBTrack_cryou",  m_cryou);
	addBranch("TBTPValid_TBTrack_cryov",  m_cryov);
	addBranch("TBTPValid_TBTrack_cryow",  m_cryow);
  // TBTriggerPatternUnit
	addBranch("TBTPValid_TBTriggerPatternUnit_triggerWord",  m_triggerWord);
	addBranch("TBTPValid_TBTriggerPatternUnit_triggers",  m_triggers);
  // TBMWPCCont
	addBranch("TBTPValid_TBMWPCCont_cPos",  m_cPos);
	addBranch("TBTPValid_TBMWPCCont_cErr",  m_cErr);
	addBranch("TBTPValid_TBMWPCCont_isX",  m_isX);
	addBranch("TBTPValid_TBMWPCCont_clusterSize_c",  m_clusterSize_c);
	addBranch("TBTPValid_TBMWPCCont_cPosOverflow",  m_cPosOverflow);
	addBranch("TBTPValid_TBMWPCCont_tbDetectorName",  m_tbDetectorName_TBMWPCCont);
	addBranch("TBTPValid_TBMWPCCont_overflow",  m_overflow_TBMWPCCont);

  return StatusCode::SUCCESS; 
  
}


StatusCode CBNTAA_TBTPValidation::CBNT_execute()
{
  // ----------------------------------------------------------------------------------------------------
  // TBADCRawCont  
  // ----------------------------------------------------------------------------------------------------
  const TBADCRawCont * adcCont;
  StatusCode sc = evtStore()->retrieve(adcCont,m_containerKey1);
  if (sc.isFailure()) {
    ATH_MSG_ERROR ( "\033[31m" << "- Can not read TBADCRawCont from StoreGate! key= " << m_containerKey1 << "\033[0m" );
  }  else  {
    ATH_MSG_DEBUG ( "\033[34m" << "- Going over TBADCRawCont ( StoreGate key =" << m_containerKey1 << " ) ..."<< "\033[0m" );

    const unsigned nADC = (TBADCRawCont::size_type)adcCont->size();
    m_adc->resize(nADC);
    m_tbDetectorName_TBADCRawCont->resize(nADC);
    m_overflow_TBADCRawCont->resize(nADC);

    unsigned NtupleVectorIndex = 0;
    TBADCRawCont::const_iterator it_adc   = adcCont->begin();
    TBADCRawCont::const_iterator last_adc = adcCont->end();
    for(;it_adc!=last_adc;it_adc++,NtupleVectorIndex++) {
      const TBADCRaw * adc = (*it_adc);
      (*m_adc)[NtupleVectorIndex]                          = adc-> getADC();
      (*m_tbDetectorName_TBADCRawCont)[NtupleVectorIndex]  = adc-> getDetectorName();	
      (*m_overflow_TBADCRawCont)[NtupleVectorIndex]        = adc-> isOverflow();
    }  
  }

  // ----------------------------------------------------------------------------------------------------
  // TBTDCRawCont  
  // ----------------------------------------------------------------------------------------------------
  const TBTDCRawCont * tdcCont;
  sc = evtStore()->retrieve(tdcCont,m_containerKey2);
  if (sc.isFailure()) {
    ATH_MSG_ERROR ( "\033[31m" << "- Can not read TBTDCRawCont from StoreGate! key= " << m_containerKey2 << "\033[0m" );
  }  else  {
    ATH_MSG_DEBUG ( "\033[34m" << "- Going over TBTDCRawCont ( StoreGate key =" << m_containerKey2 << " ) ..."<< "\033[0m" );

    const unsigned nTDC = (TBTDCRawCont::size_type)tdcCont->size();
    m_tdc_raw->resize(nTDC);
    m_underThreshold_raw->resize(nTDC);
    m_tbDetectorName_TBTDCRawCont->resize(nTDC);
    m_overflow_TBTDCRawCont->resize(nTDC);

    unsigned NtupleVectorIndex = 0;
    TBTDCRawCont::const_iterator it_tdc   = tdcCont->begin();
    TBTDCRawCont::const_iterator last_tdc = tdcCont->end();
    for(;it_tdc!=last_tdc;it_tdc++,NtupleVectorIndex++) {
      const TBTDCRaw * tdc = (*it_tdc);
      (*m_tdc_raw)[NtupleVectorIndex]                      = tdc-> getTDC();
      (*m_underThreshold_raw)[NtupleVectorIndex]           = tdc-> isUnderThreshold();
      (*m_tbDetectorName_TBTDCRawCont)[NtupleVectorIndex]  = tdc-> getDetectorName();
      (*m_overflow_TBTDCRawCont)[NtupleVectorIndex]        = tdc-> isOverflow();
    }
  }

  // ----------------------------------------------------------------------------------------------------
  // TBPhase  
  // ----------------------------------------------------------------------------------------------------
  const TBPhase * phase = nullptr;
  sc = evtStore()->retrieve(phase,m_containerKey3);
  if (sc.isFailure()) {
    ATH_MSG_ERROR ( "\033[31m" << "- Can not read TBPhase from StoreGate! key= " << m_containerKey3 << "\033[0m" );
  }  else  {	
    ATH_MSG_DEBUG ( "\033[34m" << "- Going over TBPhase ( StoreGate key =" << m_containerKey3 << " ) ..."<< "\033[0m" );

    m_phase     ->resize(1);
    m_phaseind  ->resize(1);
    m_dTtoWAC   ->resize(1);

    (*m_phase)[0]    = phase-> getPhase();
    (*m_phaseind)[0] = phase-> getPhaseind();
    (*m_dTtoWAC)[0]  = phase-> getdTtoWACSigned();
  }

  // ----------------------------------------------------------------------------------------------------
  // TBEventInfo  
  // ----------------------------------------------------------------------------------------------------
  const TBEventInfo * EventInfo = nullptr;
  sc = evtStore()->retrieve(EventInfo,m_containerKey4);
  if (sc.isFailure()) {
    ATH_MSG_ERROR ( "\033[31m" << "- Can not read EventInfo from StoreGate! key= " << m_containerKey4 << "\033[0m" );
  }  else  {
    ATH_MSG_DEBUG ( "\033[34m" << "- Going over EventInfo ( StoreGate key =" << m_containerKey4 << " ) ..."<< "\033[0m" );

    m_ev_number   ->resize(1);
    m_ev_clock    ->resize(1);
    m_ev_type     ->resize(1);
    m_run_num     ->resize(1);
    m_beam_moment ->resize(1);
    m_beam_part   ->resize(1);
    m_cryoX       ->resize(1);
    m_cryoAngle   ->resize(1);
    m_tableY      ->resize(1);

    (*m_ev_number)[0]    = EventInfo-> getEventNum();
    (*m_ev_clock)[0]     = EventInfo-> getEventClock();
    (*m_ev_type)[0]      = EventInfo-> getEventType();
    (*m_run_num)[0]      = EventInfo-> getRunNum();
    (*m_beam_moment)[0]  = EventInfo-> getBeamMomentum();
    (*m_beam_part)[0]    = EventInfo-> getBeamParticle();
    (*m_cryoX)[0]        = EventInfo-> getCryoX();
    (*m_cryoAngle)[0]    = EventInfo-> getCryoAngle();
    (*m_tableY)[0]       = EventInfo-> getTableY();
  }


  // ----------------------------------------------------------------------------------------------------
  // TBBPCCont  
  // ----------------------------------------------------------------------------------------------------
  const TBBPCCont * BPCCont = nullptr;
  sc = evtStore()->retrieve(BPCCont,m_containerKey5);
  if (sc.isFailure()) {
    ATH_MSG_ERROR ( "\033[31m" << "- Can not read TBBPCCont from StoreGate! key= " << m_containerKey5 << "\033[0m" );
  }  else  {
    ATH_MSG_DEBUG ( "\033[34m" << "- Going over TBBPCCont ( StoreGate key =" << m_containerKey5 << " ) ..."<< "\033[0m" );

    const unsigned nBPCCont = (TBBPCCont::size_type)BPCCont->size();
    m_xPos                      ->resize(nBPCCont);
    m_yPos                      ->resize(nBPCCont);
    m_xErr                      ->resize(nBPCCont);
    m_yErr                      ->resize(nBPCCont);
    m_xPulse                    ->resize(nBPCCont);
    m_yPulse                    ->resize(nBPCCont);
    m_hitnumber                 ->resize(nBPCCont);
    m_xPosOverflow              ->resize(nBPCCont);
    m_yPosOverflow              ->resize(nBPCCont);
    m_xPulseOverflow            ->resize(nBPCCont);
    m_yPulseOverflow            ->resize(nBPCCont);
    m_overflowSetFlag           ->resize(nBPCCont);
    m_tbDetectorName_TBBPCCont  ->resize(nBPCCont);
    m_overflow_TBBPCCont        ->resize(nBPCCont);
     
    unsigned NtupleVectorIndex = 0;
    TBBPCCont::const_iterator it_TBBPCCont  = BPCCont->begin();
    TBBPCCont::const_iterator last_BBPCCont = BPCCont->end();
    for(;it_TBBPCCont!=last_BBPCCont;it_TBBPCCont++,NtupleVectorIndex++) {
       const TBBPC * bpc = (*it_TBBPCCont);
     	(*m_xPos)[NtupleVectorIndex]                      = bpc-> getXPos();
     	(*m_yPos)[NtupleVectorIndex]                      = bpc-> getYPos();
     	(*m_xErr)[NtupleVectorIndex]                      = bpc-> getXErr();
     	(*m_yErr)[NtupleVectorIndex]                      = bpc-> getYErr();
     	(*m_xPulse)[NtupleVectorIndex]                    = bpc-> getXPulse();
     	(*m_yPulse)[NtupleVectorIndex]                    = bpc-> getYPulse();
     	(*m_hitnumber)[NtupleVectorIndex]                 = bpc-> getHitNbr();
     	(*m_xPosOverflow)[NtupleVectorIndex]              = bpc-> isXPosOverflow();
     	(*m_yPosOverflow)[NtupleVectorIndex]              = bpc-> isYPosOverflow();
     	(*m_xPulseOverflow)[NtupleVectorIndex]            = bpc-> isXPulseOverflow();
     	(*m_yPulseOverflow)[NtupleVectorIndex]            = bpc-> isYPulseOverflow();
     	(*m_overflowSetFlag)[NtupleVectorIndex]           = false;//bpc-> m_overflowSetFlag ;
     	(*m_tbDetectorName_TBBPCCont)[NtupleVectorIndex]  = bpc-> getDetectorName();
     	(*m_overflow_TBBPCCont)[NtupleVectorIndex]        = bpc-> isOverflow();
    }
  }


  // ----------------------------------------------------------------------------------------------------
  // TBLArDigitContainer    // FIXME !!!  probably does not work right.... //
  // ----------------------------------------------------------------------------------------------------
  const TBLArDigitContainer * LArDigitContainer = nullptr;
  sc = evtStore()->retrieve(LArDigitContainer,m_containerKey6);
  if (sc.isFailure()) {
    ATH_MSG_ERROR ( "\033[31m" << "- Can not read LArDigitContainer from StoreGate! key= " << m_containerKey6 << "\033[0m" );
    }  else  {
    ATH_MSG_DEBUG ( "\033[34m" << "- Going over LArDigitContainer ( StoreGate key =" << m_containerKey6 << " ) ..."<< "\033[0m" );

    const unsigned nLArDigits = (TBLArDigitContainer::size_type)LArDigitContainer->size();
    m_channelID->resize(nLArDigits);
    m_gain->resize(nLArDigits);
    m_nSamples->resize(nLArDigits);
    if (nLArDigits) 
      m_samples->reserve( ( (*(LArDigitContainer->begin()))->nsamples() ) * nLArDigits);

    unsigned NtupleVectorIndex = 0;
    TBLArDigitContainer::const_iterator it_LArDigitContainer   = LArDigitContainer->begin();
    TBLArDigitContainer::const_iterator last_LArDigitContainer = LArDigitContainer->end();
    for(;it_LArDigitContainer!=last_LArDigitContainer;it_LArDigitContainer++,NtupleVectorIndex++) {
      const LArDigit * larDigit = (*it_LArDigitContainer);
      (*m_channelID)[NtupleVectorIndex] = larDigit->hardwareID().get_identifier32().get_compact() ;
      (*m_gain)[NtupleVectorIndex] = (unsigned char)larDigit->gain();

      unsigned nM_samples = larDigit->nsamples();
      (*m_nSamples)[NtupleVectorIndex] = nM_samples;
        
      m_samples->assign (larDigit->samples().begin(),
                         larDigit->samples().end());
    }
  }


  // ----------------------------------------------------------------------------------------------------
  // TBScintillatorCont  
  // ----------------------------------------------------------------------------------------------------
  const TBScintillatorCont * ScintillatorCont = nullptr;
  sc = evtStore()->retrieve(ScintillatorCont,m_containerKey7);
  if (sc.isFailure()) {
    ATH_MSG_ERROR ( "\033[31m" << "- Can not read TBScintillatorCont from StoreGate! key= " << m_containerKey7 << "\033[0m" );
  }  else  {
    ATH_MSG_DEBUG ( "\033[34m" << "- Going over TBScintillatorCont ( StoreGate key =" << m_containerKey7 << " ) ..."<< "\033[0m" );

    const unsigned nScintillators = (TBScintillatorCont::size_type)ScintillatorCont->size();
    m_signal_scint                        ->resize(nScintillators);
    m_time_signal_scint                   ->resize(nScintillators);
    m_signal_overflow_scint               ->resize(nScintillators);
    m_time_overflow_scint                 ->resize(nScintillators);
    m_tbDetectorName_TBScintillatorCont   ->resize(nScintillators);
    m_overflow_TBScintillatorCont         ->resize(nScintillators);


    unsigned NtupleVectorIndex = 0;
    TBScintillatorCont::const_iterator it_TBScintillatorCont   = ScintillatorCont->begin();
    TBScintillatorCont::const_iterator last_TBScintillatorCont = ScintillatorCont->end();
    for(;it_TBScintillatorCont!=last_TBScintillatorCont;++it_TBScintillatorCont,NtupleVectorIndex++) {
      // scint->isTimeOverflow() is not a constant method, and so we define here
      // 'TBScintillator * scint'  instead of the usual  'const TBScintillator * scint'
      // since for a const TBScintillator all methods need to be const as well...
      const TBScintillator * scint = (*it_TBScintillatorCont);

      (*m_signal_scint)[NtupleVectorIndex]                        = scint-> getSignal();
      (*m_time_signal_scint)[NtupleVectorIndex]                   = scint-> getTimeSignal();
      (*m_signal_overflow_scint)[NtupleVectorIndex]               = scint-> isSignalOverflow();
      (*m_time_overflow_scint)[NtupleVectorIndex]                 = scint-> isTimeOverflow();
      (*m_tbDetectorName_TBScintillatorCont)[NtupleVectorIndex]   = scint-> getDetectorName();
      (*m_overflow_TBScintillatorCont)[NtupleVectorIndex]         = scint-> isOverflow();
    }
  }


  // ----------------------------------------------------------------------------------------------------
  // TBTailCatcher  
  // ----------------------------------------------------------------------------------------------------
  const TBTailCatcher * TailCatcher = nullptr;
  sc = evtStore()->retrieve(TailCatcher,m_containerKey8);
  if (sc.isFailure()) {
    ATH_MSG_ERROR ( "\033[31m" << "- Can not read TBTailCatcher from StoreGate! key= " << m_containerKey8 << "\033[0m" );
  }  else  {
    ATH_MSG_DEBUG ( "\033[34m" << "- Going over TBTailCatcher ( StoreGate key =" << m_containerKey8 << " ) ..."<< "\033[0m" );

    const unsigned nTailCatchers = (TBTailCatcher::size_type)TailCatcher->size();
    m_signals                         ->resize(nTailCatchers);
    m_signal_tCatch                   ->resize(nTailCatchers);
    m_time_signal_tCatch              ->resize(nTailCatchers);
    m_signal_overflow_tCatch          ->resize(nTailCatchers);
    m_time_overflow_tCatch            ->resize(nTailCatchers);
    m_tbDetectorName_TBTailCatcher    ->resize(nTailCatchers);
    m_overflow_TBTailCatcher          ->resize(nTailCatchers);

    (*m_signals) = TailCatcher->getSignals();

    unsigned NtupleVectorIndex = 0;
    TBTailCatcher::const_iterator it_TailCatcher   = TailCatcher->begin();
    TBTailCatcher::const_iterator last_TailCatcher = TailCatcher->end();
    for(;it_TailCatcher!=last_TailCatcher;++it_TailCatcher,NtupleVectorIndex++) {
      // scint->isTimeOverflow() is not a constant method, and so we define here
      // 'TBScintillator * scint'  instead of the usual  'const TBScintillator * scint'
      // since for a const TBScintillator all methods need to be const as well...
      const TBScintillator * scint = (*it_TailCatcher);

      (*m_signal_tCatch)[NtupleVectorIndex]                   = scint-> getSignal();
      (*m_time_signal_tCatch)[NtupleVectorIndex]              = scint-> getTimeSignal();
      (*m_signal_overflow_tCatch)[NtupleVectorIndex]          = scint-> isSignalOverflow();
      (*m_time_overflow_tCatch)[NtupleVectorIndex]            = scint-> isTimeOverflow();
      (*m_tbDetectorName_TBTailCatcher)[NtupleVectorIndex]    = scint-> getDetectorName();
      (*m_overflow_TBTailCatcher)[NtupleVectorIndex]          = scint-> isOverflow();
    }
  }


  // ----------------------------------------------------------------------------------------------------
  // TBTDC  
  // ----------------------------------------------------------------------------------------------------
  const TBTDC * TDC = nullptr;
  sc = evtStore()->retrieve(TDC,m_containerKey9);
  if (sc.isFailure()) {
    ATH_MSG_ERROR ( "\033[31m" << "- Can not read TBTDC from StoreGate! key= " << m_containerKey9 << "\033[0m" );
  }  else  {
    ATH_MSG_DEBUG ( "\033[34m" << "- Going over TBTDC ( StoreGate key =" << m_containerKey9 << " ) ..."<< "\033[0m" );

    m_tdc_TBTDC      ->resize(1);
    m_tdcmin_TBTDC   ->resize(1);
    m_scale_TBTDC    ->resize(1);
    m_phase_TBTDC    ->resize(1);

    (*m_tdc_TBTDC)[0]    = TDC-> tdc();
    (*m_tdcmin_TBTDC)[0] = TDC-> tdcmin();
    (*m_scale_TBTDC)[0]  = TDC-> scale();
    (*m_phase_TBTDC)[0]  = TDC-> phase();
  }


  // ----------------------------------------------------------------------------------------------------
  // TBTrack  
  // ----------------------------------------------------------------------------------------------------
  const TBTrack * Track = nullptr;
  sc = evtStore()->retrieve(Track,m_containerKey10);
  if (sc.isFailure()) {
    ATH_MSG_ERROR ( "\033[31m" << "- Can not read TBTrack from StoreGate! key= " << m_containerKey10 << "\033[0m" );
  }  else  {
    ATH_MSG_DEBUG ( "\033[34m" << "- Going over TBTrack ( StoreGate key =" << m_containerKey10 << " ) ..."<< "\033[0m" );

    m_hitNumberU      ->resize(1);
    m_hitNumberV      ->resize(1);
    m_chi2            ->resize(1);
    m_chi2u           ->resize(1);
    m_chi2v           ->resize(1);
    m_angle           ->resize(1);
    m_uslope          ->resize(1);
    m_vslope          ->resize(1);
    m_uintercept      ->resize(1);
    m_vintercept      ->resize(1);
    m_cryou           ->resize(1);
    m_cryov           ->resize(1);
    m_cryow           ->resize(1);

    (*m_hitNumberU)[0]    = Track->  getHitNumberU();
    (*m_hitNumberV)[0]    = Track->  getHitNumberV();
    (*m_chi2)[0]          = Track->  getChi2_global();
    (*m_chi2u)[0]         = Track->  getChi2_u();
    (*m_chi2v)[0]         = Track->  getChi2_v();
    (*m_angle)[0]         = Track->  getAngle();
    (*m_uslope)[0]        = Track->  getUslope();
    (*m_vslope)[0]        = Track->  getVslope();
    (*m_uintercept)[0]    = Track->  getUintercept();
    (*m_vintercept)[0]    = Track->  getVintercept();
    (*m_cryou)[0]         = Track->  getCryoHitu();
    (*m_cryov)[0]         = Track->  getCryoHitv();
    (*m_cryow)[0]         = Track->  getCryoHitw();
    (*m_residualu) = Track->  getResidualu();
    (*m_residualv) = Track->  getResidualv();
  }


  // ----------------------------------------------------------------------------------------------------
  // TBTriggerPatternUnit  
  // ----------------------------------------------------------------------------------------------------
  const TBTriggerPatternUnit * TriggerPatternUnit = nullptr;
  sc = evtStore()->retrieve(TriggerPatternUnit,m_containerKey11);
  if (sc.isFailure()) {
    ATH_MSG_ERROR ( "\033[31m" << "- Can not read TBTriggerPatternUnit from StoreGate! key= " << m_containerKey11 << "\033[0m" );
  }  else  {
    ATH_MSG_DEBUG ( "\033[34m" << "- Going over TBTriggerPatternUnit ( StoreGate key =" << m_containerKey11 << " ) ..."<< "\033[0m" );

    m_triggerWord->resize(1);
    (*m_triggerWord)[0] = TriggerPatternUnit->  getTriggerWord();

    (*m_triggers)       = TriggerPatternUnit->  getTriggers();
  }


  // ----------------------------------------------------------------------------------------------------
  // TBMWPCCont  
  // ----------------------------------------------------------------------------------------------------
  const TBMWPCCont * MWPCCont = nullptr;
  sc = evtStore()->retrieve(MWPCCont,m_containerKey12);
  if (sc.isFailure()) {
    ATH_MSG_ERROR ( "\033[31m" << "- Can not read TBMWPCCont from StoreGate! key= " << m_containerKey12 << "\033[0m" );
  }  else  {
    ATH_MSG_DEBUG ( "\033[34m" << "- Going over TBMWPCCont ( StoreGate key =" << m_containerKey12 << " ) ..."<< "\033[0m" );

    const unsigned nMWPCCont = (TBMWPCCont::size_type)MWPCCont->size();
    m_cPos                        ->resize(nMWPCCont);
    m_cErr                        ->resize(nMWPCCont);
    m_isX                         ->resize(nMWPCCont);
    m_clusterSize_c               ->resize(nMWPCCont);
    m_cPosOverflow                ->resize(nMWPCCont);
    m_tbDetectorName_TBMWPCCont   ->resize(nMWPCCont);
    m_overflow_TBMWPCCont         ->resize(nMWPCCont);

    unsigned NtupleVectorIndex = 0;
    TBMWPCCont::const_iterator it_TBMWPCCont   = MWPCCont->begin();
    TBMWPCCont::const_iterator last_TBMWPCCont = MWPCCont->end();
    for(;it_TBMWPCCont!=last_TBMWPCCont;++it_TBMWPCCont,NtupleVectorIndex++) {
      const TBMWPC * mwpc = (*it_TBMWPCCont);

      (*m_cPos)[NtupleVectorIndex]                      = mwpc-> getCPos();
      (*m_cErr)[NtupleVectorIndex]                      = mwpc-> getCErr();
      (*m_isX)[NtupleVectorIndex]                       = mwpc-> isX();
      (*m_clusterSize_c)[NtupleVectorIndex]             = mwpc-> getClusterSizeC();
      (*m_tbDetectorName_TBMWPCCont)[NtupleVectorIndex] = mwpc-> getDetectorName();
      (*m_overflow_TBMWPCCont)[NtupleVectorIndex]       = mwpc-> isOverflow();

      // ------------------------------------------------------------------------------------------------
      // for some reason a stright forward definition of m_cPosOverflow as
      // std::vector < std::vector<bool> > m_cPosOverflow;
      // does not work. perhaps due to the fact that std::vector<bool> is nonconforming (not
      // really a vector of bool variables, but some ). instead we defined
      // std::vector < std::vector<unsigned> > m_cPosOverflow;
      // and now need to convert the bool constituents of the vectors to unsigned
      // ------------------------------------------------------------------------------------------------
      unsigned nBools = mwpc->isCPosOverflow().size();    //lenght of the vector of bool in TBMWPC
      std::vector<unsigned>  cPosOverflowNow(nBools);   //defines lenght and initializes to zero
    
      for (unsigned nBoolNow=0; nBoolNow<nBools; nBoolNow++)
        if(mwpc->isCPosOverflow()[nBoolNow])
          cPosOverflowNow[nBoolNow] = 1;

      (*m_cPosOverflow)[NtupleVectorIndex] =  cPosOverflowNow;
      cPosOverflowNow.clear();
      // ------------------------------------------------------------------------------------------------
    }
  }

  return StatusCode::SUCCESS;
}  


StatusCode CBNTAA_TBTPValidation::CBNT_clear()
{

  // TBADCRawCont
  if (m_adc) m_adc->clear();
  if (m_tbDetectorName_TBADCRawCont) m_tbDetectorName_TBADCRawCont->clear();
  if (m_overflow_TBADCRawCont) m_overflow_TBADCRawCont->clear();
  // TBTDCRawCont
  if (m_tdc_raw) m_tdc_raw->clear();
  if (m_underThreshold_raw) m_underThreshold_raw->clear();
  if (m_tbDetectorName_TBTDCRawCont) m_tbDetectorName_TBTDCRawCont->clear();
  if (m_overflow_TBTDCRawCont) m_overflow_TBTDCRawCont->clear();
  // TBPhase
  if (m_phase) m_phase->clear();
  if (m_phaseind) m_phaseind->clear();
  if (m_dTtoWAC) m_dTtoWAC->clear();
  // TBEventInfo
	if (m_ev_number)   m_ev_number->clear();
	if (m_ev_clock)   m_ev_clock->clear();
	if (m_ev_type)   m_ev_type->clear();
	if (m_run_num)   m_run_num->clear();
	if (m_beam_moment)   m_beam_moment->clear();
	if (m_beam_part)   m_beam_part->clear();
	if (m_cryoX)   m_cryoX->clear();
	if (m_cryoAngle)   m_cryoAngle->clear();
	if (m_tableY)   m_tableY->clear();
  // TBBPCCont
	if (m_xPos)   m_xPos->clear();
	if (m_yPos)   m_yPos->clear();
	if (m_xErr)   m_xErr->clear();
	if (m_yErr)   m_yErr->clear();
	if (m_xPulse)   m_xPulse->clear();
	if (m_yPulse)   m_yPulse->clear();
	if (m_hitnumber)   m_hitnumber->clear();
	if (m_xPosOverflow)   m_xPosOverflow->clear();
	if (m_yPosOverflow)   m_yPosOverflow->clear();
	if (m_xPulseOverflow)   m_xPulseOverflow->clear();
	if (m_yPulseOverflow)   m_yPulseOverflow->clear();
	if (m_overflowSetFlag)   m_overflowSetFlag->clear();
	if (m_tbDetectorName_TBBPCCont)   m_tbDetectorName_TBBPCCont->clear();
	if (m_overflow_TBBPCCont)   m_overflow_TBBPCCont->clear();
  // TBLArDigitContainer
	if (m_channelID)   m_channelID->clear();
	if (m_gain)   m_gain->clear();
	if (m_nSamples)   m_nSamples->clear();
	if (m_samples)   m_samples->clear();
  // TBScintillatorCont
	if (m_signal_scint)   m_signal_scint->clear();
	if (m_time_signal_scint)   m_time_signal_scint->clear();
	if (m_signal_overflow_scint)   m_signal_overflow_scint->clear();
	if (m_time_overflow_scint)   m_time_overflow_scint->clear();
	if (m_tbDetectorName_TBScintillatorCont)   m_tbDetectorName_TBScintillatorCont->clear();
	if (m_overflow_TBScintillatorCont)   m_overflow_TBScintillatorCont->clear();
  // TBTailCatcher
	if (m_signals)   m_signals->clear();
	if (m_signal_tCatch)   m_signal_tCatch->clear();
	if (m_time_signal_tCatch)   m_time_signal_tCatch->clear();
	if (m_signal_overflow_tCatch)   m_signal_overflow_tCatch->clear();
	if (m_time_overflow_tCatch)   m_time_overflow_tCatch->clear();
	if (m_tbDetectorName_TBTailCatcher)   m_tbDetectorName_TBTailCatcher->clear();
	if (m_overflow_TBTailCatcher)   m_overflow_TBTailCatcher->clear();
  // TBTDC 
	if (m_tdc_TBTDC)   m_tdc_TBTDC->clear();
	if (m_tdcmin_TBTDC)   m_tdcmin_TBTDC->clear();
	if (m_scale_TBTDC)   m_scale_TBTDC->clear();
	if (m_phase_TBTDC)   m_phase_TBTDC->clear();
  // TBTrack 
	if (m_hitNumberU)   m_hitNumberU->clear();
	if (m_hitNumberV)   m_hitNumberV->clear();
	if (m_residualu)   m_residualu->clear();
	if (m_residualv)   m_residualv->clear();
	if (m_chi2)   m_chi2->clear();
	if (m_chi2u)   m_chi2u->clear();
	if (m_chi2v)   m_chi2v->clear();
	if (m_angle)   m_angle->clear();
	if (m_uslope)   m_uslope->clear();
	if (m_vslope)   m_vslope->clear();
	if (m_uintercept)   m_uintercept->clear();
	if (m_vintercept)   m_vintercept->clear();
	if (m_cryou)   m_cryou->clear();
	if (m_cryov)   m_cryov->clear();
	if (m_cryow)   m_cryow->clear();
  // TBTriggerPatternUnit
	if (m_triggerWord)   m_triggerWord->clear();
	if (m_triggers)   m_triggers->clear();
  // TBMWPCCont
	if (m_cPos)   m_cPos->clear();
	if (m_cErr)   m_cErr->clear();
	if (m_isX)   m_isX->clear();
	if (m_clusterSize_c)   m_clusterSize_c->clear();
	if (m_cPosOverflow)   m_cPosOverflow->clear();
	if (m_tbDetectorName_TBMWPCCont)   m_tbDetectorName_TBMWPCCont->clear();
	if (m_overflow_TBMWPCCont)   m_overflow_TBMWPCCont->clear();

  return StatusCode::SUCCESS;
}


StatusCode CBNTAA_TBTPValidation::CBNT_finalize()
{
  ATH_MSG_DEBUG ( "in finalize()" );
  return StatusCode::SUCCESS;
}

std::string CBNTAA_TBTPValidation::add_name(const char* base, const std::string extension) {
  std::string retval(base);
  for (unsigned i=0;i<extension.size();i++) {
    const char& ch=extension[i];
    if (ch=='=' || ch==':' || ch=='/')
      continue; //Inore these characters
    else if (ch=='-')
      retval.append("m");//replace by letter m
    else if (ch=='+')
      retval.append("p");//replace by letter p
    else
      retval.append(1,ch);
  }
  return retval;
}
