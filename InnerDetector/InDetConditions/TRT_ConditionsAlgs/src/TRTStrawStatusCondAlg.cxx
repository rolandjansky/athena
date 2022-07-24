/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTStrawStatusCondAlg.h"

#include "InDetIdentifier/TRT_ID.h"

TRTStrawStatusCondAlg::TRTStrawStatusCondAlg(const std::string &name, ISvcLocator *pSvcLocator)
  : ::AthReentrantAlgorithm(name,pSvcLocator)
{
}

StatusCode TRTStrawStatusCondAlg::initialize()
{
  // Read keys
  ATH_CHECK( m_strawReadKey.initialize() );
  ATH_CHECK( m_strawPermReadKey.initialize() );
  ATH_CHECK( m_strawHTReadKey.initialize() );

  // Register write handles
  ATH_CHECK (m_strawStatusWriteKey.initialize());
  ATH_CHECK (m_strawStatusPermWriteKey.initialize());
  ATH_CHECK (m_strawStatusHTWriteKey.initialize());
  ATH_CHECK (m_strawStatusSummaryWriteKey.initialize());
  ATH_CHECK (m_strawStatusHTSummaryWriteKey.initialize());

  // TRT ID helper
  ATH_CHECK(detStore()->retrieve(m_trtId, "TRT_ID"));

  ATH_MSG_INFO("Max number of TRT hashes: " << m_trtId->straw_hash_max());

  return StatusCode::SUCCESS;
}

StatusCode TRTStrawStatusCondAlg::execute(const EventContext &ctx) const
{
  ATH_MSG_DEBUG("execute " << name());

  SG::WriteCondHandle<TRTCond::StrawStatusData> strawStatusHandle{m_strawStatusWriteKey, ctx};
  SG::WriteCondHandle<TRTCond::StrawStatusData> strawStatusPermHandle{m_strawStatusPermWriteKey, ctx};
  SG::WriteCondHandle<TRTCond::StrawStatusData> strawStatusHTHandle{m_strawStatusHTWriteKey, ctx};
  SG::WriteCondHandle<TRTCond::StrawStatusSummary> strawStatusSummaryHandle{m_strawStatusSummaryWriteKey, ctx};
  SG::WriteCondHandle<TRTCond::StrawStatusSummary> strawStatusHTSummaryHandle{m_strawStatusHTSummaryWriteKey, ctx};

  bool alreadyPresent = strawStatusHandle.isValid();
  alreadyPresent &= strawStatusPermHandle.isValid();
  alreadyPresent &= strawStatusHTHandle.isValid();
  alreadyPresent &= strawStatusSummaryHandle.isValid();
  alreadyPresent &= strawStatusHTSummaryHandle.isValid();
  if(alreadyPresent){
    ATH_MSG_DEBUG("All objects are still valid");
    return StatusCode::SUCCESS;
  }
  auto outputStrawStatusHTSummary = std::make_unique<TRTCond::StrawStatusSummary>(m_trtId->straw_hash_max());
  auto outputStrawStatusSummary = std::make_unique<TRTCond::StrawStatusSummary>(m_trtId->straw_hash_max());
  auto outputStrawStatusHT = std::make_unique<TRTCond::StrawStatusData>(m_trtId->straw_hash_max());
  auto outputStrawStatusPerm = std::make_unique<TRTCond::StrawStatusData>(m_trtId->straw_hash_max());
  auto outputStrawStatus = std::make_unique<TRTCond::StrawStatusData>(m_trtId->straw_hash_max());
  SG::ReadCondHandle<StrawStatusContainer> strawReadHandle{m_strawReadKey, ctx};
  if (!strawReadHandle.isValid()) {
    ATH_MSG_FATAL("No access to conditions " << strawReadHandle.key());
    return StatusCode::FAILURE;
  }
  EventIDRange range;
  if (!strawReadHandle.range(range)) {
    ATH_MSG_ERROR("Failed to get validity range of " << strawReadHandle.key());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Retrieved " << strawReadHandle.key() << " with validity " << range);

  SG::ReadCondHandle<StrawStatusContainer> strawPermReadHandle{m_strawPermReadKey, ctx};
  if (!strawPermReadHandle.isValid()) {
    ATH_MSG_FATAL("No access to conditions " << strawPermReadHandle.key());
    return StatusCode::FAILURE;
  }
  EventIDRange rangePerm;
  if (!strawPermReadHandle.range(rangePerm)) {
    ATH_MSG_ERROR("Failed to get validity range of " << strawPermReadHandle.key());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Retrieved " << strawPermReadHandle.key() << " with validity " << rangePerm);

  SG::ReadCondHandle<StrawStatusContainer> strawHTReadHandle{m_strawHTReadKey, ctx};
  if (!strawHTReadHandle.isValid()) {
    ATH_MSG_FATAL("No access to conditions " << strawHTReadHandle.key());
    return StatusCode::FAILURE;
  }
  EventIDRange rangeHT;
  if (!strawHTReadHandle.range(rangeHT)) {
    ATH_MSG_ERROR("Failed to get validity range of " << strawHTReadHandle.key());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Retrieved " << strawHTReadHandle.key() << " with validity " << rangeHT);

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
  const StrawStatusContainer* statHTC = {*strawHTReadHandle};
  if(statHTC==nullptr) {
    ATH_MSG_ERROR("Null pointer to the straw HT status container");
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

      unsigned int status = statC->get(expID).getstatus();
      const IdentifierHash hashId = m_trtId->straw_hash(offlineID);
      outputStrawStatus->setStatus(hashId, status);

      unsigned int statusPerm = statPermC->get(expID).getstatus();
      outputStrawStatusPerm->setStatus(hashId, statusPerm);

      unsigned int statusHT = statHTC->get(expID).getstatus();
      outputStrawStatusHT->setStatus(hashId, statusHT);

      if (status or statusPerm) {
        ATH_MSG_VERBOSE(" The sector " << det << " " << lay << " " << phi << " has status " << status << " and permanent " << statusPerm);
      }

      constexpr unsigned int statusBitmask = 1 << 8; // 0000001 00000000
      bool statusBool{}, statusPermBool{}, statusHTBool{};
      if (status == 1) statusBool = true;
      else if (status == 0) statusBool = false;
      else statusBool = static_cast<bool>((status & statusBitmask) >> 8);

      if (statusPerm == 1) statusPermBool = true;
      else if (statusPerm == 0) statusPermBool = false;
      else statusPermBool = static_cast<bool>((statusPerm & statusBitmask) >> 8);

      if (statusHT == 1) statusHTBool = true;
      else if (statusHT==0) statusHTBool = false;
      else statusHTBool = static_cast<bool>((statusHT & statusBitmask) >> 8);

      outputStrawStatusSummary->setStatus(hashId, statusBool || statusPermBool);
      outputStrawStatusHTSummary->setStatus(hashId, statusHTBool);
    }
  }

  // Record CDOs
  if (strawStatusHandle.record(range, std::move(outputStrawStatus)).isFailure()) {
    ATH_MSG_ERROR("Could not record AliveStraws " << m_strawStatusWriteKey.key() 
                  << " with EventRange " << range
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  }

  if (strawStatusPermHandle.record(rangePerm, std::move(outputStrawStatusPerm)).isFailure()) {
    ATH_MSG_ERROR("Could not record AliveStraws " << m_strawStatusPermWriteKey.key() 
                  << " with EventRange " << rangePerm
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  }

  if (strawStatusHTHandle.record(rangeHT, std::move(outputStrawStatusHT)).isFailure()) {
    ATH_MSG_ERROR("Could not record AliveStraws " << m_strawStatusHTWriteKey.key() 
                  << " with EventRange " << rangeHT
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  }

  EventIDRange rangeCombined = EventIDRange::intersect (range, rangePerm);
  if (strawStatusSummaryHandle.record(rangeCombined, std::move(outputStrawStatusSummary)).isFailure()) {
    ATH_MSG_ERROR("Could not record AliveStraws " << m_strawStatusSummaryWriteKey.key() 
                  << " with EventRange " << rangeCombined
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  }

  if (strawStatusHTSummaryHandle.record(rangeHT, std::move(outputStrawStatusHTSummary)).isFailure()) {
    ATH_MSG_ERROR("Could not record AliveStraws " << m_strawStatusHTSummaryWriteKey.key() 
                  << " with EventRange " << rangeHT
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}
