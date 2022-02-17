/*
 *  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

#include "./LArXTalkWeightGlobalCondAlg.h"
#include "AthenaKernel/IOVInfiniteRange.h"

LArXTalkWeightGlobalCondAlg::LArXTalkWeightGlobalCondAlg(const std::string& name, ISvcLocator* pSvcLocator) :
	AthReentrantAlgorithm(name,pSvcLocator){};


StatusCode LArXTalkWeightGlobalCondAlg::initialize(){
   ATH_MSG_DEBUG("LArXTalkWeightGlobalCondAlg::initialize");
    ATH_CHECK(m_xtalkKey.initialize() );
    ATH_CHECK(m_xtstripKey.initialize() );
    ATH_CHECK(m_xtstrip_ecKey.initialize() );
    ATH_CHECK(m_xtmiddlebackKey.initialize() );
    ATH_CHECK(m_xtmiddleback_ecowKey.initialize() );
    ATH_CHECK(m_xtmiddleback_eciwKey.initialize() );
    ATH_CHECK(m_xtstripmiddleKey.initialize() );
    ATH_CHECK(m_xtstripmiddle_ecKey.initialize() );
    ATH_CHECK(m_xt2stripKey.initialize() );
    ATH_CHECK(m_xt2strip_ecKey.initialize() );
    ATH_CHECK(m_xtmiddle1Key.initialize() );
    ATH_CHECK(m_xtmiddle2Key.initialize() );
    ATH_CHECK(m_xtmiddle1_ecKey.initialize() );
    ATH_CHECK(m_xtmiddle2_ecKey.initialize() );
   return StatusCode::SUCCESS;
}

StatusCode
LArXTalkWeightGlobalCondAlg::execute(const EventContext& ctx) const {

    ATH_MSG_DEBUG("LArXTalkWeightGlobalCondAlg::execute");

    SG::WriteCondHandle<LArXTalkWeightGlobal> writeHandle{m_xtalkKey, ctx};
    if ( writeHandle.isValid() ) {
        ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
        return StatusCode::SUCCESS;
    } // otherwise, build the information

    //load everybody
    std::vector<const LArXTalkWeight*> weights;
    weights.push_back(pointerFromKey(ctx,m_xtstripKey));           //0
    weights.push_back(pointerFromKey(ctx,m_xtstrip_ecKey));        //1
    weights.push_back(pointerFromKey(ctx,m_xtmiddlebackKey));      //2
    weights.push_back(pointerFromKey(ctx,m_xtmiddleback_ecowKey)); //3
    weights.push_back(pointerFromKey(ctx,m_xtmiddleback_eciwKey)); //4
    weights.push_back(pointerFromKey(ctx,m_xtstripmiddleKey));     //5
    weights.push_back(pointerFromKey(ctx,m_xtstripmiddle_ecKey));  //6
    weights.push_back(pointerFromKey(ctx,m_xt2stripKey));          //7
    weights.push_back(pointerFromKey(ctx,m_xt2strip_ecKey));       //8
    weights.push_back(pointerFromKey(ctx,m_xtmiddle1Key));         //9
    weights.push_back(pointerFromKey(ctx,m_xtmiddle2Key));         //10
    weights.push_back(pointerFromKey(ctx,m_xtmiddle1_ecKey));      //11
    weights.push_back(pointerFromKey(ctx,m_xtmiddle2_ecKey));      //12

    std::unique_ptr<LArXTalkWeightGlobal> xtalk = std::make_unique<LArXTalkWeightGlobal>(weights);
    writeHandle.addDependency(EventIDRange(IOVInfiniteRange::infiniteRunLB()));

    if(writeHandle.record(std::move(xtalk)).isFailure()) {
	ATH_MSG_ERROR("Could not record LArXTalk object with " 
			<< writeHandle.key()
                        << " with EventRange " << writeHandle.getRange()
			<< " into Conditions Store");
        return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("recorded new " << writeHandle.key() << " with range " 
                   << writeHandle.getRange() << " into Conditions Store");
    return StatusCode::SUCCESS;

}

