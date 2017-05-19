/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUNTUPLE_COMBLINKS_H
#define TRIGGERMENUNTUPLE_COMBLINKS_H

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
  static std::map<int, std::string> s_FeatureIdMap;
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
  int lastStep() const { return m_lastStep; }
  bool active() const { return m_state; }
  int CombNumber() const { return m_combNumber; }
  int TENumber() const { return m_TENumber; }
  int RoINumber() const { return m_RoINumber; }
  int RoIType() const { return m_RoIType; }
  const FeatureIndex* index(const std::string& feature) const;
  const std::vector<FeatureIndex>* indexVec(const std::string& feature) const;
  const std::string& TELabelString() const { return m_TELabelString; }

  void setActiveState(bool isActive) { m_state = isActive; }
  void setLastStep(int i) { m_lastStep = i; }
  void setRoIType(int i) { m_RoIType = i; }
  void setCombNumber(int i) { m_combNumber = i; }
  void setTENumber(int i) { m_TENumber = i; }
  void setRoINumber(int i) { m_RoINumber = i; }
  void addIndex(const std::string& feature, const FeatureIndex& i);
  void addIndexVec(const std::string& feature, const FeatureIndexVec_t& iv);
  void setTELabelString(const std::string& x) { m_TELabelString = x; }
  void prependTELabel(const std::string& x);

  bool isValid() const;
  void dump() const;
  bool isSameRoI(const CombLinks& x) const;
  bool operator==(const CombLinks& x) const;

  const FeatureIndex* index(int feature_id) const;
  const std::vector<FeatureIndex>* indexVec(int feature_id) const;

private:
  int m_RoIType;
  int m_lastStep;
  bool m_state;
  int m_combNumber; // Number to identify different combinations
  int m_TENumber; // TE location in the configuration (for asymmetric combined chains)
  int m_RoINumber; // RoI number within the TE (for combo/AllTE algorithms)
  std::map<int, FeatureIndex> m_indexMap;
  std::map<int, FeatureIndexVec_t> m_indexVecMap;
  std::string m_TELabelString;
};

class order_by_TELabel {
public:
  bool operator()(const CombLinks& x, const CombLinks& y) const;
};

std::ostream& operator<<(std::ostream& o, const CombLinks& x);

#endif // TRIGGERMENUNTUPLE_COMBLINKS_H
