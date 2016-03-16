/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VXCANDIDATE_P1_H
#define VXCANDIDATE_P1_H

/**
 * Persistent representation of class VxVertex/VxTrackAtVertex
 *
 * Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
 *
 * July 2006
 * January 2006 Modified for the TP split version 2
 */

#include "AthenaPoolUtilities/TPObjRef.h"

namespace Trk
{
 class VxCandidate_p1
 
 {
 
  public:
  
   VxCandidate_p1() : m_vertexType(-99) {}
   
   TPObjRef m_recVertex;
   std::vector<TPObjRef> m_vxTrackAtVertex;
   int m_vertexType; 
 };


}//end of namespace definitions 
 

#endif //VXCANDIDATE_P1_H
