/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiClusterOnTrack.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkSurfaces/Surface.h"
#include "InDetPrepRawData/SiCluster.h"
#include "GaudiKernel/MsgStream.h"
#include <memory>
#include <new>
#include <ostream>
#include <typeinfo>


// Constructor with parameters - global position not specified here
InDet::SiClusterOnTrack::SiClusterOnTrack( const Trk::LocalParameters& locpars, 
                                           const Amg::MatrixX& locerr, 
                                           const IdentifierHash& idDE,
                                           const Identifier& id,
                                           bool isbroad) : 
  RIO_OnTrack(locpars, locerr, id), //call base class constructor
  m_idDE(idDE),
  m_globalPosition{},
  m_isbroad(isbroad)
{}

// Constructor with parameters - global position specified
InDet::SiClusterOnTrack::SiClusterOnTrack( const Trk::LocalParameters& locpars,
                                           const Amg::MatrixX& locerr,
                                           const IdentifierHash& idDE,
                                           const Identifier& id,
                                           const Amg::Vector3D& globalPosition,
                                           bool isbroad) 
    : 
    RIO_OnTrack(locpars, locerr, id), //call base class constructor
    m_idDE(idDE),
    m_globalPosition(std::make_unique<Amg::Vector3D>(globalPosition)),
    m_isbroad(isbroad)
{}


// Destructor:
InDet::SiClusterOnTrack::~SiClusterOnTrack()
{ 
}

// Default constructor:
InDet::SiClusterOnTrack::SiClusterOnTrack():
    Trk::RIO_OnTrack(),
    m_idDE(),
    m_globalPosition{},
    m_isbroad(false)
{}

// copy constructor:
InDet::SiClusterOnTrack::SiClusterOnTrack( const SiClusterOnTrack& rot)
    :
    RIO_OnTrack(rot),
    m_idDE(rot.m_idDE),
    m_globalPosition{},
    m_isbroad(rot.m_isbroad)
{
  if (rot.m_globalPosition) {
    m_globalPosition.set(std::make_unique<const Amg::Vector3D>(*(rot.m_globalPosition)));
  }
}

// assignment operator:
InDet::SiClusterOnTrack& InDet::SiClusterOnTrack::operator=( const SiClusterOnTrack& rot){
    if ( &rot != this) {
       delete m_globalPosition.release().get();
       Trk::RIO_OnTrack::operator=(rot);
       m_idDE           = rot.m_idDE;
       if (rot.m_globalPosition) {
         m_globalPosition.set(std::make_unique<const Amg::Vector3D>(*(rot.m_globalPosition)));
       }
       m_isbroad        = rot.m_isbroad;
    }
    return *this;
}




const Amg::Vector3D& InDet::SiClusterOnTrack::globalPosition() const
{ 
  if (not m_globalPosition) {
    m_globalPosition.set(std::unique_ptr<const Amg::Vector3D>(associatedSurface().localToGlobal(localParameters())));
  }
  return (*m_globalPosition);
}

MsgStream& InDet::SiClusterOnTrack::dump( MsgStream& sl ) const
{

    sl << "SiClusterOnTrack {" << endmsg;
    Trk::RIO_OnTrack::dump(sl);

    sl << "Global position (x,y,z) = (";
    this->globalPosition();
    if (m_globalPosition)
    {
       sl  <<this->globalPosition().x()<<", "
               <<this->globalPosition().y()<<", "
               <<this->globalPosition().z()<<")"<<endmsg;
    } else {
        sl<<"NULL!), "<<endmsg;
    }
    sl<<"}"<<endmsg;
    return sl;
}

std::ostream& InDet::SiClusterOnTrack::dump( std::ostream& sl ) const
{
    sl << "SiClusterOnTrack {"<<std::endl;

    Trk::RIO_OnTrack::dump(sl);

    sl << "Global position (x,y,z) = (";
    this->globalPosition();
    if (m_globalPosition)
    {
        sl  <<this->globalPosition().x()<<", "
            <<this->globalPosition().y()<<", "
            <<this->globalPosition().z()<<")"<<std::endl;
    } else {
        sl<<"NULL!), "<<std::endl;
    }

    sl<<"}"<<std::endl;
    return sl;
}






