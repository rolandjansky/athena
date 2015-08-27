/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATA_MeasuredAtaSurface_p1_TRK_H
#define ATA_MeasuredAtaSurface_p1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   MeasuredAtaSurface_p1.h
//
//-----------------------------------------------------------------------------

#include "AtaSurface_p1.h"

namespace Trk
{
   class MeasuredAtaSurface_p1 : public AtaSurface_p1 {
  public:
     MeasuredAtaSurface_p1() {}

     TPObjRef	m_errorMatrix;
   };
}

#endif 

