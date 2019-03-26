/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTHTCondAlg.h"

TRTHTCondAlg::TRTHTCondAlg(const std::string& name
				 , ISvcLocator* pSvcLocator )
  : ::AthReentrantAlgorithm(name,pSvcLocator),
    m_condSvc("CondSvc",name)
{}
TRTHTCondAlg::~TRTHTCondAlg(){}

StatusCode TRTHTCondAlg::initialize()
{

  // CondSvc
  ATH_CHECK( m_condSvc.retrieve() );

  // Read key
  ATH_CHECK( m_ReadKey.initialize() );

  // Register write handle
  ATH_CHECK( m_WriteKey.initialize() );

  if (m_condSvc->regHandle(this, m_WriteKey).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_WriteKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode TRTHTCondAlg::execute(const EventContext& ctx) const 
{
  ATH_MSG_DEBUG("execute " << name());

  // ____________ Construct Write Cond Handle and check its validity ____________

  SG::WriteCondHandle<HTcalculator> writeHandle{m_WriteKey,ctx};

  // Do we have a valid Write Cond Handle for current time?
  if(writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order.");

    return StatusCode::SUCCESS; 
  }



  // ____________ Construct new Write Cond Object  ____________
  std::unique_ptr<HTcalculator> writeCdo{std::make_unique<HTcalculator>()};
  

  // ____________ Compute the array structures for the HTcalculator object  ____________
  SG::ReadCondHandle<CondAttrListVec> readHandle{m_ReadKey,ctx};
  const CondAttrListVec* channel_values{*readHandle};
  if(channel_values==nullptr) {
      ATH_MSG_ERROR(" Problem reading TRT/Calib/PID_vector cond object");
      return StatusCode::FAILURE;
  }
  if(StatusCode::SUCCESS != writeCdo->ReadVectorDB( channel_values  )) {
     ATH_MSG_ERROR ("Problem filling HT Calculator.");
     return StatusCode::FAILURE;     
  }

 
  //__________ Assign range of writeCdo to that of the ReadHandle___________ 
  EventIDRange rangeW;

  if(!readHandle.range(rangeW)) {
        ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle.key());
        return StatusCode::FAILURE;
  }

  // Record  CDO
 if(writeHandle.record(rangeW,std::move(writeCdo)).isFailure()) {
    ATH_MSG_ERROR("Could not record HTCalculator " << writeHandle.key() 
		  << " with EventRange " << rangeW
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
}

StatusCode TRTHTCondAlg::finalize()
{
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}
