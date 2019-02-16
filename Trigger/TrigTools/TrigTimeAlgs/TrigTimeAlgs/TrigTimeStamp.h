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
 * AlgA::execute() { 
 *   timeStampHandle.record( std::move( std::make_unique<TrigTimeStamp>() ) ); 
 * }
 *
 * AlgB obtains the duration since the start tagged in AlgA:
 * AlgB::execute() { 
 *   double duration = timeStampHandle.cptr().millisecondsSince();
 * }
 **/
class TrigTimeStamp {
public:
  typedef std::chrono::high_resolution_clock::time_point stamp_type;
  
  /**
   * Default constructor, sets to now
   **/
  TrigTimeStamp();

  /**
   * Automatic copy assignemnt operator
   **/
  TrigTimeStamp& operator=(const TrigTimeStamp&) = default;  

  /**
   * Automatic copy constructor
   **/
  TrigTimeStamp(const TrigTimeStamp&) = default;

  /**
   * @return The time duration between now and when the time stamp was created
   **/
  double millisecondsSince() const; 

  /**
   * @return The time duration between when this and another time stamp were created, or last set
   * @param[in] other Time stamp to compare this one to
   **/
  double millisecondsDifference(const TrigTimeStamp& other) const; 

  /**
   * @return The time duration between when the time stamp was created and the epoch
   **/
  uint64_t microsecondsSinceEpoch() const;

  /**
   * Obtain the stamp value.
   **/
  stamp_type get() const;

  /**
   * Update the stamp value.
   * @param[in] time Time to set. Default is now.
   **/
  void set(stamp_type time = std::chrono::high_resolution_clock::now());

private:
  stamp_type m_stamp;

};

CLASS_DEF( TrigTimeStamp , 262409323 , 1 )

inline TrigTimeStamp::TrigTimeStamp() { set(); }

inline TrigTimeStamp::stamp_type TrigTimeStamp::get() const { return m_stamp; }

inline void TrigTimeStamp::set(TrigTimeStamp::stamp_type time) { m_stamp = time; }

#endif  // TrigTimeAlgs_TrigTimeStamp_h
