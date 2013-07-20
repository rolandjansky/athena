/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __HltNaviChecker_h__
#define __HltNaviChecker_h__
/*
  HltNaviChecker.h
*/
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TriggerMenuAnalysis/RoILinksCnvTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TFile.h"
#include "TTree.h"

class HltNaviChecker : public AthAlgorithm {
public:
  HltNaviChecker(const std::string& name, ISvcLocator* svcloc);
  ~HltNaviChecker();

  StatusCode beginRun();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  void checkFeatures(const std::string& chain_name);
  void checkPassBits();
  void checkFeatures(const HLT::TriggerElement* te, 
		     HLT::NavigationCore* navi, 
		     const std::string& prefix="");
  std::string subTypeLabel(int clid, short subtype_index, 
			   HLT::NavigationCore* navi);

private:
  ToolHandle<Trig::TrigDecisionTool> mTrigDecisionTool;
  ToolHandle<RoILinksCnvTool> mRoILinksCnvTool;
  std::vector<std::string> mChainNames;

  TFile* mFile;
  TTree* mTree;
  int mNPassBits;
  std::vector<unsigned int>* mEFElectronPassBitsIndex;
  
};

#endif // __HltNaviChecker_h__
