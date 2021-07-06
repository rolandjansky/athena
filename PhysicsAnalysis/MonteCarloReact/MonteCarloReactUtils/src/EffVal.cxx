/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MonteCarloReactUtils/EffVal.h"

using namespace MonteCarloReact;
using namespace std;

Value::Value(float v, float hi, float lo) : 
  m_value(v), m_errorhigh(hi), m_errorlow(lo) {;}
  
Value::~Value() {;}

//  redefine this operator for the efficiency
Value& Value::operator += (const Value& eff) {
  m_value += eff.getValue() ;
  m_errorhigh = sqrt(m_errorhigh * m_errorhigh + eff.getErrorHigh() * eff.getErrorHigh()) ;
  m_errorlow = sqrt(m_errorlow * m_errorlow + eff.getErrorLow() * eff.getErrorLow()) ;
  return *this ;
}

// following operators does not have a sence for the efficiency and hence will be prohibitted
Value& Value::Scale(float sc) {
  m_value *= sc ;
  m_errorhigh *= sc ;
  m_errorlow *=sc ;
  return *this ;
}

const Value Value::operator/ (const Value& right) const {
  float v = right.getValue() == 0 ? NAN : m_value/right.getValue() ;
  float hi = right.getValue() == 0 ? NAN :
    sqrt(m_errorhigh * m_errorhigh + v * v * right.getErrorHigh() * right.getErrorHigh())/fabs(right.getValue()) ;
  float lo = right.getValue() == 0 ? NAN :
    sqrt(m_errorlow * m_errorlow + v * v * right.getErrorLow() * right.getErrorLow())/fabs(right.getValue()) ;
  return Value(v,hi,lo) ;
}

const Value Value::operator* (const Value& right) const {
  float v = m_value * right.getValue() ;
  float hi = sqrt(right.getValue() * m_errorhigh * right.getValue() * m_errorhigh +
                  m_value * right.getErrorHigh() * m_value *right.getErrorHigh()) ;
  float lo = sqrt(right.getValue() * m_errorlow * right.getValue() * m_errorlow +
                  m_value * right.getErrorLow() * m_value * right.getErrorLow()) ;
  return Value(v,hi,lo) ;
}


/**********************************************************************/

EffVal::EffVal( long unsigned int n_passed, long unsigned int n_ini) : 
  Value(), m_passedEvents(n_passed), m_totalEvents(n_ini) { 
  if (n_ini > 0) recalculateEff() ;
}

EffVal::~EffVal() {;}

// add event, passed should be true if event passed selection or fired the trigger
void EffVal::fill(bool passed) {
  m_totalEvents++;
  if (passed) m_passedEvents++ ;
   recalculateEff() ;
}

// set number of events and recalculated efficiency
void EffVal::setEfficiency(long unsigned int n_passed, long unsigned int n_ini) {
  m_passedEvents = n_passed ;
  m_totalEvents = n_ini ;
  recalculateEff() ;
}

// recalculate efficiency (stored as Value) using n and n_ini number of events 
void EffVal::recalculateEff() {
  if (m_totalEvents <=0)  return ;
  m_value = ((float) m_passedEvents) / m_totalEvents ;
  m_errorhigh = sqrt(m_value * (1.0 - m_value) / m_totalEvents) ;
  m_errorlow = m_errorhigh ;
}

void EffVal::reset() {
  m_passedEvents = 0 ;
  m_totalEvents = 0;
  ((Value*)this)->reset() ;
}

// Assignment from Value. Set only value number. Will invalidate the number of events 
EffVal& EffVal::operator= (const Value& v) {
  m_passedEvents = 0 ; 
  m_totalEvents = 0 ;
  m_value = v.getValue() ;  
  m_errorhigh = v.getErrorHigh(); 
  m_errorlow = v.getErrorLow();
  return *this ;
}


EffVal& EffVal::operator += (const EffVal& eff) {
  m_passedEvents += eff.getPassedEvents() ;
  m_totalEvents += eff.getTotalEvents() ;
  recalculateEff() ;
  return *this ;
}

