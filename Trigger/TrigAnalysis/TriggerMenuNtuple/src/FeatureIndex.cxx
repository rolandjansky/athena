/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  FeatureIndex.cxx
*/
#include "TriggerMenuNtuple/FeatureIndex.h"
#include <iostream>

using namespace std;


FeatureIndex::FeatureIndex() : 
  mIndex(-1), mStatus(0) {
}

FeatureIndex::FeatureIndex(int index, int status) : 
  mIndex(index), mStatus(status) {
}

FeatureIndex::~FeatureIndex() {
}

void FeatureIndex::set(int index, int status){
  mIndex = index;  mStatus = status;
}
void FeatureIndex::setIndex(int index){
  mIndex = index;
}
void FeatureIndex::setStatus(int status){
  mStatus = status;
}

bool FeatureIndex::isValid() const {
  if (mIndex >= 0) return true;
  else return false;
}

bool FeatureIndex::sameIndex(const FeatureIndex& x) const {
  if (mIndex == x.mIndex) {
    return true;
  } else {
    return false;
  }
}

bool FeatureIndex::operator!=(const FeatureIndex& x) const {
  if (mIndex != x.mIndex || mStatus != x.mStatus) {
    return true;
  } else {
    return false;
  }
}

bool FeatureIndex::operator==(const FeatureIndex& x) const {
  if (mIndex == x.mIndex && mStatus == x.mStatus) {
    return true;
  } else {
    return false;
  }
}

std::ostream& operator<<(std::ostream& o, const FeatureIndex& x) {
  o << x.getIndex() << "(" << x.getStatus() << ")";
  return o;
}

std::ostream& operator<<(std::ostream& o, const std::vector<FeatureIndex>& x) {
  std::vector<FeatureIndex>::const_iterator p;
  o << "[ ";
  for (p=x.begin(); p!=x.end(); ++p) {
    o << (*p) << ", ";
  }
  o << "]";
  return o;
}

void FeatureIndex::dump(const std::string& prefix) const {
  cout << prefix << " : " << (*this) << endl;
}

bool sameIndex(const std::vector<FeatureIndex>& x, 
	       const std::vector<FeatureIndex>& y) {
  bool status = true;
  if (x.size() != y.size()) {
    status = false;
  } else {
    unsigned int i=0, j=0;
    unsigned int n = x.size();
    for (i=0; i<n; ++i) {
      bool foundit=false;
      for (j=0; j<n; ++j) {
	if (x[i].sameIndex(y[j])) {
	  foundit = true;
	  break;
	}
      }
      if (!foundit) {
	status = false;
	break;
      }
    }
  }
  return status;
}
