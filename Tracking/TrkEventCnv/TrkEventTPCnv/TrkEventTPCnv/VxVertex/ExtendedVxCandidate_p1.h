/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXTENDEDVXCANDIDATE_P1_H
#define EXTENDEDVXCANDIDATE_P1_H

/**
 * Persistent representation of the VxVertex/ExtendedVxCandidate class
 *
 * Kirill Prokofiev <Kirill.Prokofiev@cern.ch> 
 * 
 * March 2007
 *
 */

#include "AthenaPoolUtilities/TPObjRef.h"

namespace Trk
{

 class ExtendedVxCandidate_p1
 {
   public:
  
    ExtendedVxCandidate_p1(){}
    
//VxCandidate itself  
    TPObjRef m_vxCandidate;

//Full covariance of the fit
    TPObjRef m_fullCovariance;
 
 }; //end of class definitions
 
}//end of namespace definitions

#endif // EXTENDEDVXCANDIDATE_P1_H
