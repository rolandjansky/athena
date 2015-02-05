/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGETCOMPOSITIONCNV_P1_H
#define RECTPCNV_MISSINGETCOMPOSITIONCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "RecTPCnv/MissingETCompositionConverterBase.h"
#include "RecTPCnv/MissingETComposition_p1.h"
#include "MissingETEvent/MissingETComposition.h"

template <>
class MissingETCompositionConverterBase<MissingETComposition_p1>
: public T_AthenaPoolTPCnvBase<MissingETComposition, MissingETComposition_p1>
{
 public: 
  
  MissingETCompositionConverterBase() { };
  virtual ~MissingETCompositionConverterBase() { };
  
  /** Method creating the transient representation @c MissingETComposition
   *  from its persistent representation @c MissingETComposition_p1
   */
  void persToTrans( const MissingETComposition_p1* persObj, 
		    MissingETComposition* transObj, 
		    MsgStream& reporter );
  
  /** Method creating the persistent representation @c MissingETComposition_p1
   *  from its transient representation @c MissingETComposition
   */
  void transToPers( const MissingETComposition* transObj,
		    MissingETComposition_p1* persObj, 
		    MsgStream& reporter );
  
 protected: 
};

typedef MissingETCompositionConverterBase<MissingETComposition_p1> MissingETCompositionCnv_p1;


#endif
