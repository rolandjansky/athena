/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBTPCNV_TBADCRAWCONTCNV_P1
#define TBTPCNV_TBADCRAWCONTCNV_P1

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TBTPCnv/TBADCRawCont_p1.h"

class MsgStream;
class TBADCRawCont;


class TBADCRawContCnv_p1  : public T_AthenaPoolTPCnvBase<TBADCRawCont, TBADCRawCont_p1>
{
public:

  TBADCRawContCnv_p1() {}

  virtual void          persToTrans(const TBADCRawCont_p1* pers, TBADCRawCont* trans, MsgStream &log);
  virtual void          transToPers(const TBADCRawCont* trans, TBADCRawCont_p1* pers, MsgStream &log);
};



#endif


