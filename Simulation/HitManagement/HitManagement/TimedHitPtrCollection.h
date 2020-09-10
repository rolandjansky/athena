/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HITMANAGEMENT_TIMEDHITPTRCOLLECTION
#define HITMANAGEMENT_TIMEDHITPTRCOLLECTION

#include <functional>
#include <vector>
#include "HitManagement/AthenaHitsVector.h"
#include "HitManagement/TimedHitPtr.h"
#include "EventInfo/PileUpTimeEventIndex.h"

template <class HIT>
class TimedHitPtrCollection {
private:
  typedef std::vector< TimedHitPtr<HIT> > TimedVector;

public:
  typedef typename TimedVector::const_iterator const_iterator;

  TimedHitPtrCollection() : m_hits(),
			    m_currentHit(m_hits.begin()),
			    m_sorted(false) {}
  ///thrown by insert 
  class SortedException {};

  void 
  insert(const PileUpTimeEventIndex& timeEventIndex, const AthenaHitsVector<HIT>* inputCollection);
  ///@throws SortedException to show that iteration will restart from begin()

  void 
  insert(float evtTime, const AthenaHitsVector<HIT>* inputCollection);
  ///@throws SortedException to show that iteration will restart from begin()

  ///sets an iterator range with the hits of current detector element
  ///returns a bool when done
  bool nextDetectorElement(const_iterator& b, const_iterator& e);

  //FIXME elementID type std::pair<const_iterator, const_iterator> detectorElement(????) const;
  

private:
  void sortVector();
  TimedVector m_hits;
  const_iterator m_currentHit; ///< of current detector element;
  bool m_sorted; ///< flag the fact that the collection has been sorted
};

#include "TimedHitPtrCollection.icc"
#endif

