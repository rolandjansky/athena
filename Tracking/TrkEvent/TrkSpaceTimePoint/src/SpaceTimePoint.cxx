/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkSpaceTimePoint/SpaceTimePoint.h"
#include "GaudiKernel/MsgStream.h"

Trk::SpaceTimePoint::SpaceTimePoint() :
  SpaceTimePointBase(0.0, 0.0, 0.0),
  m_position(Amg::Vector3D()),
  m_detectorID(Trk::TrackState::unidentified)
{

}

Trk::SpaceTimePoint::SpaceTimePoint
(const Amg::Vector3D& position, const float& t, const float& t_error, const float& weight, const Trk::TrackState::MeasurementType detector) :
  SpaceTimePointBase(t,t_error,weight),
  m_position(position),
  m_detectorID(detector)
{

}

/** MsgStream output */
MsgStream& operator << ( MsgStream& sl, const Trk::SpaceTimePoint& stp)
// if namespace, need to use MsgStream& Trk::operator :-(
{
  sl <<"SpaceTimePoint from ";
  switch (stp.measurementType()) 
    {
    case Trk::TrackState::TRT:
      sl << "TRT "; break;
    case Trk::TrackState::TileCal:
      sl << "Tile"; break;
    case Trk::TrackState::MDT:
      sl << "MDT "; break;
    case Trk::TrackState::RPC:
      sl << "RPC "; break;
    case Trk::TrackState::CSC:
      sl << "CSC "; break;
    default:
      sl << "??? "; break;
    }
  sl << endreq << "at "<< stp.globalPosition() << ", time= "<<stp.time()
     << " +/- "<<stp.errorTime()<<", w=" << stp.weight();
  sl << endreq;
  return sl;
}

/** MsgStream output */
std::ostream& operator << ( std::ostream& sl, const Trk::SpaceTimePoint& stp)
// if namespace, need to use MsgStream& Rec::operator :-(
{
  sl <<"SpaceTimePoint from ";
  switch (stp.measurementType()) 
    {
    case Trk::TrackState::TRT:
      sl << "TRT "; break;
    case Trk::TrackState::TileCal:
      sl << "Tile"; break;
    case Trk::TrackState::MDT:
      sl << "MDT "; break;
    case Trk::TrackState::RPC:
      sl << "RPC "; break;
    case Trk::TrackState::CSC:
      sl << "CSC "; break;
    default:
      sl << "??? "; break;
    }
  sl << " at "<< stp.globalPosition() << ", time= "<<stp.time()
     << " +/- "<<stp.errorTime()<<", w=" << stp.weight();
  sl << std::endl;
  return sl;
}


