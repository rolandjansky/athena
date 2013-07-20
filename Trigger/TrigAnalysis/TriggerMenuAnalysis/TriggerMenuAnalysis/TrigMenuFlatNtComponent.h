/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TrigMenuFlatNtComponent_h__
#define __TrigMenuFlatNtComponent_h__
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
  std::string mBranchName;
  ChainEntry::RoIType mRoIType;
  RoILinksCnvTool* mRoILinksCnvTool;
  MsgStream* mLog;

  // Ntuple variables
  vi_t* mType;
  vi_t* mLastStep;
  std::map<int, vi_t*> mFeatureIndex;
  std::map<int, vi_t*> mFeatureStatus;
  std::map<int, vvi_t*> mFeatureIndexVec;
  std::map<int, vvi_t*> mFeatureStatusVec;
  
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
  Trig::TrigDecisionTool* mTrigDecisionTool;
  //  TrigAccessTool* mTrigAccessTool;
  RoILinksCnvTool* mRoILinksCnvTool;

  // Transient data
  std::vector<ChainEntry> mChainEntries;
  RoILinks mRoILinks;

  //-----------------------------------------------
  // ntuple variables
  //-----------------------------------------------
  // Chain Entries
  vi_t* mChainId;
  vvi_t* mRoIType;
  vvi_t* mRoIIndex;

  // RoI
  std::vector<RoINtComponent> mRoINtComponents;

};


#endif // __TrigMenuFlatNtComponent_h__
