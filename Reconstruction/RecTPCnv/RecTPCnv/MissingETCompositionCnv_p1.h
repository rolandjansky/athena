/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGETCOMPOSITIONCNV_P1_H
#define RECTPCNV_MISSINGETCOMPOSITIONCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "RecTPCnv/MissingETCompositionConverterBase.h"
#include "RecTPCnv/MissingETComposition_p1.h"
#include "MissingETEvent/MissingETComposition.h"

template <>
class MissingETCompositionConverterBase<MissingETComposition_p1>
: public T_AthenaPoolTPCnvConstBase<MissingETComposition, MissingETComposition_p1>
{
 public: 
  using base_class::transToPers;
  using base_class::persToTrans;


  MissingETCompositionConverterBase() { };
  virtual ~MissingETCompositionConverterBase() { };
  
  /** Method creating the transient representation @c MissingETComposition
   *  from its persistent representation @c MissingETComposition_p1
   */
  virtual void persToTrans( const MissingETComposition_p1* persObj, 
                            MissingETComposition* transObj, 
                            MsgStream& reporter ) const override;
  
  /** Method creating the persistent representation @c MissingETComposition_p1
   *  from its transient representation @c MissingETComposition
   */
  virtual void transToPers( const MissingETComposition* transObj,
                            MissingETComposition_p1* persObj, 
                            MsgStream& reporter ) const override;
};

typedef MissingETCompositionConverterBase<MissingETComposition_p1> MissingETCompositionCnv_p1;


#endif
