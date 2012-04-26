/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKAthTrackCnv_p1_h
#define FTKAthTrackCnv_p1_h

#include "TrigFTKPool/FTKAthTrack_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class FTKAthTrack;

class FTKAthTrackCnv_p1 : public T_AthenaPoolTPCnvBase<FTKAthTrack, FTKAthTrack_p1> {
public:
  FTKAthTrackCnv_p1() {;}

  virtual void persToTrans(const FTKAthTrack_p1 *persObj, FTKAthTrack *transObj, MsgStream&);
  virtual void transToPers(const FTKAthTrack *transObj, FTKAthTrack_p1 *persObj, MsgStream&);
};
#endif // FTKAthTrackCnv_p1_h
