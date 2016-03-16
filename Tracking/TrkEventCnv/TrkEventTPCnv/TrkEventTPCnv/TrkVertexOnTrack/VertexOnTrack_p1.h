/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VERTEXONTRACK_P1_TRK_H
#define VERTEXONTRACK_P1_TRK_H

#include "AthenaPoolUtilities/TPObjRef.h"

namespace Trk
{
   class VertexOnTrack_p1
   {
  public:
     VertexOnTrack_p1() {}

     TPObjRef m_localParams;  // mutable const LocalParameters*

     TPObjRef m_localErrMat; // mutable const ErrorMatrix*

     TPObjRef m_associatedSurface;// mutable const Surface*
   };
}

#endif // VERTEXONTRACK_P1_TRK_H
