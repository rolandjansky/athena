/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigCosmicEvent/CosmicMuon.h"
#undef private
#undef protected

#include "TrigCosmicEventTPCnv/CosmicMuon_p1.h"
#include "TrigCosmicEventTPCnv/CosmicMuonCnv_p1.h"


void CosmicMuonCnv_p1::persToTrans(const CosmicMuon_p1* persObj, 
				    CosmicMuon* transObj, 
				    MsgStream &log)
{
   log << MSG::DEBUG << "CosmicMuonCnv_p1::persToTrans called " << endreq;

   transObj->mP         = persObj->mP        ;
   transObj->mRadius    = persObj->mRadius   ;
   transObj->mTheta     = persObj->mTheta    ;
   transObj->mPhi       = persObj->mPhi      ;
   for(int i=0; i<3; ++i){
     transObj->mPoint[i]  = persObj->mPoint[i] ;
   }
   transObj->mT         = persObj->mT        ;
   transObj->mIsIngoing = persObj->mIsIngoing;
   transObj->mNRpcPairs = persObj->mNRpcPairs;
   transObj->mNTgcPairs = persObj->mNTgcPairs;
   transObj->mNMdtHits  = persObj->mNMdtHits ;
   transObj->mNMdtSegs  = persObj->mNMdtSegs ;
   
}


void CosmicMuonCnv_p1::transToPers(const CosmicMuon* transObj, 
				       CosmicMuon_p1* persObj, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "CosmicMuonCnv_p1::transToPers called " << endreq;

   persObj->mP         = transObj->mP        ;
   persObj->mRadius    = transObj->mRadius   ;
   persObj->mTheta     = transObj->mTheta    ;
   persObj->mPhi       = transObj->mPhi      ;
   for(int i=0; i<3; ++i){
     persObj->mPoint[i]  = transObj->mPoint[i] ;
   }
   persObj->mT         = transObj->mT        ;
   persObj->mIsIngoing = transObj->mIsIngoing;
   persObj->mNRpcPairs = transObj->mNRpcPairs;
   persObj->mNTgcPairs = transObj->mNTgcPairs;
   persObj->mNMdtHits  = transObj->mNMdtHits ;
   persObj->mNMdtSegs  = transObj->mNMdtSegs ;
  
}
