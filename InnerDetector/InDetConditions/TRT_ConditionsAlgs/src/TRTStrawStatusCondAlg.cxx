/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTStrawStatusCondAlg.h"
#include "InDetIdentifier/TRT_ID.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"

TRTStrawStatusCondAlg::TRTStrawStatusCondAlg(const std::string& name
				 , ISvcLocator* pSvcLocator )
  : ::AthAlgorithm(name,pSvcLocator)
{ 
}

StatusCode TRTStrawStatusCondAlg::initialize()
{

  // Read keys
  ATH_CHECK( m_strawReadKey.initialize() );
  ATH_CHECK( m_strawPermReadKey.initialize() );

  // Register write handles
  ATH_CHECK (m_strawStatusWriteKey.initialize());
  ATH_CHECK (m_strawStatusPermWriteKey.initialize());

  // TRT ID helper
  ATH_CHECK(detStore()->retrieve(m_trtId,"TRT_ID"));

  return StatusCode::SUCCESS;
}

StatusCode TRTStrawStatusCondAlg::execute() 
{
  ATH_MSG_DEBUG("execute " << name());
  
  EventIDRange rangeV;
  
  SG::WriteHandle<TRTStrawStatusData> strawContainer{m_strawStatusWriteKey};
  std::unique_ptr<TRTStrawStatusData> outputStrawStat{std::make_unique<TRTStrawStatusData>()};

  SG::WriteHandle<TRTStrawStatusData> strawPermContainer{m_strawStatusPermWriteKey};
  std::unique_ptr<TRTStrawStatusData> outputStrawStatPerm{std::make_unique<TRTStrawStatusData>()};
  

  
  SG::ReadCondHandle<StrawStatusContainer> strawReadHandle{m_strawReadKey};
  if (!strawReadHandle.isValid()){
    ATH_MSG_FATAL("No access to conditions " << strawReadHandle.key());
    return StatusCode::FAILURE;
  }
  if (!strawReadHandle.range(rangeV)){
    ATH_MSG_ERROR("Failed to get validity range of " << strawReadHandle.key());
    return StatusCode::FAILURE;
  }
  
  ATH_MSG_DEBUG("Retrieved " << strawReadHandle.key() << " with validity " << rangeV);

  
  SG::ReadCondHandle<StrawStatusContainer> strawPermReadHandle{m_strawPermReadKey};
  if (!strawPermReadHandle.isValid()){
    ATH_MSG_FATAL("No access to conditions " << strawPermReadHandle.key());
    return StatusCode::FAILURE;
  }
  
  if (!strawPermReadHandle.range(rangeV)){
    ATH_MSG_ERROR("Failed to get validity range of " << strawReadHandle.key());
    return StatusCode::FAILURE;
  }

  const StrawStatusContainer* statC = {*strawReadHandle};
  if(statC==nullptr) {
    ATH_MSG_ERROR("Null pointer to the straw status container");
    return StatusCode::FAILURE;
  }

  const StrawStatusContainer* statPermC = {*strawPermReadHandle};
  if(statPermC==nullptr) {
    ATH_MSG_ERROR("Null pointer to the straw permanent status container");
    return StatusCode::FAILURE;
  }

  // iterate over straws and fill non-zero statuses to TRTStrawStatusData
  
  for (std::vector<Identifier>::const_iterator it = m_trtId->straw_layer_begin(); it != m_trtId->straw_layer_end(); ++it ) {
    
    unsigned int nstraws = 0;
    nstraws = m_trtId->straw_max( *it) + 1; // There is a difference of 1 between both methods....

    constexpr int level = TRTCond::ExpandedIdentifier::STRAW ;

    for (unsigned int i=0; i<nstraws  ;i++) {
      Identifier offlineID = m_trtId->straw_id( *it, i);
      int det = m_trtId->barrel_ec(         offlineID)     ;
      int lay = m_trtId->layer_or_wheel(    offlineID)     ;
      int phi = m_trtId->phi_module(        offlineID)     ;
      TRTCond::ExpandedIdentifier expID=  TRTCond::ExpandedIdentifier( m_trtId->barrel_ec(offlineID),m_trtId->layer_or_wheel(offlineID),
								    m_trtId->phi_module(offlineID),m_trtId->straw_layer(offlineID),
								    m_trtId->straw(offlineID),level );

      int status = statC->get(expID).getstatus();
      const IdentifierHash hashId = m_trtId->straw_hash(offlineID);
      outputStrawStat->setStatus(hashId, status);	

      int statusPerm = statPermC->get(expID).getstatus();
      outputStrawStatPerm->setStatus(hashId, statusPerm);

      if (status or statusPerm){
	ATH_MSG_VERBOSE(" The sector " << det << " " << lay << " " << phi << " has status " << status << " and permanent " << statusPerm);
      }
    }
  }    

  if (strawContainer.record(std::move(outputStrawStat)).isFailure()){
     ATH_MSG_ERROR("Could not record " << strawContainer.key());
  }

  if (strawPermContainer.record(std::move(outputStrawStatPerm)).isFailure()){
     ATH_MSG_ERROR("Could not record " << strawPermContainer.key());
  }

  ServiceHandle<IIncidentSvc> p_incSvc("IncidentSvc", this->name());
  p_incSvc->fireIncident(Incident("TRTStrawCalib","TRTStrawCalib"));
  
  return StatusCode::SUCCESS;

}

StatusCode TRTStrawStatusCondAlg::finalize()
{
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

