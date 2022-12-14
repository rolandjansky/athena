/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Converted between the persistent and transient
// representations of TBEvent/TBEventInfo
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBTPCNV_TBEVENTINFOCNV_P1_H
#define TBTPCNV_TBEVENTINFOCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TBTPCnv/TBEventInfo_p1.h"

class MsgStream;
class TBEventInfo;


class TBEventInfoCnv_p1  : public T_AthenaPoolTPCnvConstBase<TBEventInfo, TBEventInfo_p1>
{
public:
  using base_class::transToPers;
  using base_class::persToTrans;


  TBEventInfoCnv_p1() {}

  virtual void          persToTrans(const TBEventInfo_p1* pers, TBEventInfo* trans, MsgStream &log) const override;
  virtual void          transToPers(const TBEventInfo* trans, TBEventInfo_p1* pers, MsgStream &log) const override;
};



#endif
