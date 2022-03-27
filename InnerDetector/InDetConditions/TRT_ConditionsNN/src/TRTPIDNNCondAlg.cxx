/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTPIDNNCondAlg.h"
#include "CoolKernel/IObject.h"

TRTPIDNNCondAlg::TRTPIDNNCondAlg(const std::string& name
				 , ISvcLocator* pSvcLocator )
  : ::AthAlgorithm(name,pSvcLocator)
{}
TRTPIDNNCondAlg::~TRTPIDNNCondAlg(){}

StatusCode TRTPIDNNCondAlg::initialize()
{
  // Read key
  ATH_CHECK( m_readKey.initialize() );

  // Register write handle
  ATH_CHECK( m_writeKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode TRTPIDNNCondAlg::execute()
{
  ATH_MSG_DEBUG("execute " << name());

  // Construct Write Cond Handle and check its validity

  SG::WriteCondHandle<InDet::TRTPIDNN> writeHandle{m_writeKey};

  // Do we have a valid Write Cond Handle for current time?
  if(writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order.");

    return StatusCode::SUCCESS; 
  }

  std::unique_ptr<InDet::TRTPIDNN> writeCdo{std::make_unique<InDet::TRTPIDNN>()};
  
  // Read the NN configuration from the ReadHandle
  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey};
  const CondAttrListCollection* attrListColl{*readHandle};
  if (attrListColl==nullptr or attrListColl->size() != 1) {
    ATH_MSG_ERROR(" Problem reading /TRT/Calib/PID_NN cond object");
    return StatusCode::FAILURE;
  }
  // The NN configuration is stored as one big string in the first channel
  const coral::AttributeList& attrList = attrListColl->begin()->second;
  const std::string json = attrList["NN_config"].data<cool::String16M>();
  if (StatusCode::SUCCESS != writeCdo->configure(json)) {
    ATH_MSG_ERROR ("Problem setting up TRTPIDNN.");
    return StatusCode::FAILURE;     
  }
 
  // Assign range of writeCdo to that of the ReadHandle 
  EventIDRange rangeW;

  if(!readHandle.range(rangeW)) {
        ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle.key());
        return StatusCode::FAILURE;
  }

  // Record CDO
 if(writeHandle.record(rangeW,std::move(writeCdo)).isFailure()) {
    ATH_MSG_ERROR("Could not record InDet::TRTPIDNN " << writeHandle.key() 
		  << " with EventRange " << rangeW
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("Recorded InDet::TRTPIDNN " << writeHandle.key() 
		  << " with EventRange " << rangeW
		  << " into Conditions Store");


  return StatusCode::SUCCESS;
}
