/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LArRecUtils/src/LArRoIMapCondAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2021
 * @brief Conditions algorithm producing LArRoIMap.
 */


#include "LArRoIMapCondAlg.h"
#include "LArIdentifier/LArOnlineID.h" 
#include "CaloIdentifier/CaloLVL1_ID.h" 
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloTTDetDescr/CaloTTDescriptor.h"
#include "CaloTTDetDescr/CaloTTDescrManager.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandle.h"
#include <fstream>
#include <vector>


/**
 * @brief Gaudi initialize method.
 */
StatusCode LArRoIMapCondAlg::initialize()
{
  ATH_CHECK( detStore()->retrieve(m_onlineID) );
  ATH_CHECK( detStore()->retrieve(m_TT_ID) );
  ATH_CHECK( detStore()->retrieve(m_em_id) );
  ATH_CHECK( detStore()->retrieve(m_hec_id) );
  ATH_CHECK( detStore()->retrieve(m_fcal_id) );

  ATH_CHECK( m_ttSvc.retrieve() );

  ATH_CHECK( m_onOffIdKey.initialize() );
  ATH_CHECK( m_febRodKey.initialize() );
  ATH_CHECK( m_LArRoIMapKey.initialize() );

  return StatusCode::SUCCESS;
}


/**
 * @brief Gaudi execute method.
 */
StatusCode LArRoIMapCondAlg::execute (const EventContext& ctx) const
{
  SG::WriteCondHandle<LArRoIMap> writeHandle (m_LArRoIMapKey, ctx);
  if (writeHandle.isValid()) {
    return StatusCode::SUCCESS;
  }
  auto larRoIMap = std::make_unique<LArRoIMap> (100);

  SG::ReadCondHandle<LArOnOffIdMapping> onOffId (m_onOffIdKey, ctx);
  SG::ReadCondHandle<LArFebRodMapping> febRod (m_febRodKey, ctx);

  for (const Identifier& id : m_em_id->channel_ids()) {
    ATH_CHECK( addEntry (*larRoIMap, **onOffId, **febRod, id) );
  }

  for (const Identifier& id : m_hec_id->channel_ids()) {
    ATH_CHECK( addEntry (*larRoIMap, **onOffId, **febRod, id) );
  }

  for (const Identifier& id : m_fcal_id->channel_ids()) {
    ATH_CHECK( addEntry (*larRoIMap, **onOffId, **febRod, id) );
  }

  writeHandle.addDependency (onOffId);
  writeHandle.addDependency (febRod);
  ATH_CHECK( writeHandle.record (std::move (larRoIMap)) );
  ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << writeHandle.getRange() << " into Conditions Store");
  return StatusCode::SUCCESS;
}


StatusCode LArRoIMapCondAlg::addEntry (LArRoIMap& m,
                                       const LArOnOffIdMapping& onOffId,
                                       const LArFebRodMapping& febRod,
                                       const Identifier& id) const
{
  HWIdentifier sigId = onOffId.createSignalChannelID (id);
  Identifier     rId = m_ttSvc->whichTTID (id);
  IdentifierHash hashid;
  IdContext layer_context = m_TT_ID->layer_context();
  if (m_TT_ID->get_hash (rId, hashid, &layer_context) != 0) {
    ATH_MSG_ERROR ("Failed to get TT hash");
    return StatusCode::FAILURE;
  }
  HWIdentifier febId = m_onlineID->feb_Id(sigId);
  HWIdentifier robId = febRod.getReadoutModuleID (febId); 
  m.addEntry (id, sigId, robId, hashid);
  return StatusCode::SUCCESS;
}
