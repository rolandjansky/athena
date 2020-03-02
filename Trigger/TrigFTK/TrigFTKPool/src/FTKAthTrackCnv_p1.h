/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKAthTrackCnv_p1_h
#define FTKAthTrackCnv_p1_h

#include "TrigFTKPool/FTKAthTrack_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class FTKAthTrack;

class FTKAthTrackCnv_p1 : public T_AthenaPoolTPCnvConstBase<FTKAthTrack, FTKAthTrack_p1> {
public:
  using base_class::transToPers;
  using base_class::persToTrans;

  FTKAthTrackCnv_p1() {;}

  virtual void persToTrans(const FTKAthTrack_p1 *persObj, FTKAthTrack *transObj, MsgStream&) const override;
  virtual void transToPers(const FTKAthTrack *transObj, FTKAthTrack_p1 *persObj, MsgStream&) const override;
};
#endif // FTKAthTrackCnv_p1_h
