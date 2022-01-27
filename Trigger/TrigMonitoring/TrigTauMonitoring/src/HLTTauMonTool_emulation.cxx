/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TProfile.h"
#include "HLTTauMonTool.h"

using namespace std;

std::string HLTTauMonTool::LowerChain(const std::string& hlt_item){

  ATH_MSG_DEBUG("HLTTauMonTool::LowerChain() " << hlt_item);
  
  std::string l1_chain("");

  TrigConf::HLTChain* hlt_chain;
  if( (hlt_chain = m_configsvc->chains().chain(hlt_item)) ){
    ATH_MSG_DEBUG("HLT Chain " << hlt_item << " found in TrigConfSvc!");
    const std::string& lower_chain( hlt_chain->lower_chain_name() );
    l1_chain = lower_chain;
  }else{
    const auto *trig_conf = getTDT()->ExperimentalAndExpertMethods().getChainConfigurationDetails(hlt_item);
    if(trig_conf != nullptr){
      ATH_MSG_DEBUG("TrigConf is available");
      l1_chain = trig_conf->lower_chain_name(); //L1 trigger seed
    }    
  }

  ATH_MSG_DEBUG("L1 Chain " << l1_chain << " found in TrigConfSvc!");

  return l1_chain;
}


