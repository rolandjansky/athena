/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "MuonRDO/CscRawData.h"
#undef private
#undef protected

#include "CscRawDataCnv_p2.h"

#include "GaudiKernel/GaudiException.h"

void
CscRawDataCnv_p2::transToPers(const CscRawData* trans, CscRawData_p2* pers, MsgStream &) 
{
    pers->m_amps        = trans->m_amps;
    pers->m_address     = trans->m_address;
    pers->m_id          = trans->m_id;
    pers->m_time        = trans->m_time;
    pers->m_rpuID       = trans->m_rpuID;
    pers->m_width       = trans->m_width;
    pers->m_isTimeComputed        = trans->m_isTimeComputed;
    pers->m_hashId        = trans->m_hashId;}


// schema evolution for case a) below https://savannah.cern.ch/bugs/?56002
// There can be two cases:
// (a)  Reco in 15.5.X.Y or after reading RDO made in 15.3.X.Y and before.
// (b)  both reco and simulation in 15.5.X.Y or after
void
CscRawDataCnv_p2::persToTrans(const CscRawData_p2* pers, CscRawData* trans, MsgStream &) 
{
   trans->m_amps        = pers->m_amps;
   trans->m_id          = pers->m_id;
   trans->m_time        = pers->m_time;
   trans->m_rpuID       = pers->m_rpuID;
   trans->m_width       = pers->m_width;
   trans->m_isTimeComputed        = pers->m_isTimeComputed;
   trans->m_hashId        = pers->m_hashId;

   // Fix for CSC phi strip - see https://savannah.cern.ch/bugs/?56002
   int stationEta  =  ( ((pers->m_address & 0x00001000) >> 12 ) == 0x0) ? -1 : 1;
   int measuresPhi   = ( (pers->m_address & 0x00000100) >>  8);

   if (stationEta==1 && measuresPhi ==1) { // for Wheel A phi strips
     uint32_t oldFirstStrip  = uint32_t (pers->m_address & 0x000000FF); // ( 0 ~ 47 )
     uint32_t newFirstStrip  = uint32_t(47-oldFirstStrip) - trans->m_width + 1; // flip and then shift the firstStrip by 1 - width
                                                                                // width is a number of strips in cluster
     uint32_t flippedAddress = pers->m_address - oldFirstStrip + newFirstStrip;
     trans->m_address = flippedAddress;

     ////////////////////
     // log message to confirm "flip and shift" correctly. stripId ranges ( 1-48 )
     ///////////////////////////
     //     std::cout << "    CscRawDataCnv_p2 " << std::hex << pers->m_address << " ===> " << trans->m_address << std::dec
     //               << "  Strips [" << int(oldFirstStrip+1) << "," << int(oldFirstStrip+trans->m_width) << "] ===> [" 
     //               <<  int(newFirstStrip)+1 << "," << int(newFirstStrip + trans->m_width) << "] "
     //               << " compared to Correct [" << 47-int(oldFirstStrip)+1 << "," << 47-int(oldFirstStrip) - int(trans->m_width)+1+1 << "]" 
     //               << std::endl;
     
   } else {
     trans->m_address     = pers->m_address;
   }
}
