/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "TrigT1EventTPCnv/RoIBResultCnv_p1.h"

/**
 * Default constructor.
 */
RoIBResultCnv_p1::RoIBResultCnv_p1()
   : T_AthenaPoolTPCnvBase< ROIB::RoIBResult, RoIBResult_p1 >() {

}

/**
 * Function transferring the information from a persistent RoIBResult_p1 object
 * to a transient ROIB::RoIBResult object.
 */
void RoIBResultCnv_p1::persToTrans( const RoIBResult_p1* persObj, ROIB::RoIBResult* transObj,
                                    MsgStream& log ) {

   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converting ROIB::RoIBResult from persistent state..." << endmsg;

   ROIB::MuCTPIResult muc (ROIB::Header (std::vector< uint32_t >(persObj->m_muctpi.m_header)),
                           ROIB::Trailer (std::vector< uint32_t >(persObj->m_muctpi.m_trailer)),
                           std::vector<ROIB::MuCTPIRoI> (persObj->m_muctpi.m_roiWords.begin(),
                                                         persObj->m_muctpi.m_roiWords.end()));

   ROIB::CTPResult ctp (0,
                        ROIB::Header (std::vector< uint32_t >(persObj->m_ctp.m_header)),
                        ROIB::Trailer (std::vector< uint32_t >(persObj->m_ctp.m_trailer)),
                        persObj->m_ctp.m_roiWords);


   std::vector< ROIB::JetEnergyResult > jetvec;
   for (const SubSysResult_p1& jeten : persObj->m_jetenergy) {
      jetvec.emplace_back ( ROIB::Header(std::vector< uint32_t >(jeten.m_header)), 
                            ROIB::Trailer(std::vector< uint32_t >(jeten.m_trailer)),
                            std::vector<ROIB::JetEnergyRoI> (jeten.m_roiWords.begin(),
                                                             jeten.m_roiWords.end()) );
   }

   std::vector< ROIB::EMTauResult > emtauvec;
   for (const SubSysResult_p1& emtau : persObj->m_emtau) {
     emtauvec.emplace_back ( ROIB::Header(std::vector< uint32_t >(emtau.m_header)), 
                             ROIB::Trailer(std::vector< uint32_t >(emtau.m_trailer)),
                             std::vector<ROIB::EMTauRoI> (emtau.m_roiWords.begin(),
                                                          emtau.m_roiWords.end()) );
   }

   *transObj = ROIB::RoIBResult (std::move(muc),
                                 std::move(ctp),
                                 std::move(jetvec),
                                 std::move(emtauvec));

   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converting ROIB::RoIBResult from persistent state [OK]" << endmsg;

   return;

}

/**
 * Function transferring the information from a transient ROIB::RoIBResult object
 * to a persistent RoIBResult_p1 object.
 */
void RoIBResultCnv_p1::transToPers( const ROIB::RoIBResult* transObj, RoIBResult_p1* persObj,
                                    MsgStream& log ) {

   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Creating persistent state of ROIB::RoIBResult..." << endmsg;

   /////////////////////////////////////////////////////////////////
   //                                                             //
   //                Transfer the MuCTPI information              //
   //                                                             //
   /////////////////////////////////////////////////////////////////
   persObj->m_muctpi.m_header  = transObj->muCTPIResult().header().header();
   persObj->m_muctpi.m_trailer = transObj->muCTPIResult().trailer().trailer();

   persObj->m_muctpi.m_roiWords.clear();
   for (const ROIB::MuCTPIRoI& r : transObj->muCTPIResult().roIVec())
     persObj->m_muctpi.m_roiWords.push_back( r.roIWord() );

   /////////////////////////////////////////////////////////////////
   //                                                             //
   //                 Transfer the CTP information                //
   //                                                             //
   /////////////////////////////////////////////////////////////////
   persObj->m_ctp.m_header  = transObj->cTPResult().header().header();
   persObj->m_ctp.m_trailer = transObj->cTPResult().trailer().trailer();

   persObj->m_ctp.m_roiWords.clear();
   for (const ROIB::CTPRoI& r : transObj->cTPResult().roIVec())
     persObj->m_ctp.m_roiWords.push_back( r.roIWord() );

   /////////////////////////////////////////////////////////////////
   //                                                             //
   //             Transfer the Jet/Energy information             //
   //                                                             //
   /////////////////////////////////////////////////////////////////
   persObj->m_jetenergy.clear();
   for (const ROIB::JetEnergyResult& jer : transObj->jetEnergyResult()) {

      persObj->m_jetenergy.push_back( SubSysResult_p1() );

      persObj->m_jetenergy.back().m_header  = jer.header().header();
      persObj->m_jetenergy.back().m_trailer = jer.trailer().trailer();
      for (const ROIB::JetEnergyRoI& r : jer.roIVec())
        persObj->m_jetenergy.back().m_roiWords.push_back( r.roIWord() );
   }

   /////////////////////////////////////////////////////////////////
   //                                                             //
   //               Transfer the EM/Tau information               //
   //                                                             //
   /////////////////////////////////////////////////////////////////
   persObj->m_emtau.clear();
   for (const ROIB::EMTauResult& emtau : transObj->eMTauResult()) {

      persObj->m_emtau.push_back( SubSysResult_p1() );

      persObj->m_emtau.back().m_header  = emtau.header().header();
      persObj->m_emtau.back().m_trailer = emtau.trailer().trailer();
      for (const ROIB::EMTauRoI& r : emtau.roIVec())
	persObj->m_emtau.back().m_roiWords.push_back( r.roIWord() );
   }

   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Creating persistent state of ROIB::RoIBResult [OK]" << endmsg;

   return;

}
