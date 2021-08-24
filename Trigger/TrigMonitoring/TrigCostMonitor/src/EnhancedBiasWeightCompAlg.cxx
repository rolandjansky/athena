/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigCostMonitor/EnhancedBiasWeightCompAlg.h"

#include <sstream>


EnhancedBiasWeightCompAlg::EnhancedBiasWeightCompAlg(const std::string& name, ISvcLocator* svcLoc)
    : AthReentrantAlgorithm(name, svcLoc) {}

StatusCode EnhancedBiasWeightCompAlg::initialize(){
    
    ATH_CHECK( m_HLTMenuKey.initialize() );
    if (!m_finalDecisionKey.empty()) {
        ATH_CHECK( m_finalDecisionKey.initialize() );
    }
    ATH_CHECK( m_EBWeightKey.initialize() );

    ATH_CHECK( m_HLTPrescaleSetInputKey.initialize() );
    ATH_CHECK( m_L1PrescaleSetInputKey.initialize() );

    // Retireve TDT
    if (!m_tdt.empty()){
        ATH_CHECK( m_tdt.retrieve() );
    }

    return StatusCode::SUCCESS;
}


StatusCode EnhancedBiasWeightCompAlg::start() {
    
    SG::ReadHandle<TrigConf::HLTMenu> hltMenuHandle = SG::makeHandle( m_HLTMenuKey );
    ATH_CHECK( hltMenuHandle.isValid() );

    // Save ids of EB chains - that contain "_eb_"
    m_EBChainIds = std::vector<TrigCompositeUtils::DecisionID>();
    for (const TrigConf::Chain& chain : *hltMenuHandle){
        std::vector<std::string> streams = chain.streams();
        if (std::find(streams.begin(), streams.end(), "EnhancedBias") != streams.end()){
            auto chainId = HLT::Identifier(chain.name());
            m_EBChainIds.push_back(chainId);
            m_EBChainIdToItem[chainId] = parseItems(chain.l1item());
        }
    }

    return StatusCode::SUCCESS;
}

StatusCode EnhancedBiasWeightCompAlg::stop(){

    // Save weights and events mapping to xml
    if (!m_outputFilename.empty()){
        std::ofstream outputStream;
        outputStream.open(m_outputFilename);
        outputStream << "<?xml version=\"1.0\" encoding=\"us-ascii\"?>" << std::endl;
        outputStream << "<run>" << std::endl;
        outputStream << "<weights>" << std::endl;

        for (size_t i = 0; i < m_ebWeights.size(); ++i){
            outputStream << "<weight id=\"" << i << "\" value=\"" << m_ebWeights[i].first << "\" unbiased=\"" << m_ebWeights[i].second << "\"/>" << std::endl;
        }

        outputStream << "</weights>" << std::endl;
        outputStream << "<events>" << std::endl;

        for (const auto& event : m_eventToWeight){
            outputStream << "<e n=\"" << event.first << "\" w=\"" << event.second << "\"/>" << std::endl;
        }

        outputStream << "</events>" << std::endl;
        outputStream << "</run>" << std::endl;
        outputStream.close();
    }

    return StatusCode::SUCCESS;
}

StatusCode EnhancedBiasWeightCompAlg::execute(const EventContext& context) const {

    // Retrieve information about EB chains that passed
    std::vector<EBChainInfo> EBChains;
    SG::WriteHandle<xAOD::TrigCompositeContainer> outputHandle;
    if (!m_tdt.empty()){
        for (const auto& chainId : m_EBChainIds){
            std::string chainName = HLT::Identifier(chainId).name();
            if (m_tdt->isPassed(chainName)){
                EBChains.push_back(EBChainInfo(HLT::Identifier(chainId)));
            }
        }
    } else {
        auto finalDecisionsHandle = SG::makeHandle( m_finalDecisionKey, context );
        ATH_CHECK( finalDecisionsHandle.isValid() );

        const TrigCompositeUtils::Decision* decisionObject = TrigCompositeUtils::getTerminusNode(finalDecisionsHandle);
        ATH_CHECK(decisionObject != nullptr);
        EBChains = getPassedEBChains(*decisionObject);

        // Setup output handle
        outputHandle = TrigCompositeUtils::createAndStore(m_EBWeightKey, context);
    }

    // None of EB chains passed the algorithm
    if (EBChains.empty()) return StatusCode::SUCCESS;

    // Retrieve L1 and HLT prescales
    ATH_CHECK( getPrescales(context, EBChains) );
    
    // Calculate EB weight
    EBResult result = calculateEBWeight(EBChains);

    // Save output values: EBWeight and EBUnbiased
    if (outputHandle.isValid()){
        // Save to xAOD
        xAOD::TrigComposite* tc = new xAOD::TrigComposite();
        outputHandle->push_back(tc);

        tc->setDetail<float>("EBWeight", result.weight);
        tc->setDetail<uint8_t>("EBUnbiased", result.isUnbiased);
    } else {

        // Create xml filename (run number necessary)
        if ( context.evt() == 0 ){
            std::stringstream filename;
            filename << "EnhancedBiasWeights_" << context.eventID().run_number() << ".xml";
            m_outputFilename = filename.str();
        }

        // Save to containers to be saved to xml
        auto resultPair = std::make_pair<int, bool>(result.weight, result.isUnbiased);
        auto newPair = std::find(m_ebWeights.begin(), m_ebWeights.end(), resultPair);
        if (newPair == m_ebWeights.end()){
            newPair = m_ebWeights.push_back(resultPair);
        }
        m_eventToWeight[context.eventID().event_number()] = std::distance(m_ebWeights.begin(), newPair);
    }

    ATH_MSG_DEBUG("EnhacedBias EBWeight: " << result.weight << " EnhacedBias isUnbiased: " << (bool) result.isUnbiased );

    return StatusCode::SUCCESS;
}


StatusCode EnhancedBiasWeightCompAlg::getPrescales(const EventContext& context, std::vector<EBChainInfo>& EBChains) const {

    SG::ReadCondHandle<TrigConf::HLTPrescalesSet> HLTPrescalesSet (m_HLTPrescaleSetInputKey, context);
    ATH_CHECK( HLTPrescalesSet.isValid() );
    
    SG::ReadCondHandle<TrigConf::L1PrescalesSet> L1PrescalesSet (m_L1PrescaleSetInputKey, context);
    ATH_CHECK( L1PrescalesSet.isValid() );

    for (EBChainInfo& chain : EBChains) {

        double HLTPrescale = HLTPrescalesSet->prescale(chain.getId()).prescale;
        double L1Prescale = 1.0;
        if (!chain.getIsNoPS()) {
            for (const std::string& item : m_EBChainIdToItem.at(chain.getId())){
                L1Prescale *= L1PrescalesSet->prescale(item).prescale;
            }
        }

        chain.setPrescale(HLTPrescale * L1Prescale);
    }

    return StatusCode::SUCCESS;
}


EnhancedBiasWeightCompAlg::EBResult EnhancedBiasWeightCompAlg::calculateEBWeight(const std::vector<EBChainInfo>& EBChains) const{

    double weight = 1.;

    for (const EBChainInfo& chain : EBChains){
       weight *= 1. - ( 1. / chain.getPrescale() );
    }

    weight = (std::fabs(1.0 - weight) < 1e-10) ? 0. : (1. / (1. - weight));

    // Check if the event was triggered by EB noalg random chain
    bool isUnbiased = checkIfTriggeredByRandomChain(EBChains);
    
    return {float(weight), isUnbiased}; 
}


std::vector<EnhancedBiasWeightCompAlg::EBChainInfo> EnhancedBiasWeightCompAlg::getPassedEBChains(const TrigCompositeUtils::Decision& decisionObject) const {
    
    std::vector<EBChainInfo> passedEBChains;

    for (const TrigCompositeUtils::DecisionID& chainId : m_EBChainIds) {
        if (std::find(decisionObject.decisions().begin(), decisionObject.decisions().end(), chainId) != decisionObject.decisions().end()) {
            passedEBChains.push_back(EBChainInfo(HLT::Identifier(chainId)));
        }
    }

    return passedEBChains;
}


bool EnhancedBiasWeightCompAlg::checkIfTriggeredByRandomChain(const std::vector<EBChainInfo>& EBChains) const {
    
    return std::find_if (EBChains.begin(), EBChains.end(), 
        [](const EBChainInfo& chain) -> bool { return chain.getIsRandom(); }) != EBChains.end();
}

std::vector<std::string> EnhancedBiasWeightCompAlg::parseItems(const std::string& itemStr) {
    
    std::vector<std::string> items;

    std::stringstream itemStream (itemStr);
    std::string item;

    while (std::getline(itemStream, item, ',')) {
        items.push_back(item);
    }

    return items;
}