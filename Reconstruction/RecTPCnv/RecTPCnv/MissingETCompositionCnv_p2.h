/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGETCOMPOSITIONCNV_P2_H
#define RECTPCNV_MISSINGETCOMPOSITIONCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "RecTPCnv/MissingETCompositionConverterBase.h"
#include "RecTPCnv/MissingETComposition_p2.h"
#include "MissingETEvent/MissingETComposition.h"

template <>
class MissingETCompositionConverterBase<MissingETComposition_p2>
: public T_AthenaPoolTPCnvBase<MissingETComposition, MissingETComposition_p2>
{
 public: 
  
  MissingETCompositionConverterBase() { };
  virtual ~MissingETCompositionConverterBase() { };
  
  void persToTrans( const MissingETComposition_p2* persObj, 
		    MissingETComposition* transObj, 
		    MsgStream& reporter );
  
  void transToPers( const MissingETComposition* transObj,
		    MissingETComposition_p2* persObj, 
		    MsgStream& reporter );
  
 protected: 
};

typedef MissingETCompositionConverterBase<MissingETComposition_p2> MissingETCompositionCnv_p2;


#endif
