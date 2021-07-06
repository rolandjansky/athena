/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MonitorChainAlgorithm.h"
#include "../counters/CounterAlgorithm.h"

MonitorChainAlgorithm::MonitorChainAlgorithm(const std::string& name, const MonitoredRange* parent)
    : MonitorBase(name, parent) {
}

StatusCode MonitorChainAlgorithm::newEvent(const CostData& data, const float weight) {
    const std::vector<TrigCompositeUtils::AlgToChainTool::ChainInfo> seededChains = data.seededChains();
    for (size_t i = 0; i < seededChains.size(); ++i){
        // Find algorithms associated with chain name
        if (!data.chainToAlgMap().count(seededChains[i].name)) continue;

        for (const size_t algIndex : data.chainToAlgMap().at(seededChains[i].name)){
            const xAOD::TrigComposite* alg = data.costCollection().at(algIndex);

            const uint32_t slot = alg->getDetail<uint32_t>("slot");
            if (slot != data.onlineSlot()) {
                continue; // When monitoring the master slot, this Monitor ignores algs running in different slots 
            }
            
            const uint32_t nameHash = alg->getDetail<TrigConf::HLTHash>("alg");
            const std::string algName = TrigConf::HLTUtils::hash2string(nameHash, "ALG");

            std::stringstream counterName;
            counterName << seededChains[i].name << "__" << algName;
            ATH_CHECK( getCounter(counterName.str())->newEvent(data, algIndex, weight) );
        }
    }

    return StatusCode::SUCCESS;
}


std::unique_ptr<CounterBase> MonitorChainAlgorithm::newCounter(const std::string& name) {
    return std::make_unique<CounterAlgorithm>(name, this);
} 
