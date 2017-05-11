/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  CombLinks.cxx
*/
#include "TriggerMenuNtuple/CombLinks.h"
#include "TriggerMenuNtuple/ChainEntry.h"
#include <iostream>
#include <sstream>

using namespace std;

int CombLinks::featureId(const std::string& feature) {
  int id=-1;
  std::map<int, std::string>::iterator p;
  for (p=s_FeatureIdMap.begin(); p!=s_FeatureIdMap.end(); ++p) {
    if (p->second == feature) {
      id = p->first;
      break;
    }
  }
  if (id < 0) {
    addFeature(feature);
    id = static_cast<int>(s_FeatureIdMap.size())-1;
  }
  return id;
}

std::string CombLinks::featureName(int feature_id) {
  std::string name="";
  std::map<int, std::string>::iterator p;
  if ( (p=s_FeatureIdMap.find(feature_id)) != s_FeatureIdMap.end()) {
    name = p->second;
  }
  return name;
}



int CombLinks::addFeature(const std::string& feature) {
  std::map<int, std::string>::iterator p;
  int id=0;
  bool already_exists=false;
  for (p=s_FeatureIdMap.begin(); p!=s_FeatureIdMap.end(); ++p) {
    if (p->second == feature) {
      id = p->first;
      already_exists = true;
      break;
    }
    if (p->first > id) id = p->first + 1;
  }
  if (!already_exists) {
    cout << "Found new type of HLT feature: " << feature << endl;
    id = static_cast<int>(s_FeatureIdMap.size());
    s_FeatureIdMap[id] = feature;
  }
  return id;
}

int CombLinks::addRoIType(const std::string& roi_type) {
  std::map<int, std::string>::iterator p;
  int id=10000;
  for (p=s_FeatureIdMap.begin(); p!=s_FeatureIdMap.end(); ++p) {
    if (p->second == roi_type) {
      id = p->first;
      break;
    }
    if (p->first > id) id = p->first + 1;
  }
  return id;
}

const std::map<int, std::string>& CombLinks::featureIdMap() {
  return s_FeatureIdMap;
}

std::map<int, std::string> CombLinks::s_FeatureIdMap;

CombLinks::CombLinks() : m_RoIType(-1), m_lastStep(-1) , m_state(false), 
			 m_combNumber(-1), 
			 m_TENumber(-1), m_RoINumber(-1), m_TELabelString("") {
}

CombLinks::CombLinks(int roi_type) : m_RoIType(roi_type), 
				     m_lastStep(-1), m_state(false), 
				     m_combNumber(-1), 
				     m_TENumber(-1), m_RoINumber(-1), 
				     m_TELabelString("") {
}

CombLinks::~CombLinks() {
}

bool CombLinks::hasFeature(const std::string& feature) const {
  int id = featureId(feature);
  if (m_indexMap.find(id) != m_indexMap.end() || 
      m_indexVecMap.find(id) != m_indexVecMap.end()) {
    return true;
  } else {
    return false;
  }
}

std::vector<std::string> CombLinks::allFeatureNames() const {
  std::map<int, FeatureIndex>::const_iterator p1;
  std::map<int, FeatureIndexVec_t>::const_iterator p2;
  std::vector<std::string> x;
  for (p1=m_indexMap.begin(); p1!=m_indexMap.end(); ++p1) {
    x.push_back(featureName(p1->first));
  }
  for (p2=m_indexVecMap.begin(); p2!=m_indexVecMap.end(); ++p2) {
    x.push_back(featureName(p2->first));
  }
  return x;
}

std::vector<std::string> CombLinks::FeatureNames() const {
  std::map<int, FeatureIndex>::const_iterator p1;
  std::vector<std::string> x;
  for (p1=m_indexMap.begin(); p1!=m_indexMap.end(); ++p1) {
    x.push_back(featureName(p1->first));
  }
  return x;
}

std::vector<std::string> CombLinks::FeatureVecNames() const {
  std::map<int, FeatureIndexVec_t>::const_iterator p2;
  std::vector<std::string> x;
  for (p2=m_indexVecMap.begin(); p2!=m_indexVecMap.end(); ++p2) {
    x.push_back(featureName(p2->first));
  }
  return x;
}


const FeatureIndex* CombLinks::index(const std::string& feature) const {
  int id = featureId(feature);
  return index(id);
}

const FeatureIndex* CombLinks::index(int feature_id) const {
  std::map<int, FeatureIndex>::const_iterator p = m_indexMap.find(feature_id);
  if (p != m_indexMap.end()) {
    return &(p->second);
  } else {
//     cout << "No feature " << featureName(feature_id) 
// 	 << " found in RoIType " << m_RoIType << endl;
  }
  return 0;
}

const vector<FeatureIndex>* CombLinks::indexVec(const string& feature) const {
  int id = featureId(feature);
  return indexVec(id);
}

const std::vector<FeatureIndex>* CombLinks::indexVec(int feature_id) const {
  std::map<int, FeatureIndexVec_t>::const_iterator p = 
    m_indexVecMap.find(feature_id);
  if (p != m_indexVecMap.end()) {
    return &(p->second);
  } else {
//     cout << "No feature " << featureName(feature_id) 
// 	 << " found in RoIType " << m_RoIType << endl;
  }
  return 0;
}

void CombLinks::addIndex(const std::string& feature, const FeatureIndex& i) {
  if (!i.isValid()) {
//     cout << "CombLinks: FeatureIndex is invalid for feature: " << feature 
// 	 << " index=" << i << endl;
    return;
  }

  int id = featureId(feature);
  //  cout << "CombLinks: Feature id for feature " << feature << " is " << id << endl;
  m_indexMap[id] = i;
//   //  if (!hasFeature(feature)) addFeature(feature);
//   if (hasFeature(feature)) {
//     cout << "CombLinks: has feature" << endl;
//     m_indexMap[id] = i;
//   } else {
//     cout << "Does not have feature" << endl;
//     std::map<int, FeatureIndex>::const_iterator p1;
//     std::map<int, FeatureIndexVec_t>::const_iterator p2;
//     cout << "CombLinks type " << m_RoIType << " does not have link to a feature "
// 	 << feature << endl;
//     cout << "CombLinks: It has links to following features:" << endl;
//     for (p1=m_indexMap.begin(); p1!=m_indexMap.end(); ++p1) {
//       cout << "  " << p1->first << " (int)" << endl;
//     }
//     for (p2=m_indexVecMap.begin(); p2!=m_indexVecMap.end(); ++p2) {
//       cout << "  " << p2->first << " (vector<int>)" << endl;
//     }
//   }
}

void CombLinks::addIndexVec(const std::string& feature, 
			    const FeatureIndexVec_t& iv) {
  if (iv.size() == 0) return;

  int id = featureId(feature);
  m_indexVecMap[id] = iv;
//   if (hasFeature(feature) || true) {
//     m_indexVecMap[id] = iv;
//   } else {
//     std::map<int, FeatureIndex>::const_iterator p1;
//     std::map<int, FeatureIndexVec_t>::const_iterator p2;
//     cout << "CombLinks type " << m_RoIType << " does not have link to a feature "
// 	 << feature << endl;
//     cout << "It has links to following features:" << endl;
//     for (p1=m_indexMap.begin(); p1!=m_indexMap.end(); ++p1) {
//       cout << "  " << p1->first << " (int)" << endl;
//     }
//     for (p2=m_indexVecMap.begin(); p2!=m_indexVecMap.end(); ++p1) {
//       cout << "  " << p2->first << " (vector<int>)" << endl;
//     }
//   }
}

bool CombLinks::isValid() const {
  if (m_indexMap.size() > 0 || m_indexVecMap.size() > 0) {
    return true;
  } else {
    return false;
  }
}

void CombLinks::prependTELabel(const std::string& x) {
  m_TELabelString = x + "," + m_TELabelString;
}

void CombLinks::dump() const {
  cout << (*this) << endl;
}

bool CombLinks::isSameRoI(const CombLinks& x) const {
  bool status = true;
  std::map<int, FeatureIndex>::const_iterator p1;
  std::map<int, FeatureIndexVec_t>::const_iterator p2;

  if (m_RoIType != x.RoIType()) return false;

  for (p1=m_indexMap.begin(); p1!=m_indexMap.end(); ++p1) {
    const FeatureIndex* tmp = x.index(p1->first);
    if (tmp == 0 || p1->second.sameIndex(*tmp)) {
      status = false;
      break;
    }
  }
  if (status) {
    for (p2=m_indexVecMap.begin(); p2!=m_indexVecMap.end(); ++p2) {
      const FeatureIndexVec_t* tmp = x.indexVec(p2->first);
      if (tmp == 0 || !sameIndex(p2->second, *tmp)) {
	status = false;
	break;
      }
    }
  }
  return status;
}

bool CombLinks::operator==(const CombLinks& x) const {
  bool status = true;
  std::map<int, FeatureIndex>::const_iterator p1;
  std::map<int, FeatureIndexVec_t>::const_iterator p2;

  if (m_RoIType != x.RoIType() || 
      m_lastStep != x.lastStep() ||
      m_state != x.active() || 
      m_TENumber != x.TENumber() ||
      m_RoINumber != x.RoINumber() ||
      m_indexMap.size() != x.m_indexMap.size() || 
      m_indexVecMap.size() != x.m_indexVecMap.size()) return false;

  for (p1=m_indexMap.begin(); p1!=m_indexMap.end(); ++p1) {
    const FeatureIndex* tmp = x.index(p1->first);
    if (tmp == 0 || p1->second != (*tmp)) {
      status = false;
      break;
    }
  }
  if (status) {
    for (p2=m_indexVecMap.begin(); p2!=m_indexVecMap.end(); ++p2) {
      const FeatureIndexVec_t* tmp = x.indexVec(p2->first);
      if (tmp == 0 || p2->second != (*tmp)) {
	status = false;
	break;
      }
    }
  }
  return status;
}

std::ostream& operator<<(std::ostream& o, const CombLinks& x) {
  std::map<int, FeatureIndex>::const_iterator p1;
  std::map<int, std::vector<FeatureIndex> >::const_iterator p2;
  std::string feature_str = "";

  o << "CombLinks: RoIType=" << x.m_RoIType 
    << " (valid=" << x.isValid() << ")" << endl;
  o << "  [TE=" << x.TENumber() << "][RoI=" << x.RoINumber()
    << "] active=" << x.active() << endl;
  o << "  TE labels: " << x.TELabelString() << endl;
  for (p1=x.m_indexMap.begin(); p1!=x.m_indexMap.end(); ++p1) {
    feature_str = x.featureName(p1->first);
    if (feature_str.length() == 0) {
      ostringstream os;
      os << p1->first;
      feature_str = os.str();
    }
    o << "  Feature (" << feature_str << "): " << p1->second << endl;
  }
  for (p2=x.m_indexVecMap.begin(); p2!=x.m_indexVecMap.end(); ++p2) {
    feature_str = x.featureName(p2->first);
    if (feature_str.length() == 0) {
      ostringstream os;
      os << p2->first;
      feature_str = os.str();
    }
    o << "  Feature (" << feature_str << "): " << p2->second << endl;
  }
  return o;
}

bool order_by_TELabel::operator()(const CombLinks& x, const CombLinks& y) const {
  if (x.TELabelString() < y.TELabelString()) return true;
  else return false;
}
