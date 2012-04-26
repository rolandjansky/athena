/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKATHTRACKCONTAINERCNV_H
#define FTKATHTRACKCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "FTKAthTrackContainerCnv_p1.h" // last persistency version

#define private public
#define protected public
#include "TrigFTKPool/FTKAthTrackContainer_p1.h"
#include "TrigFTKPool/FTKAthTrackContainer.h"
#undef public
#undef protected

#include <memory>

class FTKAthTrackContainerCnv : public T_AthenaPoolCustomCnv<FTKAthTrackContainer,FTKAthTrackContainer_p1> {
private:
  friend class CnvFactory<FTKAthTrackContainerCnv>;

  FTKAthTrackContainerCnv_p1 m_TPconverter;

public:
  FTKAthTrackContainerCnv(ISvcLocator *svcloc) :
    T_AthenaPoolCustomCnv<FTKAthTrackContainer,FTKAthTrackContainer_p1>::T_AthenaPoolCustomCnv(svcloc) {}

  virtual FTKAthTrackContainer_p1* createPersistent(FTKAthTrackContainer *transObj);
  virtual FTKAthTrackContainer* createTransient();

};
#endif // FTKATHTRACKCONTAINERCNV_H

