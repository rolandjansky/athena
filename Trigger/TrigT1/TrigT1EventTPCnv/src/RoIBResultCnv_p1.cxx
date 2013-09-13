/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converting ROIB::RoIBResult from persistent state..." << endreq;

   /////////////////////////////////////////////////////////////////
   //                                                             //
   //                Transfer the MuCTPI information              //
   //                                                             //
   /////////////////////////////////////////////////////////////////
   transObj->m_RoIBResultMuCTPI.m_MuCTPIResultHeader.m_header   = persObj->m_muctpi.m_header;
   transObj->m_RoIBResultMuCTPI.m_MuCTPIResultTrailer.m_trailer = persObj->m_muctpi.m_trailer;

   transObj->m_RoIBResultMuCTPI.m_MuCTPIResultRoIVec.clear();
   for( std::vector< uint32_t >::const_iterator it = persObj->m_muctpi.m_roiWords.begin();
        it != persObj->m_muctpi.m_roiWords.end(); ++it ) {
      transObj->m_RoIBResultMuCTPI.m_MuCTPIResultRoIVec.push_back( ROIB::MuCTPIRoI( *it ) );
   }

   /////////////////////////////////////////////////////////////////
   //                                                             //
   //                 Transfer the CTP information                //
   //                                                             //
   /////////////////////////////////////////////////////////////////
   transObj->m_RoIBResultCTP.m_CTPResultHeader.m_header   = persObj->m_ctp.m_header;
   transObj->m_RoIBResultCTP.m_CTPResultTrailer.m_trailer = persObj->m_ctp.m_trailer;

   transObj->m_RoIBResultCTP.m_CTPResultRoIVec.clear();
   for( std::vector< uint32_t >::const_iterator it = persObj->m_ctp.m_roiWords.begin();
        it != persObj->m_ctp.m_roiWords.end(); ++it ) {
      transObj->m_RoIBResultCTP.m_CTPResultRoIVec.push_back( ROIB::CTPRoI( *it ) );
   }

   /////////////////////////////////////////////////////////////////
   //                                                             //
   //             Transfer the Jet/Energy information             //
   //                                                             //
   /////////////////////////////////////////////////////////////////
   transObj->m_RoIBResultJetEnergy.clear();
   for( std::vector< SubSysResult_p1 >::const_iterator jeten = persObj->m_jetenergy.begin();
        jeten != persObj->m_jetenergy.end(); ++jeten ) {

      std::vector< ROIB::JetEnergyRoI > roiwords;
      for( std::vector< uint32_t >::const_iterator it = jeten->m_roiWords.begin();
           it != jeten->m_roiWords.end(); ++it ) {
         roiwords.push_back( ROIB::JetEnergyRoI( *it ) );
      }

      transObj->m_RoIBResultJetEnergy.push_back( ROIB::JetEnergyResult( ROIB::Header(jeten->m_header), 
									ROIB::Trailer(jeten->m_trailer),
                                                                        roiwords ) );
   }

   /////////////////////////////////////////////////////////////////
   //                                                             //
   //               Transfer the EM/Tau information               //
   //                                                             //
   /////////////////////////////////////////////////////////////////
   transObj->m_RoIBResultEMTau.clear();
   for( std::vector< SubSysResult_p1 >::const_iterator emtau = persObj->m_emtau.begin();
        emtau != persObj->m_emtau.end(); ++emtau ) {

      std::vector< ROIB::EMTauRoI > roiwords;
      for( std::vector< uint32_t >::const_iterator it = emtau->m_roiWords.begin();
           it != emtau->m_roiWords.end(); ++it ) {
         roiwords.push_back( ROIB::EMTauRoI( *it ) );
      }

      transObj->m_RoIBResultEMTau.push_back( ROIB::EMTauResult( ROIB::Header(emtau->m_header), 
								ROIB::Trailer(emtau->m_trailer),
                                                                roiwords ) );
   }

   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converting ROIB::RoIBResult from persistent state [OK]" << endreq;

   return;

}

/**
 * Function transferring the information from a transient ROIB::RoIBResult object
 * to a persistent RoIBResult_p1 object.
 */
void RoIBResultCnv_p1::transToPers( const ROIB::RoIBResult* transObj, RoIBResult_p1* persObj,
                                    MsgStream& log ) {

   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Creating persistent state of ROIB::RoIBResult..." << endreq;

   /////////////////////////////////////////////////////////////////
   //                                                             //
   //                Transfer the MuCTPI information              //
   //                                                             //
   /////////////////////////////////////////////////////////////////
   persObj->m_muctpi.m_header  = transObj->m_RoIBResultMuCTPI.m_MuCTPIResultHeader.m_header;
   persObj->m_muctpi.m_trailer = transObj->m_RoIBResultMuCTPI.m_MuCTPIResultTrailer.m_trailer;

   persObj->m_muctpi.m_roiWords.clear();
   for( std::vector< ROIB::MuCTPIRoI >::const_iterator it =
           transObj->m_RoIBResultMuCTPI.m_MuCTPIResultRoIVec.begin();
        it != transObj->m_RoIBResultMuCTPI.m_MuCTPIResultRoIVec.end(); ++it ) {
      persObj->m_muctpi.m_roiWords.push_back( it->roIWord() );
   }

   /////////////////////////////////////////////////////////////////
   //                                                             //
   //                 Transfer the CTP information                //
   //                                                             //
   /////////////////////////////////////////////////////////////////
   persObj->m_ctp.m_header  = transObj->m_RoIBResultCTP.m_CTPResultHeader.m_header;
   persObj->m_ctp.m_trailer = transObj->m_RoIBResultCTP.m_CTPResultTrailer.m_trailer;

   persObj->m_ctp.m_roiWords.clear();
   for( std::vector< ROIB::CTPRoI >::const_iterator it =
           transObj->m_RoIBResultCTP.m_CTPResultRoIVec.begin();
        it != transObj->m_RoIBResultCTP.m_CTPResultRoIVec.end(); ++it ) {
      persObj->m_ctp.m_roiWords.push_back( it->roIWord() );
   }

   /////////////////////////////////////////////////////////////////
   //                                                             //
   //             Transfer the Jet/Energy information             //
   //                                                             //
   /////////////////////////////////////////////////////////////////
   persObj->m_jetenergy.clear();
   for( std::vector< ROIB::JetEnergyResult >::const_iterator jeten =
           transObj->m_RoIBResultJetEnergy.begin();
        jeten != transObj->m_RoIBResultJetEnergy.end(); ++jeten ) {

      persObj->m_jetenergy.push_back( SubSysResult_p1() );

      persObj->m_jetenergy.back().m_header  = jeten->m_JetEnergyResultHeader.m_header;
      persObj->m_jetenergy.back().m_trailer = jeten->m_JetEnergyResultTrailer.m_trailer;

      for( std::vector< ROIB::JetEnergyRoI >::const_iterator it =
              jeten->m_JetEnergyResultRoIVec.begin();
           it != jeten->m_JetEnergyResultRoIVec.end(); ++it ) {
         persObj->m_jetenergy.back().m_roiWords.push_back( it->roIWord() );
      }
   }

   /////////////////////////////////////////////////////////////////
   //                                                             //
   //               Transfer the EM/Tau information               //
   //                                                             //
   /////////////////////////////////////////////////////////////////
   persObj->m_emtau.clear();
   for( std::vector< ROIB::EMTauResult >::const_iterator emtau =
           transObj->m_RoIBResultEMTau.begin();
        emtau != transObj->m_RoIBResultEMTau.end(); ++emtau ) {

      persObj->m_emtau.push_back( SubSysResult_p1() );

      persObj->m_emtau.back().m_header  = emtau->m_EMTauResultHeader.m_header;
      persObj->m_emtau.back().m_trailer = emtau->m_EMTauResultTrailer.m_trailer;

      for( std::vector< ROIB::EMTauRoI >::const_iterator it =
	     emtau->m_EMTauResultRoIVec.begin();
           it != emtau->m_EMTauResultRoIVec.end(); ++it ) {
	persObj->m_emtau.back().m_roiWords.push_back( it->roIWord() );
      }
   }

   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Creating persistent state of ROIB::RoIBResult [OK]" << endreq;

   return;

}
