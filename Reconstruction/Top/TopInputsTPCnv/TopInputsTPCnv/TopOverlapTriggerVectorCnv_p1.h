/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-

#ifndef TOP_OVERLAP_TRIGGER_VECTOR_CNV_PT_H
#define TOP_OVERLAP_TRIGGER_VECTOR_CNV_PT_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TopInputs/TopOverlapTriggerVector.h"
#include "TopInputsTPCnv/TopOverlapTriggerVector_p1.h"
#include "TopInputsTPCnv/TopOverlapTriggerCnv_p1.h"

class MsgStream;

class TopOverlapTriggerVectorCnv_p1 
  : public T_AthenaPoolTPCnvBase<TopRec::TopOverlapTriggerVector, TopOverlapTriggerVector_p1>
{ 
  TopOverlapTriggerCnv_p1 m_cnv;

 public: 
  virtual void persToTrans( const TopOverlapTriggerVector_p1* persObj, 
                            TopRec::TopOverlapTriggerVector* transObj, 
                            MsgStream& msg );
  
  virtual void transToPers( const TopRec::TopOverlapTriggerVector* transObj, 
                            TopOverlapTriggerVector_p1* persObj, 
                            MsgStream& msg );
}; 

#endif //TOP_OVERLAP_TRIGGER_VECTOR_CNV_PT_H
