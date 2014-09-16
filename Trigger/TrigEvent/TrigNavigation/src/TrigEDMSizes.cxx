/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigNavigation/TrigEDMSizes.h"

#include <boost/foreach.hpp>

using namespace HLT;
TrigEDMSizes::TrigEDMSizes(){}


TrigEDMSizes::EDMObjectInfo::EDMObjectInfo(const std::string& coll, const std::string& l, unsigned w, unsigned cut) 
  : collection(coll), label(l), words(w), cutsize(cut)
{}

bool TrigEDMSizes::EDMObjectInfo::isTruncated(unsigned size_after_serialization) const {
  if (cutsize >  size_after_serialization)
    return true;
  return false;
}

void TrigEDMSizes::addObject(const std::string& coll, const std::string& label, unsigned words, unsigned cutsize) {
  m_objects.push_back(TrigEDMSizes::EDMObjectInfo(coll, label, words, cutsize));
}

const TrigEDMSizes::EDMEventInfo& TrigEDMSizes::info() const {
  return m_objects;
}

const TrigEDMSizes::EDMObjectInfo& TrigEDMSizes::lastNotTruncated(unsigned size_after_truncation) const {
  bool foundTruncated = false;
  bool foundNotTruncated = false;
  
  BOOST_REVERSE_FOREACH( const TrigEDMSizes::EDMObjectInfo& oi, m_objects) {    
    if ( foundTruncated && foundNotTruncated )
      return oi; 

    if (  oi.isTruncated(size_after_truncation) ) {
      foundTruncated = true;
    } else {
      foundNotTruncated = true;
    }
  }
  return m_objects.front();
}
