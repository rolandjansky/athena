/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DistanceSolution.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

//Trk
#include "TrkSurfaces/DistanceSolution.h"

// default constructor
Trk::DistanceSolution::DistanceSolution() :
  m_num(),
  m_first(),
  m_second(),
  m_current(),
  m_signedDist()
{}

// constructor 
Trk::DistanceSolution::DistanceSolution(int num, double current, bool signedDist, double first, double second) :
  m_num(num),
  m_first(first),
  m_second(second),
  m_current(current),
  m_signedDist(signedDist)
{}


