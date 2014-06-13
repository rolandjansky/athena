/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIPIXELTRACKLETSTRUTHKEY_H
#define HIPIXELTRACKLETSTRUTHKEY_H
#include "HIGlobal/HIPixelTrackletsCollection.h"
#include "DataModel/ElementLink.h"

class HIPixelTrackletsTruthKey{
 public:
  typedef HIPixelTrackletsCollection::size_type size_type;
  size_type index() const {return m_index;}
  HIPixelTrackletsTruthKey(size_type i) : m_index(i) {}
  HIPixelTrackletsTruthKey(const ElementLink<HIPixelTrackletsCollection>& el): m_index(el.index()){}
  HIPixelTrackletsTruthKey():m_index(-1U) {}
  
 private:
  size_type m_index;
};

inline bool operator<(const HIPixelTrackletsTruthKey& a, const HIPixelTrackletsTruthKey& b){
  return a.index() < b.index();
}

#endif
