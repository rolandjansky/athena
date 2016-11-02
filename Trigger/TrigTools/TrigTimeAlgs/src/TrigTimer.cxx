// this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigTimer.cxx,v 1.12 2009-03-12 07:46:45 tbold Exp $
#include <sys/time.h>
#include "TrigTimeAlgs/TrigTimer.h"
#include "CxxUtils/AthUnlikelyMacros.h"

#define SecsInDay 86400

using namespace std;

TrigTimer::TrigTimer (const std::string name, bool active)
  : m_name (name),
    m_active(active),
    m_elapsed(0.0),
    m_lastElapsed(0.0),
    m_mean(0.0),
    m_ms(0.0),
    m_numberOfMeasurements(0),
    m_started(false),
    m_paused(false),
    m_propVal(0),
    m_meanVal(0),
    m_meanTimePerObject(0.0),
    m_NmeanTimePerObject(0) {
  m_startTime.tv_sec = 0;
  m_startTime.tv_usec = 0;
  m_stopTime.tv_sec = 0;
  m_stopTime.tv_usec = 0;
}

TrigTimer::~TrigTimer() {}

TrigTimer& TrigTimer::operator=(const TrigTimer& x) {
  m_startTime = x.m_startTime;
  m_startTime = x.m_startTime;
  m_started   = x.m_started;             
  m_stopTime  = x.m_stopTime; 
  m_active    = x.m_active;   

  m_elapsed   = x.m_elapsed;            
  m_lastElapsed  = x.m_lastElapsed;        
  m_mean      = x.m_mean;              
  m_ms        = x.m_ms;
                
  m_numberOfMeasurements  = x.m_numberOfMeasurements;    
  m_started   = x.m_started;             
  m_paused    = x.m_paused;              
  
  m_propVal   = x.m_propVal;     
  m_meanVal   = x.m_meanVal;              
  m_meanTimePerObject   = x.m_meanTimePerObject;    
  m_NmeanTimePerObject  = x.m_NmeanTimePerObject;
    
  return *this;
}
    
void TrigTimer::start( void ) {
  if ( !m_active )
	return;
  m_started = true;
  m_paused = false;
  m_elapsed = 0.0;
  m_propVal = 0;
  // if already started, the start time will be over-written
  gettimeofday(&m_startTime, 0);
}
    
void TrigTimer::stop ( void ) {
  if ( !m_active || !m_started) return;

  gettimeofday(&m_stopTime, 0);
  m_started = false;

  if (!m_paused) {
	int secs = m_stopTime.tv_sec - m_startTime.tv_sec;
	int usecs = m_stopTime.tv_usec - m_startTime.tv_usec;

	if (ATH_UNLIKELY(secs<0)) { // we have turned into a pumpkin 
      secs = m_stopTime.tv_sec - m_startTime.tv_sec + SecsInDay;
	}
	float mtime = static_cast<float>(secs)*1000. + static_cast<float>(usecs)*1e-3;
	
	// elapsed time
	m_elapsed += mtime;
	m_lastElapsed = mtime;
  }

  // statistics - mean, rms
  const double inv_denom = 1. / static_cast<double>(++m_numberOfMeasurements);
  m_mean += (m_elapsed - m_mean) * inv_denom;
  m_ms += (m_elapsed*m_elapsed - m_ms) * inv_denom;

  // mean property 
  m_meanVal += static_cast<double>(m_propVal - m_meanVal) * inv_denom;
  if (m_propVal !=0) {
	double timePerObject = m_elapsed/static_cast<double>(m_propVal);
	m_meanTimePerObject += static_cast<double>(timePerObject - m_meanTimePerObject) / 
      static_cast<double>(++m_NmeanTimePerObject);
  }
}

bool TrigTimer::running()  const {
  if ( m_started  && ! m_paused )
    return true;
  return false;
}

bool  TrigTimer::wasRun() const {
  if ( m_startTime.tv_sec  == 0 && m_startTime.tv_usec == 0 )
    return false;
  return true;
}
    
void TrigTimer::pause ( void ) {
  if ( !m_active )
	return;

  // only pause if started, a pause while paused is ignored 
  if (m_paused || !m_started) return;
  m_paused = true;
  gettimeofday(&m_stopTime, 0);

  int secs = m_stopTime.tv_sec - m_startTime.tv_sec;
  int usecs = m_stopTime.tv_usec - m_startTime.tv_usec;
      
  if (ATH_UNLIKELY(secs<0)) { // we have turned into a pumpkin 
	secs = m_stopTime.tv_sec - m_startTime.tv_sec + SecsInDay;
  }

  float mtime = static_cast<float>(secs)*1000. + static_cast<float>(usecs)*1e-3;

  m_elapsed += mtime; // time so far
  m_lastElapsed = mtime;
}

void TrigTimer::resume( void ) {
  if ( !m_active )
	return;
  if (!m_started) { 
	start(); // resume acts as start if not started
  } else if (m_paused) {
	m_paused = false;
	gettimeofday(&m_startTime, 0);
  }
}

void TrigTimer::reset () { 
  if ( !m_active )
	return;
  m_started = false;
  m_paused = false;
  m_elapsed = 0.0; 
  m_lastElapsed = 0.0;
  m_propVal = 0;
  m_startTime.tv_sec = 0;
  m_startTime.tv_usec = 0;
  m_stopTime.tv_sec = 0;
  m_stopTime.tv_usec = 0;
}



ScopeTimer::ScopeTimer(TrigTimer* t )
  : m_timer(t) {
  if ( m_timer) m_timer->start();
}


ScopeTimer::~ScopeTimer() {
  if ( m_timer ) m_timer->stop();
}


ScopeResumePauseTimer::ScopeResumePauseTimer(TrigTimer* t, bool isLastMeasurement )
  : m_timer(t), m_last(isLastMeasurement) {
  if ( m_timer) m_timer->resume();
}

ScopeResumePauseTimer::~ScopeResumePauseTimer() {
  if ( m_timer ) {
    if ( m_last ) {
      m_timer->stop();
    } else {
      m_timer->pause();
    }
  }
}


