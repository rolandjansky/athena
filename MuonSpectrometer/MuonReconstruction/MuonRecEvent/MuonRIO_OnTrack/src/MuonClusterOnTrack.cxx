/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonClusterOnTrack.cxx
//   Implementation file for class MuonClusterOnTrack
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 06/07/2004  Ketevi A. Assamagan
// adapted from  Veronique Boisvert
///////////////////////////////////////////////////////////////////

#include <new>
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include <iostream>

namespace Muon {

// Default constructor:
MuonClusterOnTrack::MuonClusterOnTrack():
    Trk::RIO_OnTrack(),
    m_globalPosition(),
    m_positionAlongStrip(0)
{ }

// copy constructor:
  MuonClusterOnTrack::MuonClusterOnTrack( const MuonClusterOnTrack& rot) :
    Trk::RIO_OnTrack(rot),
    m_globalPosition()
{ 
  m_positionAlongStrip = rot.m_positionAlongStrip;
  if (rot.m_globalPosition) m_globalPosition.store(std::make_unique<const Amg::Vector3D>(*rot.m_globalPosition));
}


// Constructor with parameters
MuonClusterOnTrack::MuonClusterOnTrack
    (
    const Trk::LocalParameters& locpos, 
    const Amg::MatrixX& locerr, 
    const Identifier& id,
     const double positionAlongStrip
    )
   : 
    RIO_OnTrack( locpos, locerr, id), //call base class constructor
    m_globalPosition(),
    m_positionAlongStrip(positionAlongStrip)
{
}

// Destructor:
MuonClusterOnTrack::~MuonClusterOnTrack()
{ 
}

// assignment operator:
MuonClusterOnTrack& MuonClusterOnTrack::operator=( const MuonClusterOnTrack& rot){
  if ( &rot != this) {
    Trk::RIO_OnTrack::operator=(rot);//base class ass. op.
    m_positionAlongStrip = rot.m_positionAlongStrip;
    if (rot.m_globalPosition) m_globalPosition.store(std::make_unique<const Amg::Vector3D>(*rot.m_globalPosition));
    else if (m_globalPosition) m_globalPosition.release().reset();
  }
  return *this;
}

MsgStream& MuonClusterOnTrack::dump( MsgStream&    stream) const
{
  stream << MSG::INFO<<"MuonClusterOnTrack {"<<std::endl;

  Trk::RIO_OnTrack::dump(stream); 
  
  stream << "Global position (x,y,z) = (";
  stream  <<this->globalPosition().x()<<", "
          <<this->globalPosition().y()<<", "
          <<this->globalPosition().z()<<")"<<std::endl;
  stream << "Position along strip: "<<m_positionAlongStrip<<std::endl;
  stream<<"}"<<endmsg;
  return stream;
}

std::ostream& MuonClusterOnTrack::dump( std::ostream&    stream) const
{
  stream << "MuonClusterOnTrack {"<<std::endl;
  std::ios_base::fmtflags originalFormat = stream.flags();

  Trk::RIO_OnTrack::dump(stream); 

  stream << std::setiosflags(std::ios::fixed)<< std::setprecision(3);

  stream << "Position along strip: "<<m_positionAlongStrip<<std::endl;
  
  stream<<"}"<<std::endl;
  stream.flags( originalFormat );
  
  return stream;
}

const Amg::Vector3D& MuonClusterOnTrack::globalPosition() const {
  if (detectorElement()==0){
    // Not much we can do here - no detelement, so just return 0,0,0
    m_globalPosition.set(std::make_unique<const Amg::Vector3D>(0.0,0.0,0.0));
  }

  if (not m_globalPosition) {
    // calculate global position from the position of the strip and the position along the strip
    Amg::Vector2D lpos;
    if( localParameters().contains( Trk::locX ) )
      lpos = Amg::Vector2D( localParameters().get(Trk::locX), m_positionAlongStrip );
    else
      lpos = Amg::Vector2D( m_positionAlongStrip, localParameters().get(Trk::locY) );
    Amg::Vector3D gpos(0.,0.,0.);
    detectorElement()->surface( identify() ).localToGlobal(lpos, gpos, gpos);

    m_globalPosition.set(std::make_unique<const Amg::Vector3D>(gpos));
  }
  
  return *m_globalPosition;
}

}


