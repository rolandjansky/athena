// this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigTimer.h,v 1.16 2008-12-08 13:12:58 tbold Exp $
#ifndef TrigTIMER_H_
#define TrigTIMER_H_
#include <sys/time.h>
#include <iostream>
#include <string>
#include <cmath>
#include "DataModel/DataVector.h"
#include "SGTools/CLASS_DEF.h"

/**
 * @class TrigTimer  
 * @brief Timer class used to measure time intervals in Trigger code.
 * TrigTimer allows for base operations @see start @see stop @see pause @see resume etc.
 * Each timer has @see name and integer property which can be used to store some counts 
 * which can indicate on which the time was spent. 
 * The timer can be made inactive ... which means that all the methods do not get to system time measurements.
 * The timers are managed by TrigTimerSvc.
 *
 * @warning The gettimeofday system function is used to acquire current time so there is number of limitations:
 *   - # overhead - one should not use this timers in tight loops, they will be slowed down considerably 
 *   - # inaccuracy - one should not try to measure very short time intervals

 *
 */
class TrigTimer {
public:
  /**
   * @brief the constructor
   * @param name of the timer
   * @param activity state (if false all calls like start and stop do nothing)
   */
  TrigTimer (const std::string name, bool active=true );
  ~TrigTimer();

  /**
   * @brief the copy constructor
   * Copies state of the timer. It can (and should) be used if there is several timers to be started in the same time.
   * In this case one timer needs to be started and other just copy it's state using this constructor i.e. timea.start(); timeb=timea; timec=timea;.
   * @param t other timer
   */
  TrigTimer& operator=(const TrigTimer& t); //!< copy state of the timer

  inline const std::string& name() const { return m_name; }   //!<  retrieve timer name
  inline float elapsed( void ) const { return (m_started? 0.0 : m_elapsed);}           //!<  retrieve elapsed time
  inline float lastElapsed( void ) const { return (m_started? 0.0 : m_lastElapsed);}   //!<  retrieve last elapsed time (if pause/resume were used)
  bool  running() const; //!< returns true if times was started but is not paused or stopped
  bool  wasRun() const;  //!< returns true if timer was stared and stopped(or paused) since reset
  
  inline double mean( void ) const { return m_mean;} //!< mean value of all time intervals measured

  inline double rms( void ) const {                  //!< rms of time intervals
    double var = m_ms - m_mean*m_mean;
    return (var > 0 ? std::sqrt(var) : 0.0);
  }

  inline int number_of_measurements( void ) const { return m_numberOfMeasurements;} //!<  retrieve number of measurements 

  inline void propName(const std::string &name) { m_propName = name; }   //!< set  property name
  inline const std::string & propName() const { return m_propName; }     //!< retrieve property name

  inline void propVal(unsigned int val) { m_propVal = val; }     //!<  set property value
  inline unsigned int propVal() const { return m_propVal;}             //!<  retrieve property value


  inline double meanPropVal() const { return m_meanVal;}                 //!< mean value of stored properties
  inline double meanTimePerObject() const { return m_meanTimePerObject;} //!< mean time per object, mean of time interval divided by property value

  bool operator < ( TrigTimer &it) const { return (this->name() < it.name());} //!< less than operator based on name (needed to store timers in STL containers)

  // methods to control clock
  void start( void );    
  void stop ( void ); 
  void pause ( void );
  void resume( void );
  void reset ();
  
  
  inline struct timeval getStartTime(void) const {return m_startTime;} //!< absolute time of start
  inline struct timeval getStopTime(void) const {return m_stopTime;}   //!< absolute time of stop
  inline bool isActive() const {return m_active;}                      //!< true if timer is active

private:
  std::string m_name;           //!< the timer name 
  struct timeval m_startTime;   //!< timer start time
  struct timeval m_stopTime;    //!< and timer stop time
  bool m_active;                //!< activation flag, if false start/stop/pause/resume are fake

  float m_elapsed;              //!< 
  float m_lastElapsed;          //!< elapsed in last start/stop pause/resume 
  double m_mean;                //!< mean time
  double m_ms;                  //!< mean squared time
  int m_numberOfMeasurements;    
  bool m_started;               //!< true if timer already started
  bool m_paused;                //!< true if paused  
  std::string m_propName;       //!< property name
  unsigned int m_propVal;       //!< property value 
  double m_meanVal;             //!< mean value of time 
  double m_meanTimePerObject;   //!< mean value of time per object (time/property) 
  int  m_NmeanTimePerObject;    //!< number of times the above was calculated
};

/**
 * @brief this class defines timer which can be used 
 * to measure all what happens in given scope
 * The ScopeTimer objects has to be instantiated with the ptr to TrigTimer
 * in some place and the timer pointe dto wil be started. 
 * It will be automatcally stopped when it gets out of scope.
 * TrigTimer *tptr = timersvc->getItem("timerblah"); 
 * { <---- scop start
 *  ScopeTimer(tpr);
 *   //do stuff  ... lots of conditional code
 * } //<---- timer will be stoped here or any other place the scope is left
 *
 */
class ScopeTimer {
public:
  ScopeTimer(TrigTimer* );
  ~ScopeTimer();
private:
  ScopeTimer() {}
  TrigTimer* m_timer;
};


class ScopeResumePauseTimer {
public:
  ScopeResumePauseTimer(TrigTimer*,  bool isLastMeasurement );
  ~ScopeResumePauseTimer();
private:
  ScopeResumePauseTimer() {}
  TrigTimer* m_timer;
  bool m_last;
};

typedef DataVector<TrigTimer> TrigTimerContainer;
CLASS_DEF(TrigTimer, 217899216, 1)
CLASS_DEF(TrigTimerContainer, 1082445600, 1)

#endif // TrigTIMER_H








