/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArRawEvent/LArOFIterResults.h"

// Set Methods

void LArOFIterResults::setChannelID( HWIdentifier chid ){
  m_chid = chid;
}

void LArOFIterResults::setGain( CaloGain::CaloGain gain ){
  m_gain = gain;
}

void LArOFIterResults::setValid( bool valid ){
  m_valid = valid;
}

void LArOFIterResults::setConverged( bool converged ){
  m_converged = converged;
}

void LArOFIterResults::setNIterPerf( int nIterPerf ){
  m_nIterPerf = nIterPerf;
}

void LArOFIterResults::setAmplitude( float amplitude){
  m_amplitude = amplitude;
}

void LArOFIterResults::setTau( float tau ){
  m_tau = tau;
}

void LArOFIterResults::setPeakSample_init( unsigned peakSample_init ){
  m_peakSample_init = peakSample_init;
}

void LArOFIterResults::setPeakSample_final( unsigned peakSample_final ){
  m_peakSample_final = peakSample_final;
}

void LArOFIterResults::setDelay_final( float delay_final ){
  m_delay_final = delay_final; 
}

void LArOFIterResults::setQuality( float quality ){
  m_quality = quality;
}


void LArOFIterResults::setOFCIndex(unsigned idx) {
  m_ofcIndex = idx;
}
