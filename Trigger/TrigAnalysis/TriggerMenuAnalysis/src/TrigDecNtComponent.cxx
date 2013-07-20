/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  TrigDecNtComponent.cxx
*/
#include "TriggerMenuAnalysis/TrigDecNtComponent.h"
#include "TriggerMenuAnalysis/TrigMenuNtupleAlg.h"
#include "TriggerMenuNtuple/TrigDecSummary.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigConfigSvc/TrigConfigSvc.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfHLTData/HLTChainList.h"

#include "boost/foreach.hpp"

using namespace std;

TrigDecNtComponent::TrigDecNtComponent(NtupleAlgorithm* mainAlg, 
				       const NtComponent::NtComponentParameters& params) :
  NtComponent::NtupleComponent(mainAlg, params), 
  mTrigConfigSvc(0), 
  mTrigDecisionTool(0), 
  mTrigDecSummary(0) {
  TrigMenuNtupleAlg* algo = dynamic_cast<TrigMenuNtupleAlg*>(mainAlg);
  mTrigConfigSvc = algo->trigConfigSvc();
  mTrigDecisionTool = algo->trigDecisionTool();
}

TrigDecNtComponent::~TrigDecNtComponent() {
  if (mTrigDecSummary) {
    delete mTrigDecSummary;
    mTrigDecSummary = 0;
  }
}


StatusCode TrigDecNtComponent::book() {
  mTrigDecSummary = new TrigDecSummary();

  m_tree->Branch("TrigDecSummary", &mTrigDecSummary);

  return StatusCode::SUCCESS;
}

StatusCode TrigDecNtComponent::fill() {
   mTrigDecSummary->clear();

   const TrigConf::CTPConfig* ctpConfig = mTrigConfigSvc->ctpConfig();
   const TrigConf::ItemContainer& items = ctpConfig->menu().items();
   const TrigConf::HLTChainList* chainList = mTrigConfigSvc->chainList();

   std::string item_name;
   std::string chain_name;
   std::string level;
   int ctpid;
   int chain_counter;
   unsigned int i;
   unsigned int bits;

   for (i=0; i<items.size(); ++i) {
      item_name = items[i]->name();
      ctpid = items[i]->ctpId();
      if (ctpid >= 256) continue;
      bits = mTrigDecisionTool->isPassedBits(item_name);
      mTrigDecSummary->setBitStatus(ctpid, TrigDecSummary::kL1_TBP, 
                                    bits & TrigDefs::L1_isPassedBeforePrescale);
      mTrigDecSummary->setBitStatus(ctpid, TrigDecSummary::kL1_TAP, 
                                    bits & TrigDefs::L1_isPassedAfterPrescale);
      mTrigDecSummary->setBitStatus(ctpid, TrigDecSummary::kL1_TAV, 
                                    bits & TrigDefs::L1_isPassedAfterVeto);
   }

   BOOST_FOREACH(TrigConf::HLTChain *chain, *chainList) {
      chain_name    = chain->chain_name();
      chain_counter = chain->chain_counter();
      level         = chain->level();
      if (chain_counter >= 8192) continue;
      bits = mTrigDecisionTool->isPassedBits(chain_name);
      if (level == "L2") {
         mTrigDecSummary->setBitStatus(chain_counter, TrigDecSummary::kL2_Raw, 
                                       bits & TrigDefs::L2_passedRaw);
         mTrigDecSummary->setBitStatus(chain_counter, TrigDecSummary::kL2_PT, 
                                       bits & TrigDefs::L2_passThrough);
      } else if (level == "EF") {
         mTrigDecSummary->setBitStatus(chain_counter, TrigDecSummary::kEF_Raw, 
                                       bits & TrigDefs::EF_passedRaw);
         mTrigDecSummary->setBitStatus(chain_counter, TrigDecSummary::kEF_PT, 
                                       bits & TrigDefs::EF_passThrough);
      }
   }

   return StatusCode::SUCCESS;
}

