/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Converted between the persistent and transient
// representations of TBEvent/TBMWPCCont
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBTPCNV_TBMWPCCONTCNV_P1_H
#define TBTPCNV_TBMWPCCONTCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TBTPCnv/TBMWPCCont_p1.h"

class MsgStream;
class TBMWPCCont;


class TBMWPCContCnv_p1  : public T_AthenaPoolTPCnvConstBase<TBMWPCCont, TBMWPCCont_p1>
{
public:
  using base_class::transToPers;
  using base_class::persToTrans;


  TBMWPCContCnv_p1() {}

  virtual void          persToTrans(const TBMWPCCont_p1* pers, TBMWPCCont* trans, MsgStream &log) const override;
  virtual void          transToPers(const TBMWPCCont* trans, TBMWPCCont_p1* pers, MsgStream &log) const override;
};



#endif
