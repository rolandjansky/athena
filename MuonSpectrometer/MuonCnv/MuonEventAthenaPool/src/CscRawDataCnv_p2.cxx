/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/CscRawData.h"
#include "CscRawDataCnv_p2.h"
#include "GaudiKernel/GaudiException.h"
#include <TString.h> // for Form

void
CscRawDataCnv_p2::transToPers(const CscRawData* trans, CscRawData_p2* pers, MsgStream &) 
{
    pers->m_amps        = trans->samples();
    pers->m_address     = trans->address();
    pers->m_id          = trans->identify();
    pers->m_time        = trans->time();
    pers->m_rpuID       = trans->rpuID();
    pers->m_width       = trans->width();
    pers->m_isTimeComputed        = trans->isTimeComputed();
    if (m_idHelp) {
      // translate the transient (positional) hash into a geometrical hash as expected by the p2 persistent data format
      if (!m_idHelp->hasCSC()) std::cout << "CscRawDataCnv_p2::transToPers: !m_idHelp.hasCSC()" << std::endl;
      IdContext context = m_idHelp->cscIdHelper().channel_context();
      Identifier id;
      IdentifierHash geoHash;
      if (!m_idHelp->cscIdHelper().get_id(trans->hashId(), id, &context)) {
          if (!m_idHelp->cscIdHelper().get_geo_channel_hash(id, geoHash)) pers->m_hashId  = geoHash;
      }
    } else {
      throw std::runtime_error(Form("File: %s, Line: %d\nCscRawDataCnv_p2::transToPers() - No MuonIdHelperSvc present (needed for channel hash conversion)", __FILE__, __LINE__));
    }
}


// schema evolution for case a) below https://savannah.cern.ch/bugs/?56002
// There can be two cases:
// (a)  Reco in 15.5.X.Y or after reading RDO made in 15.3.X.Y and before.
// (b)  both reco and simulation in 15.5.X.Y or after
void
CscRawDataCnv_p2::persToTrans(const CscRawData_p2* pers, CscRawData* trans, MsgStream &) 
{
   // Fix for CSC phi strip - see https://savannah.cern.ch/bugs/?56002
   int stationEta  =  ( ((pers->m_address & 0x00001000) >> 12 ) == 0x0) ? -1 : 1;
   int measuresPhi   = ( (pers->m_address & 0x00000100) >>  8);

   uint32_t address = 0;
   if (stationEta==1 && measuresPhi ==1) { // for Wheel A phi strips
     uint32_t oldFirstStrip  = uint32_t (pers->m_address & 0x000000FF); // ( 0 ~ 47 )
     uint32_t newFirstStrip  = uint32_t(47-oldFirstStrip) - pers->m_width + 1; // flip and then shift the firstStrip by 1 - width
                                                                                // width is a number of strips in cluster
     uint32_t flippedAddress = pers->m_address - oldFirstStrip + newFirstStrip;
     address = flippedAddress;

     ////////////////////
     // log message to confirm "flip and shift" correctly. stripId ranges ( 1-48 )
     ///////////////////////////
     //     std::cout << "    CscRawDataCnv_p2 " << std::hex << pers->m_address << " ===> " << address << std::dec
     //               << "  Strips [" << int(oldFirstStrip+1) << "," << int(oldFirstStrip+trans->m_width) << "] ===> [" 
     //               <<  int(newFirstStrip)+1 << "," << int(newFirstStrip + trans->m_width) << "] "
     //               << " compared to Correct [" << 47-int(oldFirstStrip)+1 << "," << 47-int(oldFirstStrip) - int(trans->m_width)+1+1 << "]" 
     //               << std::endl;
     
   } else {
     address     = pers->m_address;
   }

   *trans = CscRawData (pers->m_amps,
                        address,
                        pers->m_id,
                        pers->m_rpuID,
                        pers->m_width);
   // translate the persistent p2 (geometrical) hash into a positional hash (as expected by the transient data format)
   IdentifierHash geoHash = pers->m_hashId;
   IdentifierHash hash;
   if (m_idHelp) {
     IdContext context = m_idHelp->cscIdHelper().channel_context();
     if (!m_idHelp->cscIdHelper().get_hash_fromGeoHash(geoHash, hash, &context)) trans->setHashID(hash);
   } else {
     throw std::runtime_error(Form("File: %s, Line: %d\nCscRawDataCnv_p2::persToTrans() - No MuonIdHelperSvc present (needed for channel hash conversion)", __FILE__, __LINE__));
   }
   if (pers->m_isTimeComputed)
     trans->setTime (pers->m_time);
}
