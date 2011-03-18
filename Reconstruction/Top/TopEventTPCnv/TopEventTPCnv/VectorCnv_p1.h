/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
// 
// Generic converter for std::vectors
//
// Andrei Gaponenko, 2008

#ifndef VECTORCNV_P1_H 
#define VECTORCNV_P1_H 

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

template<class TransientVectorType, class PersistentVectorType, class ElementCnv>
class VectorCnv_p1 
  : public T_AthenaPoolTPCnvBase<TransientVectorType, PersistentVectorType>
{ 
  ElementCnv m_cnv;
public: 
  virtual void persToTrans( const PersistentVectorType* persObj, 
                            TransientVectorType* transObj, 
                            MsgStream& msg );
  
  virtual void transToPers( const TransientVectorType* transObj, 
                            PersistentVectorType* persObj, 
                            MsgStream& msg );
}; 

#include "TopEventTPCnv/VectorCnv_p1.icc"

#endif/*VECTORCNV_P1_H*/
