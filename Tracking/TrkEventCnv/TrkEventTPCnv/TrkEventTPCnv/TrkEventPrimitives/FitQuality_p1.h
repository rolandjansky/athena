/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FIT_QUALITY_P1_TRK_H
#define FIT_QUALITY_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   FitQuality_p1.h
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//
// Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
// 2007-03-15 Correction for number of degrees of freedom to be float
//            The Chi2 also becomes float for space safety.
//-----------------------------------------------------------------------------

namespace Trk
{
   class FitQuality_p1
   {
   
  public:
     FitQuality_p1() : m_chiSquared(0.0), m_numberDoF(0.0){}

     float  m_chiSquared;
     float  m_numberDoF;
   };
}

#endif // FIT_QUALITY_P1_TRK_H
