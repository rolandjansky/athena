/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MVFVXCANDIDATE_P1_H
#define MVFVXCANDIDATE_P1_H

/**
 * Persistent representation of class VxMultiVertex/MVFVxCandidate
 *
 * Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
 *
 * January 2006 Modified for the TP split version 2
 */


#include "AthenaPoolUtilities/TPObjRef.h"

namespace Trk
{
 class MVFVxCandidate_p1
 
 {
   public:

    MVFVxCandidate_p1(){}
    
    TPObjRef m_vxCandidate;    
    
 };//end of class definitions
    
} //end of namespace definitions
 
#endif //MVFVXCANDIDATE_P1_H
