/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Converted between the persistent and transient
// representations of TBEvent/TBBPCCont
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBTPCNV_TBBPCCONTCNV_P1_H
#define TBTPCNV_TBBPCCONTCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TBTPCnv/TBBPCCont_p1.h"

class MsgStream;
class TBBPCCont;


class TBBPCContCnv_p1  : public T_AthenaPoolTPCnvConstBase<TBBPCCont, TBBPCCont_p1>
{
public:
  using base_class::transToPers;
  using base_class::persToTrans;



  TBBPCContCnv_p1() {}

  virtual void          persToTrans(const TBBPCCont_p1* pers, TBBPCCont* trans, MsgStream &log) const override;
  virtual void          transToPers(const TBBPCCont* trans, TBBPCCont_p1* pers, MsgStream &log) const override;
};



#endif
