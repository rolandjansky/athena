/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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


class TBScintillatorContCnv_p1  : public T_AthenaPoolTPCnvBase<TBScintillatorCont, TBScintillatorCont_p1>
{
public:

  TBScintillatorContCnv_p1() {}

  virtual void          persToTrans(const TBScintillatorCont_p1* pers, TBScintillatorCont* trans, MsgStream &log);
  virtual void          transToPers(const TBScintillatorCont* trans, TBScintillatorCont_p1* pers, MsgStream &log);
};



#endif
