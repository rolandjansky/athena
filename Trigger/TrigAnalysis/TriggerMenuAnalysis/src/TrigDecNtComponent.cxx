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
  m_trigConfigSvc(0), 
  m_trigDecisionTool(0), 
  m_trigDecSummary(0) {
  TrigMenuNtupleAlg* algo = dynamic_cast<TrigMenuNtupleAlg*>(mainAlg);
  if (algo) {
    m_trigConfigSvc = algo->trigConfigSvc();
    m_trigDecisionTool = algo->trigDecisionTool();
  }
}

TrigDecNtComponent::~TrigDecNtComponent() {
  if (m_trigDecSummary) {
    delete m_trigDecSummary;
    m_trigDecSummary = 0;
  }
}


StatusCode TrigDecNtComponent::book() {
  m_trigDecSummary = new TrigDecSummary();

  m_tree->Branch("TrigDecSummary", &m_trigDecSummary);

  return StatusCode::SUCCESS;
}

StatusCode TrigDecNtComponent::fill() {
   m_trigDecSummary->clear();

   const TrigConf::CTPConfig* ctpConfig = m_trigConfigSvc->ctpConfig();
   const TrigConf::ItemContainer& items = ctpConfig->menu().items();
   const TrigConf::HLTChainList* chainList = m_trigConfigSvc->chainList();

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
      bits = m_trigDecisionTool->isPassedBits(item_name);
      m_trigDecSummary->setBitStatus(ctpid, TrigDecSummary::kL1_TBP, 
                                    bits & TrigDefs::L1_isPassedBeforePrescale);
      m_trigDecSummary->setBitStatus(ctpid, TrigDecSummary::kL1_TAP, 
                                    bits & TrigDefs::L1_isPassedAfterPrescale);
      m_trigDecSummary->setBitStatus(ctpid, TrigDecSummary::kL1_TAV, 
                                    bits & TrigDefs::L1_isPassedAfterVeto);
   }

   BOOST_FOREACH(TrigConf::HLTChain *chain, *chainList) {
      chain_name    = chain->chain_name();
      chain_counter = chain->chain_counter();
      level         = chain->level();
      if (chain_counter >= 8192) continue;
      bits = m_trigDecisionTool->isPassedBits(chain_name);
      if (level == "L2") {
         m_trigDecSummary->setBitStatus(chain_counter, TrigDecSummary::kL2_Raw, 
                                       bits & TrigDefs::L2_passedRaw);
         m_trigDecSummary->setBitStatus(chain_counter, TrigDecSummary::kL2_PT, 
                                       bits & TrigDefs::L2_passThrough);
      } else if (level == "EF") {
         m_trigDecSummary->setBitStatus(chain_counter, TrigDecSummary::kEF_Raw, 
                                       bits & TrigDefs::EF_passedRaw);
         m_trigDecSummary->setBitStatus(chain_counter, TrigDecSummary::kEF_PT, 
                                       bits & TrigDefs::EF_passThrough);
      }
   }

   return StatusCode::SUCCESS;
}

