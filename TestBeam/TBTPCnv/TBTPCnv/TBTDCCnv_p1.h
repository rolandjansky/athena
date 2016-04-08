/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Converted between the persistent and transient
// representations of TBEvent/TBTDC
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBTPCNV_TBTDCCNV_P1_H
#define TBTPCNV_TBTDCCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TBTPCnv/TBTDC_p1.h"

class MsgStream;
class TBTDC;


class TBTDCCnv_p1  : public T_AthenaPoolTPCnvBase<TBTDC, TBTDC_p1>
{
public:

  TBTDCCnv_p1() {}

  virtual void          persToTrans(const TBTDC_p1* pers, TBTDC* trans, MsgStream &log);
  virtual void          transToPers(const TBTDC* trans, TBTDC_p1* pers, MsgStream &log);
};



#endif
