/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MVFVXTRACKATVERTEX_P1_H
#define MVFVXTRACKATVERTEX_P1_H

/**
 * Persistent representation of class VxMultiVertex/MVFVxTrackAtVertex
 *
 * Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
 *
 * January 2006 Modified for the TP split version 2
 */


#include "AthenaPoolUtilities/TPObjRef.h"
namespace Trk
{
 class MVFVxTrackAtVertex_p1
 
 {
   public:

    MVFVxTrackAtVertex_p1(){}
    
    TPObjRef m_vxTrackAtVertex;
    
    
 };//end of class definitions
    
} //end of namespace definitions
 
#endif //VXTRACKATVERTEX_P1_H
