/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Converted between the persistent and transient
// representations of TBEvent/TBScintillatorCont
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBTPCNV_TBSCINTILLATORCONTCNV_P1_H
#define TBTPCNV_TBSCINTILLATORCONTCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TBTPCnv/TBScintillatorCont_p1.h"

class MsgStream;
class TBScintillatorCont;


class TBScintillatorContCnv_p1  : public T_AthenaPoolTPCnvConstBase<TBScintillatorCont, TBScintillatorCont_p1>
{
public:
  using base_class::transToPers;
  using base_class::persToTrans;


  TBScintillatorContCnv_p1() {}

  virtual void          persToTrans(const TBScintillatorCont_p1* pers, TBScintillatorCont* trans, MsgStream &log) const override;
  virtual void          transToPers(const TBScintillatorCont* trans, TBScintillatorCont_p1* pers, MsgStream &log) const override;
};



#endif
