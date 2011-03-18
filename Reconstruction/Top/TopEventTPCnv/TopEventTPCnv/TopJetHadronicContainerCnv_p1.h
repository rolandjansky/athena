/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-

#ifndef TOPJETHADRONICCONTAINERCNV_P1_H
#define TOPJETHADRONICCONTAINERCNV_P1_H

#include "TopEvent/TopJetHadronicContainer.h"
#include "TopEventTPCnv/TopJetHadronicCnv_p1.h"
#include "TopEventTPCnv/TopJetHadronicContainer_p1.h"

#include "TopEventTPCnv/VectorCnv_p1.h"


// // TopJetHadronicContainerCnv_p1
// class TopJetHadronicContainerCnv_p1 :
//   public VectorCnv_p1<TopRec::TopJetHadronicContainer,TopJetHadronicContainer_p1,TopJetHadronicCnv_p1> 
// {};


// #include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// #include "GaudiKernel/MsgStream.h"

// class TopJetHadronicContainerCnv_p1 
//   : public T_AthenaPoolTPCnvBase<TopRec::TopJetHadronicContainer, TopJetHadronicContainer_p1>
// { 
//   TopJetHadronicCnv_p1 m_cnv;
// public: 
//   virtual void persToTrans( const TopJetHadronicContainer_p1* persObj, 
//                             TopRec::TopJetHadronicContainer* transObj, 
//                             MsgStream& msg );
  
//   virtual void transToPers( const TopRec::TopJetHadronicContainer* transObj, 
//                             TopJetHadronicContainer_p1* persObj, 
//                             MsgStream& msg );
// }; 
// VectorCnv_p1
typedef VectorCnv_p1<TopRec::TopJetHadronicContainer,TopJetHadronicContainer_p1,TopJetHadronicCnv_p1> TopJetHadronicContainerCnv_p1;


// T_TPCnv
template <> class T_TPCnv<TopRec::TopJetHadronicContainer,TopJetHadronicContainer_p1> : 
public TopJetHadronicContainerCnv_p1 
{
public:
};

#endif
