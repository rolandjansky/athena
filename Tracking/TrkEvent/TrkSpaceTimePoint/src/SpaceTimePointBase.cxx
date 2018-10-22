/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkSpaceTimePoint/SpaceTimePointBase.h"

Trk::SpaceTimePointBase::SpaceTimePointBase(const float& t, const float& t_error, const float& weight) :
  m_time(t),
  m_errorTime(t_error),
  m_weight(weight)
{}


