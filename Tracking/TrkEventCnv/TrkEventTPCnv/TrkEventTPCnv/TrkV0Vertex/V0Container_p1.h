/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef V0CONTAINER_P1_TRK_H
#define V0CONTAINER_P1_TRK_H
//-----------------------------------------------------------------------------
//
// file:   V0Container_p1.h
//
// March 2007. 
// Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
//-----------------------------------------------------------------------------

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>

namespace Trk
{

 class V0Container_p1 : public std::vector<TPObjRef>
 {
   public:
   
    V0Container_p1(){}
    
 };// end of class definitions
}// end of namespace definitions


#endif
