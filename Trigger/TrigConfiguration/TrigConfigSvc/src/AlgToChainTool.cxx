/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AlgToChainTool.h"

TrigConf::AlgToChainTool::AlgToChainTool(const std::string& type,
                        const std::string& name,
                        const IInterface* parent)
    : AthAlgTool (type, name, parent) 
    {}


TrigConf::AlgToChainTool::~AlgToChainTool() {}


StatusCode TrigConf::AlgToChainTool::initialize() {
    ATH_CHECK( m_HLTMenuKey.initialize() );

    return StatusCode::SUCCESS;
}


StatusCode TrigConf::AlgToChainTool::start() {
    SG::ReadHandle<TrigConf::HLTMenu>  hltMenuHandle = SG::makeHandle( m_HLTMenuKey );
    ATH_CHECK( hltMenuHandle.isValid() );

    // fill the maps
    for ( const TrigConf::Chain& chain : *hltMenuHandle ) {
        for ( const std::string& sequencer : chain.sequencers() ) {
            m_sequencerToChainMap[sequencer].push_back(chain.name());
        }
    }

    for ( const auto& sequencer : hltMenuHandle->sequencers() ) {
        for ( const std::string& algorithm : sequencer.second ) {
            // save just second part of algorithm ex. RoRSeqFilter/FFastCaloElectron -> FFastCaloElectron
            m_algToSequencersMap[algorithm.substr(algorithm.find('/') + 1)]
                .push_back(sequencer.first);
        }
    }

    return StatusCode::SUCCESS;
}


std::set<std::string> TrigConf::AlgToChainTool::getChainsForAlg(const std::string& algorithmName) const {
    std::set<std::string> result;

    try {
        for ( const std::string& sequencer : m_algToSequencersMap.at(algorithmName) ) {
            result.insert(m_sequencerToChainMap.at(sequencer).begin(), m_sequencerToChainMap.at(sequencer).end());
        }
    } catch ( const std::out_of_range & ex ) {
        ATH_MSG_ERROR ( algorithmName << " is not part of the menu!" );
    }

    return result;
}