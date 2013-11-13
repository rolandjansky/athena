/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_IPTRACKINFOCNV_P1_H
#define JETTAGINFOTPCNV_IPTRACKINFOCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"

#include "JetTagInfoTPCnv/IPTrackInfo_p1.h"
#include "JetTagInfo/IPTrackInfo.h"

class MsgStream;

namespace Analysis {

  class IPTrackInfoCnv_p1  : public T_AthenaPoolTPCnvBase<IPTrackInfo, IPTrackInfo_p1>  {
  public:
    IPTrackInfoCnv_p1()
      {}

    virtual void   persToTrans(const IPTrackInfo_p1* persObj, IPTrackInfo* transObj, MsgStream &log);
    virtual void   transToPers(const IPTrackInfo* transObj, IPTrackInfo_p1* persObj, MsgStream &log);

  private:
    ElementLinkCnv_p1<ElementLink<Rec::TrackParticleContainer> > m_elementLinkCnv;

  };
}
#endif

