/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/CscRawData.h"
#include "CscRawDataCnv_p4.h"
#include "GaudiKernel/GaudiException.h"
#include "StoreGate/StoreGateSvc.h"
#include <TString.h> // for Form

bool CscRawDataCnv_p4::initialize() {
  if (m_init) return m_init;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StoreGateSvc* detStore;
  if (svcLocator->service("DetectorStore", detStore).isFailure()) {
    throw std::runtime_error(Form("File: %s, Line: %d\nCscRawDataCnv_p4::initialize() - Failed to retrieve DetectorStore", __FILE__, __LINE__));
  }  
  if (detStore->retrieve(m_cscIdHelper, "CSCIDHELPER").isFailure()) {
    throw std::runtime_error(Form("File: %s, Line: %d\nCscRawDataCnv_p4::initialize() - Failed to retrieve CscIdHelper (needed for channel hash conversion)", __FILE__, __LINE__));
  }
  m_init = true;
  return m_init;
}

void
CscRawDataCnv_p4::transToPers(const CscRawData* trans, CscRawData_p4* pers, MsgStream &) 
{
    if (!this->initialize()) throw std::runtime_error(Form("File: %s, Line: %d\nCscRawDataCnv_p4::transToPers() - Failed to initialize", __FILE__, __LINE__));

    pers->m_amps        = trans->samples();
    pers->m_address     = trans->address();
    pers->m_id          = trans->identify();
    pers->m_time        = trans->time();
    pers->m_rpuID       = trans->rpuID();
    pers->m_width       = trans->width();
    pers->m_isTimeComputed        = trans->isTimeComputed();
    if (m_cscIdHelper) {
      // translate the transient (geometrical) hash into a positional hash as expected by the p4 persistent data format
      IdContext context = m_cscIdHelper->channel_context();
      Identifier id;
      IdentifierHash posHash;
      if (!m_cscIdHelper->get_id(trans->hashId(), id, &context)) { // retrieve identifier from geometrical hash
          if (!m_cscIdHelper->get_pos_channel_hash(id, posHash)) pers->m_hashId  = posHash; // retrieve positional identifier hash from identifier
      }
    } else {
      throw std::runtime_error(Form("File: %s, Line: %d\nCscRawDataCnv_p4::transToPers() - No CscIdHelper present (needed for channel hash conversion)", __FILE__, __LINE__));
    }
}

void
CscRawDataCnv_p4::persToTrans(const CscRawData_p4* pers, CscRawData* trans, MsgStream &) 
{
   if (!this->initialize()) throw std::runtime_error(Form("File: %s, Line: %d\nCscRawDataCnv_p4::persToTrans() - Failed to initialize", __FILE__, __LINE__));

   *trans = CscRawData (pers->m_amps,
                        pers->m_address,
                        pers->m_id,
                        pers->m_rpuID,
                        pers->m_width);
   if (m_cscIdHelper) {
     // translate the persistent p4 (positional) hash into a geometrical hash (as expected by the transient data format)
     IdentifierHash posHash = pers->m_hashId;
     IdentifierHash geoHash;
     Identifier id;
     if (!m_cscIdHelper->get_id_fromPosHash(posHash, id)) { // retrieve identifier from positional identifier hash
       if (!m_cscIdHelper->get_channel_hash(id, geoHash)) { // retrieve geometrical identifier hash from identifier
         trans->setHashID(geoHash);
       }
     }
   } else {
      throw std::runtime_error(Form("File: %s, Line: %d\nCscRawDataCnv_p4::persToTrans() - No CscIdHelper present (needed for channel hash conversion)", __FILE__, __LINE__));
   }
   if (pers->m_isTimeComputed)
     trans->setTime (pers->m_time);
}
