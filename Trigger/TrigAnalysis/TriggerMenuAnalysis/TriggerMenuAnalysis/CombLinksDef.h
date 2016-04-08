/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSIS_COMBLINKSDEF_H
#define TRIGGERMENUANALYSIS_COMBLINKSDEF_H
/*
  CombLinksDef.h
*/
#include "TriggerMenuNtuple/ChainEntry.h"
#include "TriggerMenuNtuple/CombLinks.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TriggerMenuAnalysis/IndexFinder.h"

#include <map>
#include <vector>
#include <string>

class MsgStream;
class StoreGateSvc;

class CombLinksDef {
public:
  static std::map<ChainEntry::RoIType, CombLinksDef*> sCombLinksMap;
  static MsgStream* sLogger;
  static StoreGateSvc* sSGSvc;

  static bool addCombLinksDef(ChainEntry::RoIType type, CombLinksDef* x);
  static const CombLinksDef* getCombLinksDef(ChainEntry::RoIType type);
  static void buildKnownCombLinksDefs(StoreGateSvc* sgsvc, MsgStream* log);

private:
  static void buildKnownCombLinksDefs1();
  static void buildKnownCombLinksDefs2();
  static void buildKnownCombLinksDefs3();
  static void buildKnownCombLinksDefs4();
  static void buildKnownCombLinksDefs5();
  static void buildKnownCombLinksDefs6();
  static void buildKnownCombLinksDefs7();

  CombLinksDef(ChainEntry::RoIType type);

public:
  ~CombLinksDef();

  ChainEntry::RoIType getRoIType() const { return m_RoIType; }
  const std::vector<std::string>& getL1RoIs() const { return m_L1RoIs; }

  CombLinks createCombLinks();
  std::vector<std::string> typeLabelNamesInt() const;
  std::vector<std::string> typeLabelNamesVectorOfInt() const;
  int fillCombLinks(CombLinks& links, 
		    const Trig::Combination& comb,
		    Trig::TrigDecisionTool* tdt,
		    long RoIWord) const;

protected:
  void addL1RoI(const std::string& roi_feature);
  template<class Feature_t, class Cont_t>
  void addFeature(const std::string& cont_key, const std::string& label="");
  template<class Feature_t, class Elem_t>
  void addContFeature(const std::string& cont_key, const std::string& label="");

private:
  ChainEntry::RoIType m_RoIType;
  mutable std::vector<std::string> m_L1RoIs;
  mutable std::vector<IndexFinderBase*> m_singleFeatures;
  mutable std::vector<IndexFinderBase*> m_multipleFeatures;
  mutable std::vector<IndexFinderBase*> m_containerFeatures;

};

template<class Feature_t, class Cont_t>
void CombLinksDef::addFeature(const std::string& cont_key, const std::string& label) {
  IndexFinderBase* x = new IndexFinder<Feature_t, Cont_t>(cont_key, label);
  m_singleFeatures.push_back(x);
}

template<class Feature_t, class Elem_t>
void CombLinksDef::addContFeature(const std::string& cont_key, const std::string& label) {
  IndexFinderBase* x = new IndexFinder<Feature_t, Feature_t, Elem_t>(cont_key, label);
  m_containerFeatures.push_back(x);
}


#endif // TRIGGERMENUANALYSIS_COMBLINKSDEF_H

