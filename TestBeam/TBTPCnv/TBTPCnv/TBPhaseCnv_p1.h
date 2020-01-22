/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Converted between the persistent and transient
// representations of TBEvent/TBPhase
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBTPCNV_TBPHASECNV_P1_H
#define TBTPCNV_TBPHASECNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TBTPCnv/TBPhase_p1.h"

class MsgStream;
class TBPhase;


class TBPhaseCnv_p1  : public T_AthenaPoolTPCnvConstBase<TBPhase, TBPhase_p1>
{
public:
  using base_class::transToPers;
  using base_class::persToTrans;


  TBPhaseCnv_p1() {}

  virtual void          persToTrans(const TBPhase_p1* pers, TBPhase* trans, MsgStream &log) const override;
  virtual void          transToPers(const TBPhase* trans, TBPhase_p1* pers, MsgStream &log) const override;
};



#endif
