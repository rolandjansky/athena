/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __FeatureIndex_h__
#define __FeatureIndex_h__
/*
  FeatureIndex.h
*/
#include <vector>
#include <ostream>
#include <sstream>

class FeatureIndex {
public:
  template<class It>
  static void dump(It begin, It end, const std::string& prefix) {
    for (int i=0; begin!=end; ++begin, ++i) {
      std::ostringstream os;
      os << prefix << "[" << i << "] : ";
      begin->dump(os.str());
    }
  }
public:
  FeatureIndex();
  FeatureIndex(int index, int status);
  ~FeatureIndex();

  void set(int index, int status);
  void setIndex(int index);
  void setStatus(int status);

  int getIndex() const { return mIndex; }
  int getStatus() const { return mStatus; }

  bool isValid() const;
  bool sameIndex(const FeatureIndex& x) const;
  bool operator!=(const FeatureIndex& x) const;
  bool operator==(const FeatureIndex& x) const;

  void dump(const std::string& prefix) const;

private:
  int mIndex;        // index of feature in container
  int mStatus;       // if mStatus = 0 , is failed

};

std::ostream& operator<<(std::ostream& o, const FeatureIndex& x);
std::ostream& operator<<(std::ostream& o, const std::vector<FeatureIndex>& x);
bool sameIndex(const std::vector<FeatureIndex>& x, 
	       const std::vector<FeatureIndex>& y);


#endif // __FeatureIndex_h__
