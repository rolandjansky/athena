/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArWaveCumul.h"

#include "LArCalibTools/LArWaves2Ntuple.h"

LArWaves2Ntuple::LArWaves2Ntuple(const std::string& name, ISvcLocator* pSvcLocator): LArCond2NtupleBase(name, pSvcLocator)
{ 

  declareProperty("SaveDerivedInfo",m_saveDerivedInfo=false);
}

LArWaves2Ntuple::~LArWaves2Ntuple()
{}

StatusCode LArWaves2Ntuple::initialize() {
  
  StatusCode sc=LArCond2NtupleBase::initialize();
  if (sc.isFailure()) 
    return sc;
  
  sc=m_nt->addItem("flag",m_flag,0,1000);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'Flag' failed" );
    return StatusCode::FAILURE;
  }
  
  sc=m_nt->addItem("timeIndex",m_timeIndex,0,100000);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'timeIndex' failed" );
    return StatusCode::FAILURE;
  }
  
  sc=m_nt->addItem("Dt",m_dt,0.,25.);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'Dt' failed" );
    return StatusCode::FAILURE;
  }

  sc=m_nt->addItem("Time",m_timeIndex,m_time);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'Time' failed" );
    return StatusCode::FAILURE;
  }
  
  sc=m_nt->addItem("Amplitude",m_timeIndex,m_ampliPulse);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem failed" );
    return StatusCode::FAILURE;
  }
  
  sc=m_nt->addItem("Error",m_timeIndex,m_errorPulse);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'Error' failed" );
    return StatusCode::FAILURE;
  }
  
  sc=m_nt->addItem("Triggers",m_timeIndex,m_triggers);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'Triggers' failed" );
    return StatusCode::FAILURE;
  }
 
  if ( m_saveDerivedInfo ) {
    sc=m_nt->addItem("Baseline",m_baseline,0.,1200.);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'Baseline' failed" );
      return StatusCode::FAILURE;
    }

    sc=m_nt->addItem("TmaxAmp",m_tmaxAmp,0.,4095.);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'TmaxAmp' failed" );
      return StatusCode::FAILURE;
    }

    sc=m_nt->addItem("MaxAmp",m_maxAmp,0.,4095.);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'MaxAmp' failed" );
      return StatusCode::FAILURE;
    }
    
    sc=m_nt->addItem("PosLobe",m_posLobe,0.,100.);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'PosLobe' failed" );
      return StatusCode::FAILURE;
    }

    sc=m_nt->addItem("T0",m_rT0,20.,100.);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'RT0' failed" );
      return StatusCode::FAILURE;
    }

    sc=m_nt->addItem("Width",m_width,0.,125.);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'Width' failed" );
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}


bool LArWaves2Ntuple::fillWave(const HWIdentifier chid, const LArWaveCumul& wave) {
  fillFromIdentifier(chid);
  m_flag = wave.getFlag();
  m_dt   = wave.getDt();
      
  for (m_timeIndex=0; m_timeIndex<(long)wave.getSize(); m_timeIndex++ ) {
    m_time[m_timeIndex]       = wave.getTime(m_timeIndex);
    m_ampliPulse[m_timeIndex] = wave.getSample(m_timeIndex);
    m_errorPulse[m_timeIndex] = wave.getError(m_timeIndex);
    m_triggers[m_timeIndex]   = wave.getTrigger(m_timeIndex);
  }

  if ( m_saveDerivedInfo ) { // additional (derived) information, computed on the fly...
    m_baseline = m_waveHelper.getBaseline(wave,10);
    m_maxAmp   = m_waveHelper.getMaxAmp(wave);
    // please note the 'dt' normalization in the following!
    m_tmaxAmp  = m_dt * m_waveHelper.getMax(wave);
    m_width    = m_dt * m_waveHelper.getWidth(wave);
    m_rT0      = m_dt * m_waveHelper.getT0(wave);
    m_posLobe  = m_dt * m_waveHelper.getSumRegion(wave,0,m_waveHelper.getZeroCross(wave))/m_maxAmp;
  }
  return true;
}

