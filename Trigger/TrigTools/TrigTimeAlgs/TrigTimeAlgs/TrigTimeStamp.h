/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigTimeAlgs_TrigTimeStamp_h
#define TrigTimeAlgs_TrigTimeStamp_h


#include <chrono>
#include "SGTools/CLASS_DEF.h"
/**
 * utility class to measure time duration in AthenaMT
 * The pattern when it is useful: 
 * AlgA tags the beginning of the time period
 *AlgA::execute() { 
 * timeStampHandle.record( std::move( std::make_unique<TrigTimeStamp>() ) ); }
 *
 * AlgB obtains the duration since the start tagged in AlgA:
 * AlgB::execute() { 
 *  double duration = timeStampHandle.cptr().milisecondsSince();  }
 **/
class TrigTimeStamp {
public:
  typedef std::chrono::high_resolution_clock::time_point stamp_type;

  TrigTimeStamp()
    : m_stamp( std::chrono::high_resolution_clock::now() ) {}

  /**
   * time duration between now and when the time stamp was created
   **/
  double millisecondsSince() const; 

  /**
   * Obtain the stamp value.
   **/
  stamp_type get() const { return m_stamp; }
private:
  stamp_type m_stamp;
};



CLASS_DEF( TrigTimeStamp , 262409323 , 1 )

#endif  // TrigTimeAlgs_TrigTimeStamp_h

