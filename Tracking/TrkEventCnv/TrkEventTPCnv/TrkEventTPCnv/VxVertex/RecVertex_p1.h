/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECVERTEX_P1_H
#define RECVERTEX_P1_H

/**
 * Persistent representation of the VxVertex/RecVertex class
 *
 * Kirill Prokofiev <Kirill.Prokofiev@cern.ch> 
 * 
 * July 2006
 * January 2007 corrected for the new TP split schema
 *
 */

#include "AthenaPoolUtilities/TPObjRef.h"

namespace Trk
{
 class RecVertex_p1 
 {
 
   public:
 
//default constructor 
    RecVertex_p1(){}
     
//Vertex  
    TPObjRef vtx;
    
//Error matrix    
    TPObjRef m_positionError;
    
//Fit quality
    TPObjRef m_fitQuality;   
    
 };//end of class definitions
}//end of namespace definitions

#endif //RECVERTEX_P1_H
