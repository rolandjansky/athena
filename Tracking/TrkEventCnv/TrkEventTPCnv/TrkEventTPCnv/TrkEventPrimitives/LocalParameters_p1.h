/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   LocalParameters_p1.h
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------

#ifndef LOCAL_PARAMETERS_P1_TRK_H
#define LOCAL_PARAMETERS_P1_TRK_H

#include <vector>

namespace Trk
{
   class LocalParameters_p1
   {
  public:
     LocalParameters_p1() : m_parameterKey(0) {}

     int                   m_parameterKey;
     std :: vector<float>  m_vec;     
   };
}

#endif // LOCAL_PARAMETERS_P1_TRK_H
