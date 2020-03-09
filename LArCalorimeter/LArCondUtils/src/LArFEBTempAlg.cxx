/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArFEBTempAlg.h" 
#include "LArIdentifier/LArOnlineID.h"

LArFEBTempAlg::LArFEBTempAlg(const std::string& name, ISvcLocator* pSvcLocator):AthReentrantAlgorithm(name,pSvcLocator){}

// intialize 
StatusCode LArFEBTempAlg::initialize()
{
  ATH_CHECK(m_foldernameKey.initialize());
  ATH_CHECK(m_tempKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode LArFEBTempAlg::execute(const EventContext& ctx ) const 
{
  SG::WriteCondHandle<LArFEBTempData> writeHandle{m_tempKey,ctx};
  
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("Found valid write handle");
    return StatusCode::SUCCESS;
  }  

  SG::ReadCondHandle<CondAttrListCollection> cHdl(m_foldernameKey, ctx);
  const CondAttrListCollection* cattr = *cHdl;
  if(!cattr) {
     ATH_MSG_ERROR("Why do not have FEB TEMP folder ?" << m_foldernameKey.fullKey());
     return StatusCode::FAILURE;
  }
  EventIDRange rangeIn;
  if(!cHdl.range(rangeIn)) {
          ATH_MSG_ERROR("Failed to retrieve validity range for " << cHdl.key());
          return StatusCode::FAILURE;
  }
  // Fill LArFEBTempData
  std::unique_ptr<LArFEBTempData> febTemp=std::make_unique<LArFEBTempData>();
  LArFEBTempData* p_febTemp = febTemp.get();

  for (CondAttrListCollection::const_iterator citr=cattr->begin();
	citr!=cattr->end();++citr) {
	     float temp1=-1;
             if(! ((*citr).second)["temp1"].isNull()) temp1 = (((*citr).second)["temp1"]).data<float>();
	     float temp2 =-1;
             if(! ((*citr).second)["temp2"].isNull()) temp2 = (((*citr).second)["temp2"]).data<float>();
             LArFEBTempData::FEBTemp temp = std::make_pair(temp1,temp2);

             p_febTemp->m_mapVec[HWIdentifier(Identifier32((*citr).first))] = temp;
  }

  const EventIDRange crangeW(rangeIn);
  if(writeHandle.record(crangeW,febTemp.release()).isFailure()) {
     ATH_MSG_ERROR("Could not record LArFebConfig object with " << writeHandle.key()
                      << " with EventRange " << crangeW << " into Conditions Store");
     return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << crangeW << " into Conditions Store");

  return StatusCode::SUCCESS;

}


