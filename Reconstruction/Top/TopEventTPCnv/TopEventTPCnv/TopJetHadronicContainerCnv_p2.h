/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-

#ifndef TOPJETHADRONICCONTAINERCNV_P2_H
#define TOPJETHADRONICCONTAINERCNV_P2_H

#include "TopEvent/TopJetHadronicContainer.h"
#include "TopEventTPCnv/TopJetHadronicCnv_p2.h"
#include "TopEventTPCnv/TopJetHadronicContainer_p2.h"

#include "TopEventTPCnv/VectorCnv_p1.h"


// // TopJetHadronicContainerCnv_p2
// class TopJetHadronicContainerCnv_p2 :
//   public VectorCnv_p1<TopRec::TopJetHadronicContainer,TopJetHadronicContainer_p2,TopJetHadronicCnv_p2> 
// {};


// #include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// #include "GaudiKernel/MsgStream.h"

// class TopJetHadronicContainerCnv_p2 
//   : public T_AthenaPoolTPCnvBase<TopRec::TopJetHadronicContainer, TopJetHadronicContainer_p2>
// { 
//   TopJetHadronicCnv_p2 m_cnv;
// public: 
//   virtual void persToTrans( const TopJetHadronicContainer_p2* persObj, 
//                             TopRec::TopJetHadronicContainer* transObj, 
//                             MsgStream& msg );
  
//   virtual void transToPers( const TopRec::TopJetHadronicContainer* transObj, 
//                             TopJetHadronicContainer_p2* persObj, 
//                             MsgStream& msg );
// }; 
// VectorCnv_p1
typedef VectorCnv_p1<TopRec::TopJetHadronicContainer,TopJetHadronicContainer_p2,TopJetHadronicCnv_p2> TopJetHadronicContainerCnv_p2;


// T_TPCnv
template <> class T_TPCnv<TopRec::TopJetHadronicContainer,TopJetHadronicContainer_p2> : 
public TopJetHadronicContainerCnv_p2 
{
public:
};

#endif
