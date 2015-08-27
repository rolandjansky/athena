/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RIO_ON_TRACK_p2_TRK_H
#define RIO_ON_TRACK_p2_TRK_H

#include "AthenaPoolUtilities/TPObjRef.h"

#include "Identifier/Identifier.h"

namespace Trk
{
    class RIO_OnTrack_p2
    {
    public:
        RIO_OnTrack_p2() : m_id(0) {}

        unsigned long long  m_id;

     //LocalParameters_p1 *
        TPObjRef               m_localParams;
     //ErrorMatrix *
        TPObjRef               m_localErrMat;
    };
}

#endif 
