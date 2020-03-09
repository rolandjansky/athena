/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Converted between the persistent and transient
// representations of TBEvent/TBTDCRawCont
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBTPCNV_TBTDCRAWCONTCNV_P1_H
#define TBTPCNV_TBTDCRAWCONTCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TBTPCnv/TBTDCRawCont_p1.h"

class MsgStream;
class TBTDCRawCont;


class TBTDCRawContCnv_p1  : public T_AthenaPoolTPCnvConstBase<TBTDCRawCont, TBTDCRawCont_p1>
{
public:
  using base_class::transToPers;
  using base_class::persToTrans;


  TBTDCRawContCnv_p1() {}

  virtual void          persToTrans(const TBTDCRawCont_p1* pers, TBTDCRawCont* trans, MsgStream &log) const override;
  virtual void          transToPers(const TBTDCRawCont* trans, TBTDCRawCont_p1* pers, MsgStream &log) const override;
};



#endif
