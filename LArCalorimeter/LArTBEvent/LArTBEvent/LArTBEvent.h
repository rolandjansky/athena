/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTBEVENT_LARTBEVENT_H
#define LARTBEVENT_LARTBEVENT_H


/** @class LArTBEvent LArTBEvent.h "LArTBEvent/LArTBEvent.h"

    @author A. Abdesselam and P. Ghez
*/
#include "AthContainers/DataVector.h"
#include "SGTools/CLASS_DEF.h"


class LArTBEvent {
 public:
  
  /** Constructor : LArTBEvent is identified by eventNumber */
  LArTBEvent(int eventNumber);
  
  /** Standard Constructor */
  LArTBEvent(int eventNumber, int burstNumber, int random, int tdcTime, 
	     int TDC_min, int tdcAntiTime, float time, int hmas_data,
	     int eventNumberInROD, int eventNumberInMiniROD);
  
  /** Constructor for Iterator */
  LArTBEvent(const LArTBEvent& larTBEvent);

  /** Destructor */
  ~LArTBEvent();
  
  /**  Overloaded operator \& */
  LArTBEvent& operator = ( const LArTBEvent& larTBEvent);

  /**  Overloaded operators == */
  bool operator == (const LArTBEvent & larTBEvent) const;
  
  /* ------------------------------------------------------------  */
  /* ------------------------ G E T T E R ------------------------ */
  /* ------------------------------------------------------------  */  

  /** Get event number */
  inline int getEventNumber() const { return m_eventNumber; }
  
  /** Get burst number */
  inline int getBurstNumber() const { return m_burstNumber; }
  
  /** Get random flag */
  inline int getRandom() const { return m_random; }
  
  /** Get TDC time in tdc counts */
  inline int getTdcTime() const { return m_tdcTime; }

  /** Get TDC time in tdc counts */
  inline int getTdcMin() const { return m_TDC_min; }

  /** Get TDC anti Clock time in tdc counts */
  inline int getAntiTdcTime() const { return m_tdcAntiTime; }
  
  /** Get TDC time in nano seconds */
  inline float getTime() const { return m_time; }
  
  /** Get the data recorded time ?? (hours + minutes + seconds) */
  inline int getHmsData() const { return m_hms_data; }
  
  /** Get the event number recorded in ROD data */
  inline int getEventNumberInROD() const { return m_eventNumberInROD; }
  
  /** Get the event number recorded in miniROD data */
  inline int getEventNumberInMiniROD() 
    const { return m_eventNumberInMiniROD; }
  
 private:
  LArTBEvent();
  
  int m_eventNumber; /**<  evnet number */
  int m_burstNumber; /**<  burst number read in RD13 data (range 1:xx) */
  int m_random;      /**<  =1:random =0:physics */
  int m_tdcTime;     /**<  phase L1/40Mhz clock (TDC counts) */
  int m_TDC_min;     /**<  for conversion to time */
  int m_tdcAntiTime; /**<  phase L1/40Mhz anti-clock (TDC counts) */
  float m_time;      /**<  phase L1/40Mhz clock (nanosecondes) */
  int m_hms_data;    /**<  hour minute seconds  from the data */
  int m_eventNumberInROD; /**<  evnet number in HEC-ROD data */
  int m_eventNumberInMiniROD; /**<  evnet number in EMEC-miniROD data */
};
CLASS_DEF(LArTBEvent,2784,0)
#endif // LARTBEVENT_LARTBEVENT_H
