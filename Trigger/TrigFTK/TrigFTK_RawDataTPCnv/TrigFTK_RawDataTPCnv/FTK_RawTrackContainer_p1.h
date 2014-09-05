/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETEVENT_FTK_RAWTRACKCONTAINER_P1_H 
#define TRIGINDETEVENT_FTK_RAWTRACKCONTAINER_P1_H 

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>

class FTK_RawTrackContainer_p1{

 public:

  FTK_RawTrackContainer_p1() {}
  friend class FTK_RawTrackContainerCnv_p1;

 private:

  std::vector<TPObjRef> m_FTK_RawTrackVector;

};

#endif

