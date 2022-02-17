/*
 *  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

#include "./LArXTalkWeightCondAlg.h"
#include "AthenaKernel/IOVInfiniteRange.h"
#include "LArRecUtils/LArXTalkWeightConstants.h"

StatusCode LArXTalkWeightCondAlg::initialize(){
   ATH_MSG_DEBUG("LArXTalkWeightCondAlg::initialize");
   ATH_CHECK(m_xtalkKey.initialize() );
   return StatusCode::SUCCESS;
}

StatusCode
LArXTalkWeightCondAlg::execute(const EventContext& ctx) const {

    ATH_MSG_DEBUG("LArXTalkWeightCondAlg::execute");

    SG::WriteCondHandle<LArXTalkWeight> writeHandle{m_xtalkKey, ctx};
    if ( writeHandle.isValid() ) {
        ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
        return StatusCode::SUCCESS;
    }
    // just to handle a special case
    std::vector<int> endcap_case_table;
    if(!((m_endcap_case_table.size()==1)&&(m_endcap_case_table[0]==0)) ) {
    endcap_case_table.insert(endcap_case_table.end(),
	m_endcap_case_table.begin(),
	m_endcap_case_table.end());
    }
    std::unique_ptr<LArXTalkWeight> xtalk = std::make_unique<LArXTalkWeight>(m_xtalk_to_inject, m_xtalk_factor,endcap_case_table);
    writeHandle.addDependency(EventIDRange(IOVInfiniteRange::infiniteRunLB()));
    if(writeHandle.record(std::move(xtalk)).isFailure()) {
	ATH_MSG_ERROR("Could not record LArXTalk object with " 
			<< writeHandle.key()
                        << " with EventRange " << writeHandle.getRange()
			<< " into Conditions Store");
        return StatusCode::FAILURE;
    }

    ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " 
                   << writeHandle.getRange() << " into Conditions Store");
    return StatusCode::SUCCESS;

}

LArXTalkWeightCondAlg::LArXTalkWeightCondAlg(const std::string& name, ISvcLocator* pSvcLocator) :
	AthReentrantAlgorithm(name,pSvcLocator){};

LArXTalkWeightCondAlg_strip::LArXTalkWeightCondAlg_strip(const std::string& name,ISvcLocator* pSvcLocator) : LArXTalkWeightCondAlg(name,pSvcLocator){
    m_xtalk_to_inject.insert(m_xtalk_to_inject.end(),std::begin(LArXTalkWeightConstants::xtalk_strip),std::end(LArXTalkWeightConstants::xtalk_strip));
    m_endcap_case_table.insert(m_endcap_case_table.end(),std::begin(LArXTalkWeightConstants::table_noregion),std::end(LArXTalkWeightConstants::table_noregion));
    m_xtalk_factor=LArXTalkWeightConstants::factor_strip;
    m_xtalkKey = std::string("LArXTalkWeight_strip");
};

LArXTalkWeightCondAlg_strip_ec::LArXTalkWeightCondAlg_strip_ec(const std::string& name,ISvcLocator* pSvcLocator) : LArXTalkWeightCondAlg(name,pSvcLocator){
    m_xtalk_to_inject.insert(m_xtalk_to_inject.end(),std::begin(LArXTalkWeightConstants::xtalk_strip_ec),std::end(LArXTalkWeightConstants::xtalk_strip_ec));
    m_endcap_case_table.insert(m_endcap_case_table.end(),std::begin(LArXTalkWeightConstants::table_withregion),std::end(LArXTalkWeightConstants::table_withregion));
    m_xtalk_factor=LArXTalkWeightConstants::factor_strip_ec;
    m_xtalkKey = std::string("LArXTalkWeight_strip_ec");
};

LArXTalkWeightCondAlg_middleback::LArXTalkWeightCondAlg_middleback(const std::string& name,ISvcLocator* pSvcLocator) : LArXTalkWeightCondAlg(name,pSvcLocator){
    m_xtalk_to_inject.insert(m_xtalk_to_inject.end(),std::begin(LArXTalkWeightConstants::xtalk_middleback),std::end(LArXTalkWeightConstants::xtalk_middleback));
    m_endcap_case_table.insert(m_endcap_case_table.end(),std::begin(LArXTalkWeightConstants::table_noregion),std::end(LArXTalkWeightConstants::table_noregion));
    m_xtalk_factor=LArXTalkWeightConstants::factor_middleback;
    m_xtalkKey = std::string("LArXTalkWeight_middleback");
};

LArXTalkWeightCondAlg_middleback_ecow::LArXTalkWeightCondAlg_middleback_ecow(const std::string& name,ISvcLocator* pSvcLocator) : LArXTalkWeightCondAlg(name,pSvcLocator){
    m_xtalk_to_inject.insert(m_xtalk_to_inject.end(),std::begin(LArXTalkWeightConstants::xtalk_middleback_ecow),std::end(LArXTalkWeightConstants::xtalk_middleback_ecow));
    m_endcap_case_table.insert(m_endcap_case_table.end(),std::begin(LArXTalkWeightConstants::table_noregion),std::end(LArXTalkWeightConstants::table_noregion));
    m_xtalk_factor=LArXTalkWeightConstants::factor_middleback_ecow;
    m_xtalkKey = std::string("LArXTalkWeight_middleback_ecow");
};

LArXTalkWeightCondAlg_middleback_eciw::LArXTalkWeightCondAlg_middleback_eciw(const std::string& name,ISvcLocator* pSvcLocator) : LArXTalkWeightCondAlg(name,pSvcLocator){
    m_xtalk_to_inject.insert(m_xtalk_to_inject.end(),std::begin(LArXTalkWeightConstants::xtalk_middleback_eciw),std::end(LArXTalkWeightConstants::xtalk_middleback_eciw));
    m_endcap_case_table.insert(m_endcap_case_table.end(),std::begin(LArXTalkWeightConstants::table_noregion),std::end(LArXTalkWeightConstants::table_noregion));
    m_xtalk_factor=LArXTalkWeightConstants::factor_middleback_eciw;
    m_xtalkKey = std::string("LArXTalkWeight_middleback_eciw");
};

LArXTalkWeightCondAlg_stripmiddle::LArXTalkWeightCondAlg_stripmiddle(const std::string& name,ISvcLocator* pSvcLocator) : LArXTalkWeightCondAlg(name,pSvcLocator){
    m_xtalk_to_inject.insert(m_xtalk_to_inject.end(),std::begin(LArXTalkWeightConstants::xtalk_stripmiddle),std::end(LArXTalkWeightConstants::xtalk_stripmiddle));
    m_endcap_case_table.insert(m_endcap_case_table.end(),std::begin(LArXTalkWeightConstants::table_noregion),std::end(LArXTalkWeightConstants::table_noregion));
    m_xtalk_factor=LArXTalkWeightConstants::factor_stripmiddle;
    m_xtalkKey = std::string("LArXTalkWeight_stripmiddle");
};

LArXTalkWeightCondAlg_stripmiddle_ec::LArXTalkWeightCondAlg_stripmiddle_ec(const std::string& name,ISvcLocator* pSvcLocator) : LArXTalkWeightCondAlg(name,pSvcLocator){
    m_xtalk_to_inject.insert(m_xtalk_to_inject.end(),std::begin(LArXTalkWeightConstants::xtalk_stripmiddle_ec),std::end(LArXTalkWeightConstants::xtalk_stripmiddle_ec));
    m_endcap_case_table.insert(m_endcap_case_table.end(),std::begin(LArXTalkWeightConstants::table_withregion),std::end(LArXTalkWeightConstants::table_withregion));
    m_xtalk_factor=LArXTalkWeightConstants::factor_stripmiddle_ec;
    m_xtalkKey = std::string("LArXTalkWeight_stripmiddle_ec");
};

LArXTalkWeightCondAlg_2strip::LArXTalkWeightCondAlg_2strip(const std::string& name,ISvcLocator* pSvcLocator) : LArXTalkWeightCondAlg(name,pSvcLocator){
    for(int i=0;i<380;i++)  m_xtalk_to_inject.push_back(0.004);
    for(int i=0;i<68;i++)   m_xtalk_to_inject.push_back(0.005);
    m_endcap_case_table.insert(m_endcap_case_table.end(),std::begin(LArXTalkWeightConstants::table_noregion),std::end(LArXTalkWeightConstants::table_noregion));
    m_xtalk_factor=LArXTalkWeightConstants::factor_2strip;
    m_xtalkKey = std::string("LArXTalkWeight_2strip");
};

LArXTalkWeightCondAlg_2strip_ec::LArXTalkWeightCondAlg_2strip_ec(const std::string& name,ISvcLocator* pSvcLocator) : LArXTalkWeightCondAlg(name,pSvcLocator){
    for(int i=0;i<40;i++)  m_xtalk_to_inject.push_back(0.004);
    for(int i=0;i<110;i++)   m_xtalk_to_inject.push_back(0.0025);
    for(int i=0;i<298;i++)   m_xtalk_to_inject.push_back(0.004);
    m_endcap_case_table.insert(m_endcap_case_table.end(),std::begin(LArXTalkWeightConstants::table_withregion),std::end(LArXTalkWeightConstants::table_withregion));
    m_xtalk_factor=LArXTalkWeightConstants::factor_2strip_ec;
    m_xtalkKey = std::string("LArXTalkWeight_2strip_ec");
};

LArXTalkWeightCondAlg_middle1::LArXTalkWeightCondAlg_middle1(const std::string& name,ISvcLocator* pSvcLocator) : LArXTalkWeightCondAlg(name,pSvcLocator){
    m_xtalk_to_inject.insert(m_xtalk_to_inject.end(),std::begin(LArXTalkWeightConstants::xtalk_middle1),std::end(LArXTalkWeightConstants::xtalk_middle1));
    m_endcap_case_table.insert(m_endcap_case_table.end(),std::begin(LArXTalkWeightConstants::table_noregion),std::end(LArXTalkWeightConstants::table_noregion));
    m_xtalk_factor=LArXTalkWeightConstants::factor_middle1;
    m_xtalkKey = std::string("LArXTalkWeight_middle1");
};

LArXTalkWeightCondAlg_middle2::LArXTalkWeightCondAlg_middle2(const std::string& name,ISvcLocator* pSvcLocator) : LArXTalkWeightCondAlg(name,pSvcLocator){
    m_xtalk_to_inject.insert(m_xtalk_to_inject.end(),std::begin(LArXTalkWeightConstants::xtalk_middle2),std::end(LArXTalkWeightConstants::xtalk_middle2));
    m_endcap_case_table.insert(m_endcap_case_table.end(),std::begin(LArXTalkWeightConstants::table_noregion),std::end(LArXTalkWeightConstants::table_noregion));
    m_xtalk_factor=LArXTalkWeightConstants::factor_middle2;
    m_xtalkKey = std::string("LArXTalkWeight_middle2");
};

LArXTalkWeightCondAlg_middle1_ec::LArXTalkWeightCondAlg_middle1_ec(const std::string& name,ISvcLocator* pSvcLocator) : LArXTalkWeightCondAlg(name,pSvcLocator){
    m_xtalk_to_inject.insert(m_xtalk_to_inject.end(),std::begin(LArXTalkWeightConstants::xtalk_middle1_ec),std::end(LArXTalkWeightConstants::xtalk_middle1_ec));
    m_endcap_case_table.insert(m_endcap_case_table.end(),std::begin(LArXTalkWeightConstants::table_noregion),std::end(LArXTalkWeightConstants::table_noregion));
    m_xtalk_factor=LArXTalkWeightConstants::factor_middle1_ec;
    m_xtalkKey = std::string("LArXTalkWeight_middle1_ec");
};

LArXTalkWeightCondAlg_middle2_ec::LArXTalkWeightCondAlg_middle2_ec(const std::string& name,ISvcLocator* pSvcLocator) : LArXTalkWeightCondAlg(name,pSvcLocator){
    m_xtalk_to_inject.insert(m_xtalk_to_inject.end(),std::begin(LArXTalkWeightConstants::xtalk_middle2_ec),std::end(LArXTalkWeightConstants::xtalk_middle2_ec));
    m_endcap_case_table.insert(m_endcap_case_table.end(),std::begin(LArXTalkWeightConstants::table_noregion),std::end(LArXTalkWeightConstants::table_noregion));
    m_xtalk_factor=LArXTalkWeightConstants::factor_middle2_ec;
    m_xtalkKey = std::string("LArXTalkWeight_middle2_ec");
};

