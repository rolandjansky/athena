/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKTRACKFITTERSTATSCNV_H
#define FTKTRACKFITTERSTATSCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "FTKTrackFitterStatsCnv_p1.h" // last persistency version

#define private public
#define protected public
#include "TrigFTKPool/FTKTrackFitterStats_p1.h"
#include "TrigFTKPool/FTKTrackFitterStats.h"
#undef public
#undef protected

#include <memory>

class FTKTrackFitterStatsCnv : public T_AthenaPoolCustomCnv<FTKTrackFitterStats,FTKTrackFitterStats_p1> {
private:
  friend class CnvFactory<FTKTrackFitterStatsCnv>;

  FTKTrackFitterStatsCnv_p1 m_TPconverter;

public:
  FTKTrackFitterStatsCnv(ISvcLocator *svcloc) :
    T_AthenaPoolCustomCnv<FTKTrackFitterStats,FTKTrackFitterStats_p1>::T_AthenaPoolCustomCnv(svcloc) {}

  virtual FTKTrackFitterStats_p1* createPersistent(FTKTrackFitterStats *transObj);
  virtual FTKTrackFitterStats* createTransient();

};
#endif // FTKTRACKFITTERSTATSCNV_H

