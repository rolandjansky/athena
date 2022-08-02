/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCompositeUtils/AlgToChainTool.h"
#include "boost/algorithm/string/predicate.hpp"

#ifndef XAOD_STANDALONE

TrigCompositeUtils::AlgToChainTool::AlgToChainTool(const std::string& type,
                        const std::string& name,
                        const IInterface* parent)
    : AthAlgTool (type, name, parent) 
    {}


TrigCompositeUtils::AlgToChainTool::~AlgToChainTool() {}


StatusCode TrigCompositeUtils::AlgToChainTool::initialize() {
    ATH_CHECK( m_HLTMenuKey.initialize() );

    return StatusCode::SUCCESS;
}


StatusCode TrigCompositeUtils::AlgToChainTool::start() {
    SG::ReadHandle<TrigConf::HLTMenu>  hltMenuHandle = SG::makeHandle( m_HLTMenuKey );
    ATH_CHECK( hltMenuHandle.isValid() );

    // Fill the maps
    for ( const TrigConf::Chain& chain : *hltMenuHandle ) {
        for ( const std::string& sequencer : chain.sequencers() ) {
            // Skip empty steps = empty sequencers in the list
            if (sequencer.empty()) continue;
            m_sequencerToChainMap[sequencer].push_back(chain);
        }
    }

    for ( const auto& sequencer : hltMenuHandle->sequencers() ) {
        for ( const std::string& algorithm : sequencer.second ) {
            // PassFilter is for empty steps - will never be associated with a chain
            if (boost::starts_with (algorithm, "PassFilter")) continue;

            // Save just second part of algorithm ex. RoRSeqFilter/FFastCaloElectron -> FFastCaloElectron
            m_algToSequencersMap[algorithm.substr(algorithm.find('/') + 1)]
                .push_back(sequencer.first);
        }
    }

    m_cachedEventID = 0; // Revet the cache identifier

    return StatusCode::SUCCESS;
}

StatusCode TrigCompositeUtils::AlgToChainTool::getAllChains(std::vector<TrigConf::Chain>& chainNames) const {
    SG::ReadHandle<TrigConf::HLTMenu>  hltMenuHandle = SG::makeHandle( m_HLTMenuKey );
    ATH_CHECK( hltMenuHandle.isValid() );

    for ( const TrigConf::Chain& chain : *hltMenuHandle ) {
        chainNames.push_back(chain);
    }
    return StatusCode::SUCCESS;
}


std::vector<std::string> TrigCompositeUtils::AlgToChainTool::getChainsForAlg(const std::string& algorithmName) const {
    std::vector<std::string> result;

    try {
        for ( const std::string& sequencer : m_algToSequencersMap.at(algorithmName) ) {
            try {
                for (const TrigConf::Chain& chain : m_sequencerToChainMap.at(sequencer)){
                    result.push_back(chain.name());
                }
            }
            catch ( const std::out_of_range & ex ) {
                ATH_MSG_DEBUG ( "Sequence " << sequencer << " is not part of the menu!" );             
            }
        }
    } catch ( const std::out_of_range & ex ) {
        ATH_MSG_DEBUG ( "Algorithm " << algorithmName << " is not part of the menu!" );
    }

    return result;
}


std::set<std::string> TrigCompositeUtils::AlgToChainTool::getActiveChainsForAlg(const std::string& algorithmName, const EventContext& context) const {

    std::set<std::string> allActiveChains;
    try {
        for ( const std::string& sequenceName : m_algToSequencersMap.at(algorithmName) ) {

            std::string filterName = createCollectionName(sequenceName);
            std::set<TrigCompositeUtils::DecisionID> activeChainsInSequence = retrieveActiveChains(context, filterName);

            for ( const TrigCompositeUtils::DecisionID& id : activeChainsInSequence ) {
                allActiveChains.insert( HLT::Identifier(id).name() );
            }
        }

    } catch ( const std::out_of_range & ex ) {
        ATH_MSG_DEBUG ( "Algorithm " << algorithmName << " is not part of the menu!" );
    }

    return allActiveChains;
}


StatusCode TrigCompositeUtils::AlgToChainTool::getAllActiveSequences( const EventContext& context, std::map<std::string, std::string>& algToSeq) {

    // Retrieve EventStore and keys if not cached
    IProxyDict* storeProxy = Atlas::getExtendedEventContext(context).proxy();
    SmartIF<SGImplSvc> eventStore (storeProxy);
    if (m_cachedEventID != context.eventID().event_number()){
        ATH_MSG_INFO("Caching the event store keys for event " << context.eventID().event_number());
        eventStore->keys(static_cast<CLID>( ClassID_traits<TrigCompositeUtils::DecisionContainer>::ID() ), m_cachedEventStoreKeys);
        m_cachedEventID = context.eventID().event_number();
    }

    SG::ReadHandle<TrigConf::HLTMenu>  hltMenuHandle = SG::makeHandle( m_HLTMenuKey, context );
    ATH_CHECK( hltMenuHandle.isValid() );
    for (const auto& sequence : hltMenuHandle->sequencers()) {
        std::string filterName = createCollectionName(sequence.first);

        // Optimize
        auto foundKey = std::find_if(m_cachedEventStoreKeys.begin(), m_cachedEventStoreKeys.end(), [&](const std::string& key) {
            return boost::starts_with(key, filterName);
        });

        if (foundKey != m_cachedEventStoreKeys.end() ){
            // Check if sequence passed - if at least one DecisionObject was produced
            for ( const TrigCompositeUtils::Decision* d : *getDecisionFromStore(eventStore, *foundKey) ) {
                if (!d->decisions().empty()){
                    // Save algorithm to active sequence mapping
                    for (const std::string& alg : sequence.second) {
                        algToSeq[alg.substr(alg.find('/') + 1, alg.size())] = sequence.first;
                    }
                    break;
                }
            }
        }
    }

    return StatusCode::SUCCESS;
}

void TrigCompositeUtils::AlgToChainTool::cacheSGKeys(const EventContext& context) {
    if (m_cachedEventID != context.eventID().event_number()){
        ATH_MSG_INFO("Caching the event store keys for event " << context.eventID().event_number());
        m_cachedEventStoreKeys = readSGKeys(context);
        m_cachedEventID = context.eventID().event_number();
    }
}

std::vector<std::string> TrigCompositeUtils::AlgToChainTool::readSGKeys(const EventContext& context) const {
    std::vector<std::string> keys;
    IProxyDict* storeProxy = Atlas::getExtendedEventContext(context).proxy();
    SmartIF<SGImplSvc> eventStore (storeProxy);
    eventStore->keys(static_cast<CLID>( ClassID_traits<TrigCompositeUtils::DecisionContainer>::ID() ), keys);
    return keys;
}

std::set<TrigCompositeUtils::DecisionID> TrigCompositeUtils::AlgToChainTool::retrieveActiveChains(const EventContext& context, const std::string& collectionName) const {
    
    if (m_cachedEventID == context.eventID().event_number()) {
        return retrieveActiveChainsForKeys(context, collectionName, m_cachedEventStoreKeys);
    } else {
        const std::vector<std::string> keys = readSGKeys(context);
        return retrieveActiveChainsForKeys(context, collectionName, keys);
    }
}


std::set<TrigCompositeUtils::DecisionID> TrigCompositeUtils::AlgToChainTool::retrieveActiveChainsForKeys(const EventContext& context, const std::string& collectionName, const std::vector<std::string>& keys) const {
    std::set<TrigCompositeUtils::DecisionID> activeChainsID;

    // Retrieve EventStore and keys if not cached
    IProxyDict* storeProxy = Atlas::getExtendedEventContext(context).proxy();
    SmartIF<SGImplSvc> eventStore (storeProxy);

    // Retrieve active chains name hashes
    for ( const std::string& key : keys ) {

        // Look for given collection
        if ( !collectionName.empty() && (!boost::starts_with(key, collectionName)) ){
            continue;
        }

        // Get data from any nav collection
        if( collectionName.empty() && (!boost::starts_with(key, "HLTNav") || key == "HLTNav_Summary") ) {
            continue;
        }

        for ( const TrigCompositeUtils::Decision* d : *getDecisionFromStore(eventStore, key) ) {
            TrigCompositeUtils::DecisionIDContainer chainsID;
            TrigCompositeUtils::decisionIDs( d, chainsID );

            // Save the active chains IDs
            for (TrigCompositeUtils::DecisionID id : chainsID){
                activeChainsID.insert(TrigCompositeUtils::getIDFromLeg(id));
            }
        }
    }

    return activeChainsID;
}


StatusCode TrigCompositeUtils::AlgToChainTool::getChainsForAllAlgs(const EventContext& context, std::map<std::string, std::set<std::string>>& algToChain) const {

    // Look for chains which were active for any of the algorithms of the sequence
    std::map<std::string, std::set<TrigCompositeUtils::DecisionID>> seqToActiveChains;
    for (const auto& sequence : m_sequencerToChainMap) {
        // Look for associated filters names with the sequence
        std::string filterName = createCollectionName(sequence.first);
        seqToActiveChains[sequence.first] = retrieveActiveChains(context, filterName);
    }

    for (const auto& algSeqPair : m_algToSequencersMap){
        std::set<std::string> activeChains;
        for (const std::string& sequenceName : algSeqPair.second){
            // Save all active chains per sequences that algorithm was executed
            for (TrigCompositeUtils::DecisionID chainId : seqToActiveChains.at(sequenceName)){
                activeChains.insert(HLT::Identifier(chainId).name());
            }
        }

        algToChain[algSeqPair.first] = activeChains;
    }

    return StatusCode::SUCCESS;
}



StatusCode TrigCompositeUtils::AlgToChainTool::getChainInfo(const EventContext& context, TrigCompositeUtils::DecisionID decId, ChainInfo& info) const {
    SG::ReadHandle<TrigConf::HLTMenu>  hltMenuHandle = SG::makeHandle( m_HLTMenuKey, context );
    ATH_CHECK( hltMenuHandle.isValid() );

    HLT::Identifier id = HLT::Identifier(decId);
    if (TrigCompositeUtils::isLegId(id)){
        id = getIDFromLeg(id);
    }

    info.id = id;

    // Find chain with given id
    TrigConf::HLTMenu::const_iterator chain = std::find_if(hltMenuHandle->begin(), hltMenuHandle->end(), 
        [&id](const TrigConf::Chain& c) {return c.namehash() == id;}
    );

    if(chain == hltMenuHandle->end()){
        ATH_MSG_WARNING("Chain " << id << " not found in the menu!");
        info.name = id.name();
        return StatusCode::SUCCESS;
    }

    info.name = (*chain).name();
    info.groups = (*chain).groups();

    // Check if chain passed - is in HLTPassRaw collection
    IProxyDict* storeProxy = Atlas::getExtendedEventContext(context).proxy();
    SmartIF<SGImplSvc> eventStore (storeProxy);
    SG::ReadHandle<TrigCompositeUtils::DecisionContainer> dc = getDecisionFromStore(eventStore, "HLTNav_Summary");

    const TrigCompositeUtils::Decision* passRaw = TrigCompositeUtils::getTerminusNode(dc);
    TrigCompositeUtils::DecisionIDContainer chainsID;
    TrigCompositeUtils::decisionIDs( passRaw, chainsID );
    info.isPassRaw = std::find(chainsID.begin(), chainsID.end(), id) != chainsID.end();

    return StatusCode::SUCCESS;
}


SG::ReadHandle<TrigCompositeUtils::DecisionContainer> TrigCompositeUtils::AlgToChainTool::getDecisionFromStore(SmartIF<SGImplSvc>& eventStore, const std::string& key) const {
    SG::DataProxy* dp = eventStore->proxy(
        static_cast<CLID>(ClassID_traits<TrigCompositeUtils::DecisionContainer>::ID()), key, true);

    SG::ReadHandle<TrigCompositeUtils::DecisionContainer> dc (dp);
    if ( !dc.isValid() ) {            
        ATH_MSG_WARNING("Failed to retrieve " << key << " from event store.");            
        return SG::ReadHandle<TrigCompositeUtils::DecisionContainer>();        
    }

    return dc;
}


std::string TrigCompositeUtils::AlgToChainTool::createCollectionName(const std::string& sequenceName) const{
    std::string filterName = "HLTNav_F" + sequenceName + "__";

    if (filterName.find("Trig") != std::string::npos){
        filterName.replace(filterName.find("Trig"), 4, "");
    }

    return filterName;
}

#endif // XAOD_STANDALONE
