/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  CombLinksDef.cxx
*/
#include "GaudiKernel/MsgStream.h"

#include "TriggerMenuAnalysis/CombLinksDef.h"
#include "TriggerMenuAnalysis/IndexFinderBase.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

using namespace std;

std::map<ChainEntry::RoIType, CombLinksDef*> CombLinksDef::sCombLinksMap;
MsgStream* CombLinksDef::sLogger=0;
StoreGateSvc* CombLinksDef::sSGSvc=0;

bool CombLinksDef::addCombLinksDef(ChainEntry::RoIType type, CombLinksDef* x) {
  if (sCombLinksMap.find(type) == sCombLinksMap.end()) {
    sCombLinksMap[type] = x;
    return true;
  } else {
    return false;
  }
}

const CombLinksDef* CombLinksDef::getCombLinksDef(ChainEntry::RoIType type) {
  std::map<ChainEntry::RoIType, CombLinksDef*>::iterator p = 
    sCombLinksMap.find(type);
  if (p == sCombLinksMap.end()) {
    return 0;
  } else {
    return p->second;
  }
}

void CombLinksDef::buildKnownCombLinksDefs(StoreGateSvc* sgsvc, MsgStream* log) {
  sSGSvc = sgsvc;
  sLogger = log;

  // Need to split this up to keep the memory required for compilation
  // from exploding.
  buildKnownCombLinksDefs1();
  buildKnownCombLinksDefs2();
  buildKnownCombLinksDefs3();
  buildKnownCombLinksDefs4();
  buildKnownCombLinksDefs5();
  buildKnownCombLinksDefs6();
  buildKnownCombLinksDefs7();
}

CombLinksDef::CombLinksDef(ChainEntry::RoIType type) :
  m_RoIType(type) {
}

CombLinksDef::~CombLinksDef() {
  std::vector<IndexFinderBase*>::iterator p1;
  std::vector<IndexFinderBase*>::iterator p2;

  for (p1=m_singleFeatures.begin(); p1!=m_singleFeatures.end(); ++p1) {
    if (*p1) delete *p1;
    *p1 = 0;
  }
  for (p2=m_containerFeatures.begin(); p2!=m_containerFeatures.end(); ++p2) {
    if (*p2) delete *p2;
    *p2 = 0;
  }
}

CombLinks CombLinksDef::createCombLinks() {
  CombLinks x(static_cast<int>(m_RoIType));
  return x;
}

std::vector<std::string> CombLinksDef::typeLabelNamesInt() const {
  std::vector<std::string> s;
  std::string tmp;
  std::vector<IndexFinderBase*>::iterator p;

  for (p=m_singleFeatures.begin(); p!=m_singleFeatures.end(); ++p) {
    tmp = (*p)->typeLabel();
    s.push_back(tmp);
  }
  copy(m_L1RoIs.begin(), m_L1RoIs.end(), back_inserter(s));
  return s;
}

std::vector<std::string> CombLinksDef::typeLabelNamesVectorOfInt() const {
  std::vector<std::string> s;
  std::string tmp;
  std::vector<IndexFinderBase*>::iterator p;

  for (p=m_containerFeatures.begin(); p!=m_containerFeatures.end(); ++p) {
    tmp = (*p)->typeLabel();
    s.push_back(tmp);
  }
  return s;
}

void CombLinksDef::addL1RoI(const std::string& roi_feature) {
  if (find(m_L1RoIs.begin(), m_L1RoIs.end(), roi_feature) == m_L1RoIs.end()) {
    m_L1RoIs.push_back(roi_feature);
  }
}

int CombLinksDef::fillCombLinks(CombLinks& links, 
				const Trig::Combination& comb,
				Trig::TrigDecisionTool* tdt,
				long RoIWord) const {
  const std::vector<std::string>& l1rois = getL1RoIs();
  std::vector<std::string>::const_iterator p_roi;
  for (p_roi=l1rois.begin(); p_roi!=l1rois.end(); ++p_roi) {
    const FeatureIndex* tmp = links.index(*p_roi);
    if(!tmp) links.addIndex( *p_roi, FeatureIndex(-1,0) );
  }

  std::vector<IndexFinderBase*>::iterator p1;
  std::vector<IndexFinderBase*>::iterator p2;
  std::string te_label="";

  links.setTELabelString("");
  for (p1=m_singleFeatures.begin(); p1!=m_singleFeatures.end(); ++p1) {
    (*p1)->setTrigDecisionTool(tdt);
    (*p1)->setLogger(sLogger);
    (*p1)->setL1RoIName( m_L1RoIs[0] );
    (*p1)->setL1ROIWord( RoIWord );
    FeatureIndex x = (*p1)->findIndex(comb, te_label);
    links.prependTELabel(te_label);
    links.addIndex( (*p1)->typeLabel(), x);
  }
  for (p2=m_containerFeatures.begin(); p2!=m_containerFeatures.end(); ++p2) {
    (*p2)->setTrigDecisionTool(tdt);
    (*p2)->setLogger(sLogger);
    (*p2)->setL1RoIName( m_L1RoIs[0] );
    (*p2)->setL1ROIWord( RoIWord );
    std::vector<FeatureIndex> x = (*p2)->findIndexes(comb, te_label);
    links.prependTELabel(te_label);
    links.addIndexVec( (*p2)->typeLabel(), x);
  }
  return 0;
}

