/*
 * TriggerAnalysisHelper.cxx
 * Copyright (C) 2016 Ryan Mackenzie White <ryan.white@cern.ch>
 *
 * Distributed under terms of the  license.
 */
#include "AthenaKernel/errorcheck.h"
#include "TriggerAnalysisHelper.h"



Trig::TriggerAnalysisHelper::TriggerAnalysisHelper( const std::string& type,
        const std::string& name,
        const IInterface* parent )
    : AthAlgTool(type, name, parent),
    m_trigDecisionTool( "Trig::TrigDecisionTool/TrigDecisionTool" ),
    m_matchTool( "Trig::MatchingTool/MatchingTool"){
        declareInterface<TriggerAnalysisHelper>(this);
    }

StatusCode Trig::TriggerAnalysisHelper::initialize(){
    ATH_MSG_INFO("Initialising TriggerAnalysisHelper");
    CHECK(m_trigDecisionTool.retrieve());
    return StatusCode::SUCCESS;
}

/*bool Trig::TriggerAnalysisHelper::passL1(const xAOD::IParticle &recoObj, const std::string chain) {
    return true;
}
bool Trig::TriggerAnalysisHelper::passHLT(const xAOD::IParticle &recoObj, const std::string chain){
    return true;
} 
bool Trig::TriggerAnalysisHelper::passHLT(const xAOD::IParticle &recoObj, std::vector<std::string> chainList){

    return true;
} */
