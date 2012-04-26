/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKTrackFitterStatsCnv_p1_h
#define FTKTrackFitterStatsCnv_p1_h

#include "TrigFTKPool/FTKTrackFitterStats_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class FTKTrackFitterStats;

class FTKTrackFitterStatsCnv_p1 : public T_AthenaPoolTPCnvBase<FTKTrackFitterStats, FTKTrackFitterStats_p1> {
public:
  FTKTrackFitterStatsCnv_p1() {;}

  virtual void persToTrans(const FTKTrackFitterStats_p1 *persObj, FTKTrackFitterStats *transObj, MsgStream&);
  virtual void transToPers(const FTKTrackFitterStats *transObj, FTKTrackFitterStats_p1 *persObj, MsgStream&);
};
#endif // FTKTrackFitterStatsCnv_p1_h
