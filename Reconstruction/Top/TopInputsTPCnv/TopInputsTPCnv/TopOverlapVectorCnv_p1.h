/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-

#ifndef TOP_OVERLAP_VECTOR_CNV_P1_H
#define TOP_OVERLAP_VECTOR_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TopInputs/TopOverlapVector.h"
#include "TopInputsTPCnv/TopOverlapVector_p1.h"
#include "TopInputsTPCnv/TopOverlapCnv_p1.h"

class MsgStream;

class TopOverlapVectorCnv_p1 
  : public T_AthenaPoolTPCnvBase<TopRec::TopOverlapVector, TopOverlapVector_p1>
{ 
  TopOverlapCnv_p1 m_cnv;
public: 
  virtual void persToTrans( const TopOverlapVector_p1* persObj, 
                            TopRec::TopOverlapVector* transObj, 
                            MsgStream& msg );
  
  virtual void transToPers( const TopRec::TopOverlapVector* transObj, 
                            TopOverlapVector_p1* persObj, 
                            MsgStream& msg );
}; 

#endif //TOP_OVERLAP_VECTOR_CNV_P1_H
