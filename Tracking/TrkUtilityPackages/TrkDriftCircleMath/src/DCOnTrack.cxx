/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkDriftCircleMath/DCOnTrack.h"


namespace TrkDriftCircleMath {


std::ostream& operator<<( std::ostream& os, const TrkDriftCircleMath::DCOnTrack& dc )
{
  double pull = dc.residual()/dc.dr();
  
  os << dc.position() << std::setw(6) << " r " << dc.r() << " dr " << dc.dr() << " id " << dc.id();
  if (dc.DriftCircle::state() == TrkDriftCircleMath::DriftCircle::EarlyHit) os << " EarlyHit";
  else if (dc.DriftCircle::state() == TrkDriftCircleMath::DriftCircle::InTime) os << " InTime";
  else if (dc.DriftCircle::state() == TrkDriftCircleMath::DriftCircle::LateHit) os << " LateHit";
  else if (dc.DriftCircle::state() == TrkDriftCircleMath::DriftCircle::EmptyTube) os << " EmptyTube";
  else if (dc.DriftCircle::state() == TrkDriftCircleMath::DriftCircle::NotCrossed) os << " NotCrossed";
  else os << " unknown state";
  os << " res " << dc.residual() << " pull " << pull;
  if( dc.dr() != dc.drPrecise() ) {
    os << " pull(2) " << dc.residual()/dc.drPrecise();
  }
  os << " errTrk " << dc.errorTrack();
  if( dc.state() == TrkDriftCircleMath::DCOnTrack::OnTrack ) os << " OnTrack ";
  else if( dc.state() == TrkDriftCircleMath::DCOnTrack::Delta ) os << " Delta ";
  else if( dc.state() == TrkDriftCircleMath::DCOnTrack::OutOfTime ) os << " OutOfTime ";
  else if( dc.state() == TrkDriftCircleMath::DCOnTrack::CloseDC ) os << " CloseDC ";
    return os;
}

MsgStream& operator<< (MsgStream& os, const TrkDriftCircleMath::DCOnTrack& dc)
{
  if (dc.DriftCircle::state() == TrkDriftCircleMath::DriftCircle::EarlyHit) os << " EarlyHit";
  else if (dc.DriftCircle::state() == TrkDriftCircleMath::DriftCircle::InTime) os << " InTime";
  else if (dc.DriftCircle::state() == TrkDriftCircleMath::DriftCircle::LateHit) os << " LateHit";
  else if (dc.DriftCircle::state() == TrkDriftCircleMath::DriftCircle::EmptyTube) os << " EmptyTube";
  else if (dc.DriftCircle::state() == TrkDriftCircleMath::DriftCircle::NotCrossed) os << " NotCrossed";
  else os << " unknown state";
  os << std::setw(7) << " res " << dc.residual() 
     << std::setw(7) << " pull " << dc.residual()/dc.dr();
  if (dc.state() == TrkDriftCircleMath::DCOnTrack::OnTrack) os << " OnTrack ";
  else if (dc.state() == TrkDriftCircleMath::DCOnTrack::Delta) os << " Delta ";
  else if (dc.state() == TrkDriftCircleMath::DCOnTrack::OutOfTime) os << " OutOfTime ";
  else if (dc.state() == TrkDriftCircleMath::DCOnTrack::CloseDC) os << " CloseDC ";
  return os;
}


} // namespace TrkDriftCircleMath
