/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SI_WIDTH_P2_TRK_H
#define SI_WIDTH_P2_TRK_H

//-----------------------------------------------------------------------------
//
// file:   SiWidth_p2.h
//
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPosition_p1.h"


namespace InDet
{

class SiWidthCnv_p2;

class SiWidth_p2
  {
  public:
    SiWidth_p2() {};
    friend class SiWidthCnv_p2;
  private:
    unsigned int m_size;
    Trk::LocalPosition_p1 m_phirzWidth;   
  };

}
#endif // SI_WIDTH_TRK_H
