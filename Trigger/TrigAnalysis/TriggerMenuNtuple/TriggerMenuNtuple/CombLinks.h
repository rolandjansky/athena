/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __CombLinks_h__
#define __CombLinks_h__
/*
  CombLinks.h
*/
#include "TriggerMenuNtuple/FeatureIndex.h"
#include <vector>
#include <string>
#include <map>

class CombLinks {
public:
  static int featureId(const std::string& feature);
  static std::string featureName(int feature_id);
  static int addFeature(const std::string& feature);
  static int addRoIType(const std::string& feature);
  static const std::map<int, std::string>& featureIdMap();
private:
  static std::map<int, std::string> sFeatureIdMap;
  friend std::ostream& operator<<(std::ostream& o, const CombLinks& x);

public:
  typedef std::vector<FeatureIndex> FeatureIndexVec_t;
  
public:
  CombLinks();
  CombLinks(int roi_type);
  ~CombLinks();

  bool hasFeature(const std::string& feature) const;
  std::vector<std::string> allFeatureNames() const;
  std::vector<std::string> FeatureNames() const;
  std::vector<std::string> FeatureVecNames() const;
  int lastStep() const { return mLastStep; }
  bool active() const { return mState; }
  int CombNumber() const { return mCombNumber; }
  int TENumber() const { return mTENumber; }
  int RoINumber() const { return mRoINumber; }
  int RoIType() const { return mRoIType; }
  const FeatureIndex* index(const std::string& feature) const;
  const std::vector<FeatureIndex>* indexVec(const std::string& feature) const;
  const std::string& TELabelString() const { return mTELabelString; }

  void setActiveState(bool isActive) { mState = isActive; }
  void setLastStep(int i) { mLastStep = i; }
  void setRoIType(int i) { mRoIType = i; }
  void setCombNumber(int i) { mCombNumber = i; }
  void setTENumber(int i) { mTENumber = i; }
  void setRoINumber(int i) { mRoINumber = i; }
  void addIndex(const std::string& feature, const FeatureIndex& i);
  void addIndexVec(const std::string& feature, const FeatureIndexVec_t& iv);
  void setTELabelString(const std::string& x) { mTELabelString = x; }
  void prependTELabel(const std::string& x);

  bool isValid() const;
  void dump() const;
  bool isSameRoI(const CombLinks& x) const;
  bool operator==(const CombLinks& x) const;

  const FeatureIndex* index(int feature_id) const;
  const std::vector<FeatureIndex>* indexVec(int feature_id) const;

private:
  int mRoIType;
  int mLastStep;
  bool mState;
  int mCombNumber; // Number to identify different combinations
  int mTENumber; // TE location in the configuration (for asymmetric combined chains)
  int mRoINumber; // RoI number within the TE (for combo/AllTE algorithms)
  std::map<int, FeatureIndex> mIndexMap;
  std::map<int, FeatureIndexVec_t> mIndexVecMap;
  std::string mTELabelString;
};

class order_by_TELabel {
public:
  bool operator()(const CombLinks& x, const CombLinks& y) const;
};

std::ostream& operator<<(std::ostream& o, const CombLinks& x);

#endif // __CombLinks_h__
