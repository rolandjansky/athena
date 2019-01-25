/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackSurfaceIntersection.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


// Trk
#include "TrkExUtils/TrackSurfaceIntersection.h"
//Gaudi
#include "GaudiKernel/MsgStream.h"
//STD
#include <iostream>
#include <iomanip>

// initialize static serial number
unsigned long long	Trk::TrackSurfaceIntersection::s_serialNumber = 0;

// constructor
Trk::TrackSurfaceIntersection::TrackSurfaceIntersection(const Amg::Vector3D&	pos,
							const Amg::Vector3D&	dir,
							double			path)
  : m_position (pos),
    m_direction (dir),
    m_pathlength (path)
{
    m_serialNumber	= ++s_serialNumber;
}

Trk::TrackSurfaceIntersection::TrackSurfaceIntersection(const TrackSurfaceIntersection& other)
  : m_position (other.m_position),
    m_direction (other.m_direction),
    m_pathlength (other.m_pathlength)
{
    m_serialNumber	= ++s_serialNumber;
}

// destructor
Trk::TrackSurfaceIntersection::~TrackSurfaceIntersection()
{}

//Overload of << operator for both, MsgStream and std::ostream for debug output
MsgStream& Trk::operator << ( MsgStream& sl, const Trk::TrackSurfaceIntersection& tsfi)
{   
    const std::streamsize ss = sl.precision();
    sl << std::setiosflags(std::ios::fixed);
    sl << std::setprecision(7);
    sl << MSG::DEBUG << "Trk::TrackSurfaceIntersection  " << std::endl;
    sl << "    position  [mm]   =  (" << tsfi.position().x() << ", " << tsfi.position().y() << ", " << tsfi.position().z() << ")" << std::endl;
    sl << "    direction [mm]   =  (" << tsfi.direction().x() << ", " << tsfi.direction().y() << ", " << tsfi.direction().z() << ")" << std::endl;
    sl << "    delta pathlength =   " << tsfi.pathlength() << std::endl;
    sl.precision (ss);
    return sl;  
}

std::ostream& Trk::operator << ( std::ostream& sl, const Trk::TrackSurfaceIntersection& tsfi)
{
    const std::streamsize ss = sl.precision();
    sl << std::setiosflags(std::ios::fixed);
    sl << std::setprecision(7);
    sl << "Trk::TrackSurfaceIntersection  " << std::endl;
    sl << "    position  [mm]   =  (" << tsfi.position().x() << ", " << tsfi.position().y() << ", " << tsfi.position().z() << ")" << std::endl;
    sl << "    direction [mm]   =  (" << tsfi.direction().x() << ", " << tsfi.direction().y() << ", " << tsfi.direction().z() << ")" << std::endl;
    sl << "    delta pathlength =   " << tsfi.pathlength() << std::endl;  
    sl.precision (ss);
    return sl;
}   
