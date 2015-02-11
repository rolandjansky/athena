/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGFTK_RAWDATATPCNV_TRIGINDETEVENTTPCNVDICT_H
#define TRIGFTK_RAWDATATPCNV_TRIGINDETEVENTTPCNVDICT_H

#include "TrigFTK_RawDataTPCnv/FTK_RawTrack_p1.h"
#include "TrigFTK_RawDataTPCnv/FTK_RawTrackContainer_p1.h"
#include "TrigFTK_RawDataTPCnv/FTK_RawTrackContainer_tlp1.h"

//dummy instances of all objects using std::vector<TPObjRef> for the dictionary
struct dummy_TrigFTK_RawDataTPCnvDict
{

  std::vector<FTK_RawTrackContainer_p1>       m_dummyFTK_RawTrackContainer_p1;
  std::vector<FTK_RawTrack_p1>                m_dummyFTK_RawTrack_p1;

};

#endif
