/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
//  file:   TgcCoinDataCnv_p1.cxx
//* Takashi Kubota - June 30, 2008 */
//-----------------------------------------------------------------------------

#include "MuonTrigCoinData/TgcCoinData.h"
#include "MuonEventTPCnv/MuonTrigCoinData/TgcCoinDataCnv_p1.h"
#include <algorithm>
#include "AthLinks/tools/IdentContIndex.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"
#include "CxxUtils/make_unique.h"

void TgcCoinDataCnv_p1::
persToTrans( const Muon::TgcCoinData_p1 *persObj, Muon::TgcCoinData *transObj,MsgStream &log ) 
{
   Muon::TgcCoinData::CoinDataType type = Muon::TgcCoinData::TYPE_UNKNOWN;
   switch(persObj->m_type){
   case 0: type=Muon::TgcCoinData::TYPE_TRACKLET; break;
   case 1: type=Muon::TgcCoinData::TYPE_HIPT; break;
   case 2: type=Muon::TgcCoinData::TYPE_SL; break;
   case 3: type=Muon::TgcCoinData::TYPE_UNKNOWN; break;
   case 4: type=Muon::TgcCoinData::TYPE_TRACKLET_EIFI; break;
   default: break;
   }

   auto posIn = std::unique_ptr<Amg::Vector2D>
     (createTransFromPStore( &m_localPosCnv, persObj->m_posIn, log ));
   auto posOut = std::unique_ptr<Amg::Vector2D>
     (createTransFromPStore( &m_localPosCnv, persObj->m_posOut, log ));

   Trk::ErrorMatrix dummy;
   fillTransFromPStore( &m_errorMxCnv, persObj->m_errMat, &dummy, log );
   auto tempMat = CxxUtils::make_unique<Amg::MatrixX>();
   EigenHelpers::vectorToEigenMatrix(dummy.values, *tempMat, "TgcCoinDataCnv_p1");

   *transObj = Muon::TgcCoinData (Identifier(persObj->m_channelIdIn),
                                  Identifier(persObj->m_channelIdOut),
                                  persObj->m_collectionIdHash,
                                  transObj->detectorElementIn(),
                                  transObj->detectorElementOut(),
                                  type,
                                  persObj->m_isAside,
                                  persObj->m_phi,
                                  false, // isInner
                                  persObj->m_isForward,
                                  persObj->m_isStrip,
                                  persObj->m_trackletId,
                                  posIn.release(),
                                  posOut.release(),
                                  persObj->m_widthIn,
                                  persObj->m_widthOut,
                                  persObj->m_delta,
                                  0,  // sub
                                  0,  // tile
                                  persObj->m_roi,
                                  persObj->m_pt,
                                  persObj->m_trackletIdStrip,
                                  tempMat.release()
                                  );

   transObj->setHashAndIndex(persObj->m_indexAndHash);
}

void TgcCoinDataCnv_p1::
transToPers( const Muon::TgcCoinData *transObj, Muon::TgcCoinData_p1 *persObj, MsgStream &log )
{
   //log << MSG::DEBUG << "TgcCoinDataCnv_p1::transToPers" << endmsg;
 
  persObj->m_channelIdIn = transObj->channelIdIn().get_identifier32().get_compact(); 
   persObj->m_channelIdOut = transObj->channelIdOut().get_identifier32().get_compact(); 
   persObj->m_collectionIdHash = transObj->identifyHash();
   persObj->m_indexAndHash=transObj->getHashAndIndex().hashAndIndex();
   switch(transObj->type()){
   case Muon::TgcCoinData::TYPE_TRACKLET: persObj->m_type=0; break;
   case Muon::TgcCoinData::TYPE_HIPT: persObj->m_type=1;  break;
   case Muon::TgcCoinData::TYPE_SL: persObj->m_type=2; break;
   case Muon::TgcCoinData::TYPE_UNKNOWN: persObj->m_type=3; break;
   case Muon::TgcCoinData::TYPE_TRACKLET_EIFI: persObj->m_type=4; break;
   default: break;
   }  
   //persObj->m_type = transObj->m_type; 
   persObj->m_isAside = transObj->isAside(); 
   persObj->m_phi = transObj->phi(); 
   persObj->m_isForward = transObj->isForward(); 
   persObj->m_isStrip = transObj->isStrip(); 
   persObj->m_trackletId = transObj->trackletId(); 
   persObj->m_trackletIdStrip = transObj->trackletIdStrip(); 
   persObj->m_widthIn = transObj->widthIn(); 
   persObj->m_widthOut = transObj->widthOut(); 
   persObj->m_posIn = toPersistent( &m_localPosCnv, &transObj->posIn(), log );
   persObj->m_posOut = toPersistent( &m_localPosCnv, &transObj->posOut(), log );
   // persObj->m_errMat = toPersistent( &m_errorMxCnv, transObj->m_errMat, log );
   if (transObj->hasErrMat() )
   {
     Trk::ErrorMatrix pMat;
     EigenHelpers::eigenMatrixToVector(pMat.values, transObj->errMat(), "TgcCoinDataCnv_p1");
     persObj->m_errMat = toPersistent( &m_errorMxCnv, &pMat, log );
   }
   persObj->m_delta = transObj->delta(); 
   persObj->m_roi = transObj->roi(); 
   persObj->m_pt = transObj->pt(); 
}


