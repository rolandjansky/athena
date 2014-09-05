/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGFTK_RAWDATATPCNV_FTK_RAWTRACKCONTAINER_tlp4_H
#define TRIGFTK_RAWDATATPCNV_FTK_RAWTRACKCONTAINER_tlp4_H

#include "FTK_RawTrackContainer_p1.h"
#include "FTK_RawTrack_p1.h"

class FTK_RawTrackContainer_tlp1
{
 public:
  
  FTK_RawTrackContainer_tlp1() {}
  friend class FTK_RawTrackContainerCnv_tlp1;
  
 private:
  
  std::vector< FTK_RawTrackContainer_p1 >  m_FTK_RawTrackContainers;
  std::vector< FTK_RawTrack_p1 >           m_FTK_RawTracks;

};

#endif
