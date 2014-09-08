/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECVERTEX_P2_H
#define RECVERTEX_P2_H


#include "AthenaPoolUtilities/TPObjRef.h"
#include "TrkEventTPCnv/TrkEventPrimitives/FitQuality_p1.h"
namespace Trk
{
 class RecVertex_p2 
 {
 
   public:
 
//default constructor 
    RecVertex_p2(){}
     
//Vertex  
    // TPObjRef vtx;// need to move to this! spare 3 baskets per collection  
    float  m_position[3];
    
//Error matrix    
    TPObjRef m_positionError;
    
//Fit quality
    FitQuality_p1 m_fitQuality;   
    
 };
}

#endif
