/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSIS_TRIGMENUFLATNTCOMPONENT_H
#define TRIGGERMENUANALYSIS_TRIGMENUFLATNTCOMPONENT_H
/*
  TrigMenuFlatNtComponent.h
*/
#include "TrigNtupleBase/NtupleComponent.h"
#include <vector>
#include "TriggerMenuNtuple/ChainEntry.h"
#include "TriggerMenuNtuple/RoILinks.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TriggerMenuAnalysis/TrigAccessTool.h"
#include "TriggerMenuAnalysis/RoILinksCnvTool.h"

typedef std::vector<int> vi_t;
typedef std::vector<std::vector<int> > vvi_t;


class RoINtComponent {
public:
  RoINtComponent(const std::string& brname="", 
		 ChainEntry::RoIType roi_type=ChainEntry::kUnknown, 
		 RoILinksCnvTool* roitool=0, 
		 MsgStream* msg=0);
  ~RoINtComponent();
  
  void book(TTree* t);
  void fill(const RoILinks& roilinks);
  
  void clear();

private:
  std::string m_branchName;
  ChainEntry::RoIType m_RoIType;
  RoILinksCnvTool* m_RoILinksCnvTool;
  MsgStream* m_log;

  // Ntuple variables
  vi_t* m_type;
  vi_t* m_lastStep;
  std::map<int, vi_t*> m_featureIndex;
  std::map<int, vi_t*> m_featureStatus;
  std::map<int, vvi_t*> m_featureIndexVec;
  std::map<int, vvi_t*> m_featureStatusVec;
  
};

class TrigMenuFlatNtComponent : public NtComponent::NtupleComponent {
public:
  TrigMenuFlatNtComponent(NtupleAlgorithm* mainAlg, 
		      const NtComponent::NtComponentParameters& params);
  ~TrigMenuFlatNtComponent();

  StatusCode book();
  StatusCode fill();

  void clear();

private:
  void addRoINtComponent(const std::string& brname, 
			 ChainEntry::RoIType roi_type);

private:
  Trig::TrigDecisionTool* m_trigDecisionTool;
  //  TrigAccessTool* mTrigAccessTool;
  RoILinksCnvTool* m_RoILinksCnvTool;

  // Transient data
  std::vector<ChainEntry> m_chainEntries;
  RoILinks m_RoILinks;

  //-----------------------------------------------
  // ntuple variables
  //-----------------------------------------------
  // Chain Entries
  vi_t* m_chainId;
  vvi_t* m_RoIType;
  vvi_t* m_RoIIndex;

  // RoI
  std::vector<RoINtComponent> m_RoINtComponents;

};


#endif // TRIGGERMENUANALYSIS_TRIGMENUFLATNTCOMPONENT_H
