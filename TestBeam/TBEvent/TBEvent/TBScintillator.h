/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBEVENT_TBSCINTILLATOR_H
#define TBEVENT_TBSCINTILLATOR_H
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Templated class for represenetation of scintillator signals (pulse        //
// heights). Typical implementation:                                         //
//                                                                           //
// TBScintillator<unsigned short> theRawSignal;  // raw signal               //
// TBScintillator<double> theRecoSignal;         // reconstructed signal     //
//  Modified: Jan 29th 2004 by Malachi Schram                                //
///////////////////////////////////////////////////////////////////////////////


#include "TBEvent/TBBeamDetector.h"

#include "AthenaKernel/CLASS_DEF.h"



class TBScintillator : public TBBeamDetector
{ 
 public:

  /////////////////////////////////
  // Constructors and Destructor //
  /////////////////////////////////

  TBScintillator() : TBBeamDetector()
    , m_signal(0)
    , m_time_signal(0)
    , m_signal_overflow(false)
    , m_time_overflow(false)
    { }

  TBScintillator(const std::string& scintillatorName) : TBBeamDetector(scintillatorName,false)
    , m_signal(0)
    , m_time_signal(0)
    , m_signal_overflow(false)
    , m_time_overflow(false)
    { }

  

  TBScintillator(const std::string& scintillatorName,
		   bool overflow,
		   float theSignal)
    : TBBeamDetector(scintillatorName,  overflow)
    , m_signal(theSignal)
    , m_time_signal(0)
    , m_signal_overflow(false)
    , m_time_overflow(false)
    { }

  virtual ~TBScintillator()
    { }

  ///////////////////////
  // Data Manipulation //
  ///////////////////////

  virtual void setSignal(float theSignal)
    { m_signal = theSignal; }
  virtual float getSignal() const
    { return m_signal; }
  virtual void setSignalOverflow(bool signal_overflow)
    { m_signal_overflow = signal_overflow; }
  virtual bool isSignalOverflow() const
    { return m_signal_overflow; }


 virtual void setTimeSignal(float theSignal)
    { m_time_signal = theSignal; }
  virtual float getTimeSignal() const
    { return m_time_signal; }
  virtual void setTimeOverflow(bool time_overflow)
    { m_time_overflow = time_overflow; }
  virtual bool isTimeOverflow() const
    { return m_time_overflow; }


  //////////
  // Data //
  //////////

 protected:

  float m_signal, m_time_signal;
  bool m_signal_overflow, m_time_overflow;
 
};




CLASS_DEF( TBScintillator , 132535898 , 1 )

#endif
