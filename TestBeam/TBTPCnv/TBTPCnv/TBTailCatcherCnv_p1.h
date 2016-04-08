/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Converted between the persistent and transient
// representations of TBEvent/TBTailCatcher
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBTPCNV_TBTAILCATCHERCNV_P1_H
#define TBTPCNV_TBTAILCATCHERCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TBTPCnv/TBTailCatcher_p1.h"

class MsgStream;
class TBTailCatcher;


class TBTailCatcherCnv_p1  : public T_AthenaPoolTPCnvBase<TBTailCatcher, TBTailCatcher_p1>
{
public:

  TBTailCatcherCnv_p1() {}

  virtual void          persToTrans(const TBTailCatcher_p1* pers, TBTailCatcher* trans, MsgStream &log);
  virtual void          transToPers(const TBTailCatcher* trans, TBTailCatcher_p1* pers, MsgStream &log);
};



#endif
