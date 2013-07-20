/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __CombLinksDef_h__
#define __CombLinksDef_h__
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
  CombLinksDef(ChainEntry::RoIType type);

public:
  ~CombLinksDef();

  ChainEntry::RoIType getRoIType() const { return mRoIType; }
  const std::vector<std::string>& getL1RoIs() const { return mL1RoIs; }

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
  ChainEntry::RoIType mRoIType;
  mutable std::vector<std::string> mL1RoIs;
  mutable std::vector<IndexFinderBase*> mSingleFeatures;
  mutable std::vector<IndexFinderBase*> mMultipleFeatures;
  mutable std::vector<IndexFinderBase*> mContainerFeatures;

};

template<class Feature_t, class Cont_t>
void CombLinksDef::addFeature(const std::string& cont_key, const std::string& label) {
  IndexFinderBase* x = new IndexFinder<Feature_t, Cont_t>(cont_key, label);
  mSingleFeatures.push_back(x);
}

template<class Feature_t, class Elem_t>
void CombLinksDef::addContFeature(const std::string& cont_key, const std::string& label) {
  IndexFinderBase* x = new IndexFinder<Feature_t, Feature_t, Elem_t>(cont_key, label);
  mContainerFeatures.push_back(x);
}


#endif // __CombLinksDef_h__

