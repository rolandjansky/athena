/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/** Adapted from code by A.Hamilton to check trigger EDM; R.Goncalo 21/11/07 */

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "TrigEDMAuxChecker.h"
#include <algorithm>
TrigEDMAuxChecker::TrigEDMAuxChecker(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_count(0)
{
    declareProperty("AuxContainerList",m_auxList);
}

TrigEDMAuxChecker::~TrigEDMAuxChecker() {}

StatusCode TrigEDMAuxChecker::initialize() { 
    ATH_MSG_INFO("Initializing TrigEDMAuxChecker");
    std::vector<std::string> auxvar;
    for(const auto& key:m_auxList){
        ATH_MSG_INFO("REGTEST AuxStore " << key);
        m_auxmap.insert(std::make_pair(key,auxvar));
        m_dynauxmap.insert(std::make_pair(key,auxvar));
    }
    m_count=0;
    return StatusCode::SUCCESS; 
}

StatusCode TrigEDMAuxChecker::finalize() { 

    std::vector<std::string> auxvar;
    for(const auto& key:m_auxList){
        ATH_MSG_INFO("REGTEST : Aux vars for " << key << " ============================");
        auxvar=m_auxmap[key];
        for(const auto& var:auxvar){
            ATH_MSG_INFO("REGTEST : " << key << " " << var);
        }
        ATH_MSG_INFO(" ========================================================");
        ATH_MSG_INFO("REGTEST : Dynamic Aux vars for " << key);
        auxvar=m_dynauxmap[key];
        for(const auto& var:auxvar){
            ATH_MSG_INFO("REGTEST : " << key << " " << var);
        }
        ATH_MSG_INFO(" ========================================================");
    }

    return StatusCode::SUCCESS; 
}

void TrigEDMAuxChecker::dumpDecorators(const xAOD::AuxContainerBase *x,const std::string& key){
    ATH_MSG_DEBUG("REGTEST: DUMP DECORATORS");
    ATH_MSG_DEBUG("Container size " << x->size() );
    //Get list of auxids
    const SG::auxid_set_t& auxIds=x->getAuxIDs(); 
    const SG::auxid_set_t& dyn_auxids = x->getDynamicAuxIDs();
    const std::type_info *type(0);
    //Get registry to determine variable type
    SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();
    ATH_MSG_INFO("Number of auxIds " << auxIds.size() << " Number of dynaux " << dyn_auxids.size());
    std::vector<std::string> auxvar = m_auxmap.at(key);
    std::vector<std::string> dynauxvar = m_dynauxmap.at(key);
    ATH_MSG_DEBUG("======== Aux store variables ==========");
    for (auto ai: auxIds) {
        const std::string &auxName=SG::AuxTypeRegistry::instance().getName(ai);
        type = reg.getType( ai );
        std::vector<std::string>::iterator it = find(auxvar.begin(), auxvar.end(), auxName);
        if (it == auxvar.end()) {
            auxvar.push_back(auxName);
            if(m_count==0) ATH_MSG_INFO("Auxid=" << ai << ", Name=" << auxName << ", type= " << type->name());
            else ATH_MSG_WARNING("REGTEST: Found new Auxid=" << ai << ", Name=" << auxName << ", type= " << type->name());
        }
    
    }
    ATH_MSG_DEBUG("======== Aux store dynamic variables ==========");
   
    for (auto ai: dyn_auxids) {
        const std::string &auxName=SG::AuxTypeRegistry::instance().getName(ai);
        type = reg.getType( ai );
        std::vector<std::string>::iterator it = find(dynauxvar.begin(), dynauxvar.end(), auxName);
        if (it == dynauxvar.end()) {
            dynauxvar.push_back(auxName);
            if(m_count==0) ATH_MSG_INFO("Auxid=" << ai << ", Name=" << auxName << ", type= " << type->name());
            else ATH_MSG_WARNING("REGTEST: Found new Dynamic Auxid=" << ai << ", Name=" << auxName << ", type= " << type->name());
        }
       
    }
    m_auxmap[key]=auxvar;
    m_dynauxmap[key]=dynauxvar;
}

StatusCode TrigEDMAuxChecker::execute() {
  const xAOD::AuxContainerBase* aux=0;
  StatusCode sc = StatusCode::SUCCESS;
  for(const auto& key:m_auxList){
      sc = evtStore()->retrieve(aux,key);
      if (sc.isFailure()) {
          ATH_MSG_WARNING("REGTEST Cannot retrieve " << key);
          continue;
      }
      ATH_MSG_DEBUG("REGTEST: Dump Aux variables for " << key);
      dumpDecorators(aux,key);
  }
  m_count++;
  return sc;
}
