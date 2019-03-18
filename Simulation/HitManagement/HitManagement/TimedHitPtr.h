/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HITMANAGEMENT_TIMEDHITPTR
#define HITMANAGEMENT_TIMEDHITPTR

/** @class TimedHitPtr
 *  @brief a smart pointer to a hit that also provides access to the
 *  extended timing info of the host event. For a pu event this is
 *  taken from the event PileUpTimeEventIndex usually accessed via
 *  PileUpMergeSvc::TimedList
 **/

template <class HIT>
class TimedHitPtr {
public:
  ///STL required constructors
  TimedHitPtr() : m_eventTime(0), m_eventId(0), m_pileupType(0), m_pHit(nullptr) {}
  TimedHitPtr(const TimedHitPtr<HIT>& rhs) :
    m_eventTime(rhs.m_eventTime), m_eventId(rhs.m_eventId), m_pileupType(rhs.m_pileupType), m_pHit(rhs.m_pHit)  {}

  ///minimal constructor: pass only t0 offset of bunch xing
  TimedHitPtr(float eventTime, const HIT* pHit, int pileupType=0) :
    m_eventTime(eventTime), m_eventId(0), m_pileupType(pileupType), m_pHit(pHit) {}
  ///use this constructor when hit has a PileUpTimeEventIndex
  TimedHitPtr(float eventTime, unsigned short eventId, const HIT* pHit, int pileupType=0) :
    m_eventTime(eventTime), m_eventId(eventId), m_pileupType(pileupType), m_pHit(pHit) {}

  ///assignment operator
  TimedHitPtr<HIT>& operator=(const TimedHitPtr<HIT>& rhs) {
    if(&rhs != this) { m_eventTime = rhs.m_eventTime; m_eventId = rhs.m_eventId; m_pileupType = rhs.m_pileupType; m_pHit = rhs.m_pHit; }
    return *this;
  }

  ///smart pointer interface
  const HIT& operator*() const   { return *m_pHit; }
  const HIT* operator->() const  { return m_pHit; }

  ///the index of the component event in PileUpEventInfo. Allows, in principle,
  ///to navigate back to the parent event
  unsigned short eventId() const { return m_eventId; }

  ///the type of event which the hit came from (signal, low pt
  ///minbias, high pt minbias, cavern bg, etc.)
  int pileupType() const { return m_pileupType; }

  /// t0 offset of the bunch xing containing the hit in ns. This is an integer
  /// multiple of the bunch xing distance for a certain event (e.g. N*25ns)
  float eventTime() const { return m_eventTime; }

private:

  ///t0 offset of the bunch xing in ns
  float m_eventTime;
  ///the index in PileUpEventInfo of the component event hosting this hit
  unsigned short m_eventId;
  int m_pileupType;
  const HIT* m_pHit; //don't own

  template <class FHIT>
  friend float hitTime(const TimedHitPtr<FHIT>&);
};

template <class HIT>
inline float hitTime(const TimedHitPtr<HIT>& tHit) {
  return tHit.m_eventTime + hitTime(*tHit);
}



template <class HIT>
bool operator < (const TimedHitPtr<HIT>& lhs, const TimedHitPtr<HIT>& rhs ) {
  return ( 0 != lhs.operator->() && 0 != rhs.operator->() && *lhs < *rhs);
}

#endif
