/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTPhaseCondAlg.h"
#include "TRT_ConditionsData/RtRelation.h"
#include "TRT_ConditionsData/BasicRtRelation.h"
#include "InDetIdentifier/TRT_ID.h"

TRTPhaseCondAlg::TRTPhaseCondAlg(const std::string& name
				 , ISvcLocator* pSvcLocator )
  : ::AthAlgorithm(name,pSvcLocator),
    m_caldbtool("TRT_CalDbTool",this),
    m_trtId(nullptr)
{ declareProperty("TRTCalDbTool",m_caldbtool); }

TRTPhaseCondAlg::~TRTPhaseCondAlg(){}

StatusCode TRTPhaseCondAlg::initialize()
{
  // Straw status
  ATH_CHECK ( m_caldbtool.retrieve() );

  // Read key
  ATH_CHECK( m_T0ReadKey.initialize() );

  // Register write handle
  ATH_CHECK( m_T0WriteKey.initialize() );

  // TRT ID helper
  ATH_CHECK(detStore()->retrieve(m_trtId,"TRT_ID"));

  return StatusCode::SUCCESS;
}

StatusCode TRTPhaseCondAlg::execute() 
{
  ATH_MSG_DEBUG("execute " << name());

  // ____________ Construct Write Cond Handle and check its validity ____________

  SG::WriteCondHandle<TRTCond::AverageT0> writeHandle{m_T0WriteKey};

  // Do we have a valid Write Cond Handle for current time?
  if(writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order.");

    return StatusCode::SUCCESS; 
  }



  // ____________ Construct new Write Cond Object  ____________
  std::unique_ptr<TRTCond::AverageT0> writeCdo{std::make_unique<TRTCond::AverageT0>()};
  

  // ____________ Average T0 for Write Cond object (code adapted from InDetCosmicsEventPhaseTool::beginRun()) _____
  int countAverageT0 = 0;
  double rtShift = 0.;
  double averageT0 = 0.;

  for (std::vector<Identifier>::const_iterator it = m_trtId->straw_layer_begin(); it != m_trtId->straw_layer_end(); ++it  ) {

    int nStrawsInLayer = m_trtId->straw_max(*it);

    for (int i = 0; i <= nStrawsInLayer; i++) {
      Identifier id = m_trtId->straw_id(*it, i);
      if (std::abs(m_trtId->barrel_ec(id)) != 1) continue; // average over barrel only
      averageT0 += m_caldbtool->getT0(id);
      countAverageT0++;
      const TRTCond::RtRelation* rtRelation = m_caldbtool->getRtRelation(id);
      if (!rtRelation) {
        ATH_MSG_DEBUG("rtRelation missing for straw ");
        continue;
      }
      rtShift += rtRelation->drifttime(0.);
    }
  }
  if (countAverageT0 != 0){
    averageT0 /= double(countAverageT0);
  } else {
    averageT0 = 0.;
  }

  if (countAverageT0 != 0) {
    rtShift /= double(countAverageT0);
  } else {
    rtShift = 0.;
  }

  double evtPhaseT0 = averageT0-20.+rtShift; // (Correction to be checked /PH)

  ATH_MSG_INFO(" Subtracting: " << evtPhaseT0 << " ns (average T0: " << averageT0 << " ns, average t(r=0): " << rtShift << " ns )");

  writeCdo->update(evtPhaseT0);

  //__________ Assign range of writeCdo to that of the ReadHandle___________ 
  EventIDRange rangeW;

    SG::ReadCondHandle<StrawT0Container> T0ReadHandle{m_T0ReadKey};
    const StrawT0Container* T0Container{*T0ReadHandle};
    if(T0Container==nullptr) {
        ATH_MSG_ERROR("Null pointer to the straw T0 container");
        return StatusCode::FAILURE;
    }

    // Get range
    if(!T0ReadHandle.range(rangeW)) {
        ATH_MSG_ERROR("Failed to retrieve validity range for " << T0ReadHandle.key());
        return StatusCode::FAILURE;
    }


  // Record  CDO
 if(writeHandle.record(rangeW,std::move(writeCdo)).isFailure()) {
    ATH_MSG_ERROR("Could not record AverageT0 " << writeHandle.key() 
		  << " with EventRange " << rangeW
		  << " into Conditions Store");
    return StatusCode::FAILURE;
 } else {
    ATH_MSG_INFO(" Recorded AverageT0 " << writeHandle.key() 
		  << " with EventRange " << rangeW
		  << " into Conditions Store");

 }
  return StatusCode::SUCCESS;
}

StatusCode TRTPhaseCondAlg::finalize()
{
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}


