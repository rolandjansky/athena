/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LArRawConditions/LArWaveCumul.h"
#include <math.h>


// LArWaveCumul:: LArWaveCumul(const std::vector<double>& theVector,double dt, HWIdentifier channelID) :
//   LArWave(theVector,dt), m_channelID(channelID)
// {
//   unsigned nSamples = theVector.size() ;
//   m_errors.resize(nSamples,0) ; 
//   m_triggers.resize(nSamples,0) ;
// }

// LArWaveCumul::LArWaveCumul(const std::vector<double>& vAmpl, const std::vector<double>& vErr, const std::vector<int>& vTrig ,
// 			   const double dt, const HWIdentifier channelID) : LArWave(vAmpl,dt), m_channelID(channelID),
// 										  m_errors(vErr),m_triggers(vTrig)		       {
//   unsigned int nSamples = vAmpl.size() ;
// }


// LArWaveCumul:: LArWaveCumul(const unsigned nSamples,const double dt, const HWIdentifier channelID) :
//   LArWave(nSamples,dt), m_channelID(channelID)
// {
//   m_errors.resize(nSamples,0) ; 
//   m_triggers.resize(nSamples,0) ;
// }


void LArWaveCumul::addEvent( int delay , int step ,
			     const std::vector<double>& Samples )
{
  const unsigned int nSwave = getSize() ;
  const unsigned int nSevt  = Samples.size() ;
  for ( unsigned int i=0 ; i<nSevt ; i++ ) {
    //unsigned int k = delay + i*step ;
    unsigned int k=(i+1)*step-(delay+1);
    if (k<nSwave) {
      double evt  = Samples[i] ;
      int    N    = m_triggers[k] ;
      double amp2 = m_amplitudes[k]*m_amplitudes[k];
      //double sum2 = N * ( m_errors[k]*m_errors[k] + amp2 );
      double sum2 = (N-1)*m_errors[k]*m_errors[k] + N*amp2;

      double sum  = m_amplitudes[k] * N ;
      m_amplitudes[k] = ( sum + evt ) / (N+1) ;
      //double arg = ( sum2 + evt*evt ) / (N+1) - m_amplitudes[k]*m_amplitudes[k] ;
      double arg = ( ( sum2 + evt*evt ) - (N+1)*m_amplitudes[k]*m_amplitudes[k] )/N ;

      // Can happen due to underflow if m_errors is zero and evt is
      // exactly the same as m_amplitudes.
      if (arg < 0) arg = 0;
      m_errors[k]     = sqrt(arg);
      m_triggers[k]   = N + 1 ;

    }
  }
}



void LArWaveCumul::addAccumulatedEvent( int delay , int step ,
			                const std::vector<double>& SamplesSum ,
			                const std::vector<double>& Samples2Sum , 
			                unsigned nTriggers )
{
  const unsigned int nSwave = getSize() ;
  const unsigned int nSevt  = SamplesSum.size() ;
  
  for ( unsigned int i=0 ; i<nSevt ; i++ ) {

    unsigned int k=(i+1)*step-(delay+1);
    
    if (k<nSwave ) {

      int    N1    = m_triggers[k] ;
      double sum1  = m_amplitudes[k] * N1 ;
      //double sum12 = N1 * ( m_errors[k]*m_errors[k] + m_amplitudes[k]*m_amplitudes[k] ) ;
      double sum12 = (N1-1)*m_errors[k]*m_errors[k] + N1*m_amplitudes[k]*m_amplitudes[k] ;

      int    N2    = nTriggers ;
      double sum2  = SamplesSum[i] ;
      double sum22 = Samples2Sum[i] ;
      
      m_triggers[k]   = N1+N2 ;
      m_amplitudes[k] = (sum1+sum2)/m_triggers[k] ;      
      //m_errors[k]    = sqrt( (sum12+sum22)/m_triggers[k] - m_amplitudes[k]*m_amplitudes[k] ) ;
      m_errors[k]     = sqrt( ( sum12+sum22-m_amplitudes[k]*m_amplitudes[k]*m_triggers[k] )/(m_triggers[k]-1) ) ;

    }
  
  }

}

// std::vector<double> LArWaveCumul::getErrors() const 
// { return m_errors ; }

// double LArWaveCumul::getError(unsigned int i) const
// { return (i<getSize()) ? m_errors[i] : -9.9999E+99 ; }

// std::vector<int> LArWaveCumul::getTriggers() const 
// { return m_triggers ; }

// int LArWaveCumul::getTrigger(unsigned int i) const
// { return (i<getSize()) ? m_triggers[i] : -99999999 ; }

