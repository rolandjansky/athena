/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACK_PARAMETERS_P1_TRK_H
#define TRACK_PARAMETERS_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   TrackParameters_p1.h
//
//-----------------------------------------------------------------------------

#include <vector>

namespace Trk
{
   class TrackParameters_p1
   {
  public:
     TrackParameters_p1() {}

     #ifdef UseFloatsIn_TrackParameters
     std::vector<float>  m_parameters;
     #else
     std::vector<double> m_parameters;
     #endif
   };
}

#endif // TRACK_PARAMETERS_P1_TRK_H
