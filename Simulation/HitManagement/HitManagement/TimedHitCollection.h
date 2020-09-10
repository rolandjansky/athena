/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HITMANAGEMENT_TIMEDHITCOLLECTION
#define HITMANAGEMENT_TIMEDHITCOLLECTION

#include <functional>
#include <vector>
#include "HitManagement/AtlasHitsVector.h"
#include "HitManagement/TimedHitPtr.h"
#include "EventInfo/PileUpTimeEventIndex.h"

template <class HIT>
class TimedHitCollection {
private:
  typedef std::vector< TimedHitPtr<HIT> > TimedVector;

public:
  typedef typename TimedVector::const_iterator const_iterator;

  TimedHitCollection() : m_hits(),
			 m_currentHit(m_hits.begin()),
			 m_sorted(false) {}

  TimedHitCollection(unsigned int numberOfHits): m_hits(),
    m_currentHit(m_hits.begin()),
    m_sorted(false) {
  m_hits.reserve(numberOfHits); 
  return;
}
 
  ///thrown by insert 
  class SortedException {};

  void 
  insert(const PileUpTimeEventIndex& timeEventIndex, const AtlasHitsVector<HIT>* inputCollection);
  ///@throws SortedException to show that iteration will restart from begin()

  void 
  insert(float evtTime, const AtlasHitsVector<HIT>* inputCollection);
  ///@throws SortedException to show that iteration will restart from begin()

  ///reserve a timed vector numberOfHits in size.
  void reserve(unsigned int numberOfHits);

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

#include "TimedHitCollection.icc"
#endif

