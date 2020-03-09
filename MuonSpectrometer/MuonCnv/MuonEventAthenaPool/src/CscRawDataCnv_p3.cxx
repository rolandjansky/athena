/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/CscRawData.h"
#include "CscRawDataCnv_p3.h"
#include "GaudiKernel/GaudiException.h"
#include <TString.h> // for Form

void
CscRawDataCnv_p3::transToPers(const CscRawData* trans, CscRawData_p3* pers, MsgStream &) 
{
    pers->m_amps        = trans->samples();
    pers->m_address     = trans->address();
    pers->m_id          = trans->identify();
    pers->m_time        = trans->time();
    pers->m_rpuID       = trans->rpuID();
    pers->m_width       = trans->width();
    pers->m_isTimeComputed        = trans->isTimeComputed();
    if (m_idHelp) {
      // translate the transient (positional) hash into a geometrical hash as expected by the p3 persistent data format
      IdContext context = m_idHelp->cscIdHelper().channel_context();
      Identifier id;
      IdentifierHash geoHash;
      if (!m_idHelp->cscIdHelper().get_id(trans->hashId(), id, &context)) {
          if (!m_idHelp->cscIdHelper().get_geo_channel_hash(id, geoHash)) pers->m_hashId  = geoHash;
      }
    } else {
      throw std::runtime_error(Form("File: %s, Line: %d\nCscRawDataCnv_p3::transToPers() - No MuonIdHelperSvc present (needed for channel hash conversion)", __FILE__, __LINE__));
    }
}

void
CscRawDataCnv_p3::persToTrans(const CscRawData_p3* pers, CscRawData* trans, MsgStream &) 
{
   *trans = CscRawData (pers->m_amps,
                        pers->m_address,
                        pers->m_id,
                        pers->m_rpuID,
                        pers->m_width);
   if (m_idHelp) {
     // translate the persistent p3 (geometrical) hash into a positional hash (as expected by the transient data format)
     IdentifierHash geoHash = pers->m_hashId;
     IdentifierHash hash;
     IdContext context = m_idHelp->cscIdHelper().channel_context();
     if (!m_idHelp->cscIdHelper().get_hash_fromGeoHash(geoHash, hash, &context)) {
      trans->setHashID(hash);
    }
   } else {
      throw std::runtime_error(Form("File: %s, Line: %d\nCscRawDataCnv_p3::persToTrans() - No MuonIdHelperSvc present (needed for channel hash conversion)", __FILE__, __LINE__));
   }
   if (pers->m_isTimeComputed)
     trans->setTime (pers->m_time);
}
