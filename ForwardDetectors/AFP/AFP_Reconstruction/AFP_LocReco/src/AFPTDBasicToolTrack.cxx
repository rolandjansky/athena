/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file   AFPTDBasicToolTrack.cxx
/// @author Leszek Adamczyk  <leszek.adamczyk@agh.edu.pl>
/// @date   2021-11-11
/// 
/// @brief  Implementation file for AFPTDBasicToolTrack used in ToF tracks reconstruction.

#include <math.h>

// debug remove after debugging
#include<iostream>
// end remove after debugging

#include "AFP_LocReco/AFPTDBasicToolTrack.h"

AFPTDBasicToolTrack::AFPTDBasicToolTrack (int stationID, int TrainID , double Time, int Size, int Sat)
{
m_stationID = stationID;
m_TrainID = TrainID;
m_Time = Time;
m_Size = Size;
m_Sat = Sat;
}

