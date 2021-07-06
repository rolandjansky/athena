/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MONTECARLOREACTUTILS_EFFVAL_H
#define MONTECARLOREACTUTILS_EFFVAL_H

#include <math.h>
#include <stdexcept>

namespace MonteCarloReact {

// this is a class to save any value with errors in the specified bin in histogram 
// (not necessarily efficiency). For example, should be used directly by lumi reweighting
class Value {


 public:
  
  Value(float v=0, float hi=0, float lo=0);

  virtual ~Value();
  
  //  redefine this operator for the efficiency
  Value& operator += (const Value& eff);

  // following operators does not have a sence for the efficiency and hence will be prohibitted
  Value& Scale(float sc);

  const Value operator/ (const Value& right) const;

  const Value operator* (const Value& right) const;  
  
  float getValue() const {return m_value;}
  float getErrorHigh() const {return m_errorlow;}
  float getErrorLow() const {return m_errorhigh;}

  void setValue(float val) {m_value = val;}
  void setErrorHigh(float ehi) {m_errorhigh = ehi;}
  void setErrorLow(float elo) {m_errorlow = elo;}
  
  // reset value and efficiencies
  virtual void reset() {m_value = 0.0 ; m_errorhigh = 0; m_errorlow = 0; }
  
 protected:
  
  float m_value;
  float m_errorhigh;
  float m_errorlow;
};

// class to save the efficiency value 
// only number of events is saved. Efficiency is calculated when needed
class EffVal : public Value {

 public:

  EffVal( long unsigned int n_passed=0, long unsigned int n_ini=0); 

  ~EffVal();
  
  // add event, passed should be true if event passed selection or fired the trigger
  void fill(bool passed=true);
 
  // set number of events and recalculated efficiency
  void setEfficiency(long unsigned int n_passed, long unsigned int n_ini);

  // recalculate efficiency (stored as Value) using n and n_ini number of events 
  void recalculateEff();

  void reset();

  // Assignment from Value. Set only value number. Will invalidate the number of events 
  EffVal& operator= (const Value& v);
  
  EffVal& operator += (const EffVal& eff); 

  long unsigned int getTotalEvents() const {return m_totalEvents;}
  long unsigned int getPassedEvents() const {return m_passedEvents;}
  
  // prohibit scale operation for the efficiency
  EffVal& Scale(float /* sc */) {
    throw std::runtime_error("Scale operation is not defined for EffVal!");
  }

  // prohibit scale operator* for the efficiency
  const EffVal operator*(float /* sc */) {
    throw std::runtime_error("Operation * is not defined for EffVal!") ;
  }
  
  // prohibit scale operator/ for the efficiency
  const EffVal operator/(float /* sc */) {
    throw std::runtime_error("Operation * is not defined for EffVal!") ;
  }

 private:

  long unsigned int m_passedEvents;
  long unsigned int m_totalEvents ;
  
 };

}
#endif
