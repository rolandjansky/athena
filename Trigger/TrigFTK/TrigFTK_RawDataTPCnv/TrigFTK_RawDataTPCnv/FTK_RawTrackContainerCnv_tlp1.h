/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGFTK_RAWDATATPCNV_FTK_RAWTRACKCONTAINERCNV_tlp1_H
#define TRIGFTK_RAWDATATPCNV_FTK_RAWTRACKCONTAINERCNV_tlp1_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "FTK_RawTrackContainerCnv_p1.h"
#include "FTK_RawTrackCnv_p1.h"
#include "FTK_RawTrackContainer_tlp1.h"

class FTK_RawTrackContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter<FTK_RawTrackContainerCnv_p1, FTK_RawTrackContainer_tlp1>
{

 public:

  FTK_RawTrackContainerCnv_tlp1();
  virtual ~FTK_RawTrackContainerCnv_tlp1() {}

  virtual void setPStorage( FTK_RawTrackContainer_tlp1 *storage );


 protected:

  FTK_RawTrackContainerCnv_p1  m_FTK_RawTrackContainerCnv;
  FTK_RawTrackCnv_p1           m_FTK_RawTrackCnv;


};

#endif
