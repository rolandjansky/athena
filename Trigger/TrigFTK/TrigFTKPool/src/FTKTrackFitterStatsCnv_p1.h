/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKTrackFitterStatsCnv_p1_h
#define FTKTrackFitterStatsCnv_p1_h

#include "TrigFTKPool/FTKTrackFitterStats_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class FTKTrackFitterStats;

class FTKTrackFitterStatsCnv_p1 : public T_AthenaPoolTPCnvConstBase<FTKTrackFitterStats, FTKTrackFitterStats_p1> {
public:
  using base_class::transToPers;
  using base_class::persToTrans;


  FTKTrackFitterStatsCnv_p1() {;}

  virtual void persToTrans(const FTKTrackFitterStats_p1 *persObj, FTKTrackFitterStats *transObj, MsgStream&) const override;
  virtual void transToPers(const FTKTrackFitterStats *transObj, FTKTrackFitterStats_p1 *persObj, MsgStream&) const override;
};
#endif // FTKTrackFitterStatsCnv_p1_h
