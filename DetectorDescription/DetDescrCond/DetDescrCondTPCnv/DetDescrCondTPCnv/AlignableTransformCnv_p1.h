/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCONDTPCNV_ALIGNABLETRANSFORMCNV_P1_H
#define DETDESCRCONDTPCNV_ALIGNABLETRANSFORMCNV_P1_H

#include "DetDescrConditions/AlignableTransform.h"
#include "DetDescrCondTPCnv/AlignableTransform_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class AlignableTransformCnv_p1  : public T_AthenaPoolTPCnvBase<AlignableTransform, AlignableTransform_p1>  {
public:
  AlignableTransformCnv_p1() {}
  virtual void   persToTrans(const AlignableTransform_p1* persObj, AlignableTransform* transObj, MsgStream &log) ;
  virtual void   transToPers(const AlignableTransform* transObj, AlignableTransform_p1* persObj, MsgStream &log) ;

};

#endif // DETDESCRCONDTPCNV_ALIGNABLETRANSFORMCNV_P1_H

