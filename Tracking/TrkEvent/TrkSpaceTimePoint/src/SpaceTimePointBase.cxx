/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkSpaceTimePoint/SpaceTimePointBase.h"

Trk::SpaceTimePointBase::SpaceTimePointBase(const float& t, const float& t_error, const float& weight) :
  m_time(t),
  m_errorTime(t_error),
  m_weight(weight)
{}

Trk::SpaceTimePointBase::SpaceTimePointBase( const Trk::SpaceTimePointBase& stpb){
    m_time = stpb.m_time;
    m_errorTime = stpb.m_errorTime;
    m_weight = stpb.m_weight;
}

Trk::SpaceTimePointBase& Trk::SpaceTimePointBase::operator=(const Trk::SpaceTimePointBase& stpb) {
  if (this!=&stpb){
    m_time = stpb.m_time;
    m_errorTime = stpb.m_errorTime;
    m_weight = stpb.m_weight;
  }
 return *this;
}

