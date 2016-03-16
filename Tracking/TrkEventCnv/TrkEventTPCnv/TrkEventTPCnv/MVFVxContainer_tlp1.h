/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MVFVXCONTAINER_TLP1_TRK_H
#define MVFVXCONTAINER_TLP1_TRK_H

//-----------------------------------------
// VxMultiVertex
//-----------------------------------------
#include "TrkEventTPCnv/VxMultiVertex/MVFVxContainer_p1.h"
#include "TrkEventTPCnv/VxMultiVertex/MVFVxCandidate_p1.h"
#include "TrkEventTPCnv/VxMultiVertex/MVFVxTrackAtVertex_p1.h"

namespace Trk
{
 class MVFVxContainer_tlp1 
 {
 
  public:
  
   MVFVxContainer_tlp1(){}

//VxMultiVertex
   std::vector<MVFVxContainer_p1>           m_mVFVxContainers;
   std::vector<Trk::MVFVxCandidate_p1>      m_mVFVxCandidates;
   std::vector<Trk::MVFVxTrackAtVertex_p1>  m_mVFVxTrackAtVertices;

 };//end of class definitions
} //end of namespace definitions


#endif //MVFVXCONTAINER_TLP1_TRK_H
