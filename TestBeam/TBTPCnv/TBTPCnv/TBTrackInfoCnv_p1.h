/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------------
// Converted between the persistent and transient
// representations of TBEvent/TBTrackInfo
// See: https://twiki.cern.ch/twiki/bin/view/Atlas/TransientPersistentSeparation#TP_converters_for_component_type
// Author: Iftach Sadeh (iftach.sadeh@NOSPAMTODAYcern.ch) , February 2010
// -------------------------------------------------------------------
#ifndef TBTPCNV_TBTRACKINFOCNV_P1
#define TBTPCNV_TBTRACKINFOCNV_P1

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TBTPCnv/TBTrackInfo_p1.h"

class MsgStream;
class TBTrackInfo;


class TBTrackInfoCnv_p1  : public T_AthenaPoolTPCnvBase<TBTrackInfo, TBTrackInfo_p1>
{
public:

  TBTrackInfoCnv_p1() {}

  virtual void          persToTrans(const TBTrackInfo_p1* pers, TBTrackInfo* trans, MsgStream &log);
  virtual void          transToPers(const TBTrackInfo* trans, TBTrackInfo_p1* pers, MsgStream &log);
};



#endif
