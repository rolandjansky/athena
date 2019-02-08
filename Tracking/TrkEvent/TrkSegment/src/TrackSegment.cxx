/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackSegment.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkSegment/TrackSegment.h"
#include "TrkSurfaces/Surface.h"
#include "GaudiKernel/MsgStream.h"
#include "AthContainers/DataVector.h"


// default constructor
Trk::TrackSegment::TrackSegment()
    :
    Trk::Segment(),
    m_associatedSurface(0),
    m_globalPosition(0)
{
}

// copy constructor
Trk::TrackSegment::TrackSegment(const Trk::TrackSegment& tseg) 
    :
    Trk::Segment(tseg),
    m_associatedSurface(tseg.m_associatedSurface ? tseg.m_associatedSurface->clone() : 0),
    m_globalPosition(tseg.m_globalPosition ? new Amg::Vector3D(*tseg.m_globalPosition) : 0)
{
}


// explicit constructor
Trk::TrackSegment::TrackSegment(const Trk::LocalParameters& locpars,
                                const Amg::MatrixX& locerr,
                                const Trk::Surface*  sf,
                                DataVector<const MeasurementBase>* crots,
                                FitQuality* fqual,
				Segment::Author author)
    :
Trk::Segment(locpars, locerr, crots, fqual,author),
    m_associatedSurface(sf),
    m_globalPosition(0)
{
}


// move constructor
Trk::TrackSegment::TrackSegment(Trk::TrackSegment&& tseg)
     : Trk::Segment(tseg)
{
     m_associatedSurface = tseg.m_associatedSurface;
     tseg.m_associatedSurface = nullptr;
     m_globalPosition = tseg.m_globalPosition;
     tseg.m_globalPosition = nullptr;
}


// move assignment operator
Trk::TrackSegment& Trk::TrackSegment::operator=(Trk::TrackSegment&& tseg) {
   if (this!=&tseg){
     Trk::Segment::operator=(tseg);
     delete m_associatedSurface;
     m_associatedSurface = tseg.m_associatedSurface;
     tseg.m_associatedSurface = nullptr;
     delete m_globalPosition;
     m_globalPosition = tseg.m_globalPosition;
     tseg.m_globalPosition = nullptr;
   }
   return (*this);
 }



Trk::TrackSegment& Trk::TrackSegment::operator=(const Trk::TrackSegment& tseg)
{
   if (this!=&tseg){
    // assingment operator of base class
    Trk::Segment::operator=(tseg); 
    if (m_associatedSurface && !m_associatedSurface->associatedDetectorElement()) delete m_associatedSurface;
    delete m_globalPosition;
    
    if (tseg.m_associatedSurface){
       // copy only if surface is not one owned by a detector Element
       m_associatedSurface = (!tseg.m_associatedSurface->associatedDetectorElement()) ? tseg.m_associatedSurface->clone() : tseg.m_associatedSurface;
    } else { m_associatedSurface = 0; }
    m_globalPosition = tseg.m_globalPosition ? new Amg::Vector3D(*tseg.m_globalPosition) : 0; 
  }
  return (*this);
}

Trk::TrackSegment::~TrackSegment()
{
  if (m_associatedSurface && !m_associatedSurface->associatedDetectorElement()) {delete m_associatedSurface;}
  delete m_globalPosition;
}

const Amg::Vector3D& Trk::TrackSegment::globalPosition() const
{ if (m_globalPosition) return (*m_globalPosition);
   m_globalPosition = m_associatedSurface->localToGlobal(localParameters()); 
   return (*m_globalPosition);  
} 

MsgStream& Trk::TrackSegment::dump( MsgStream& out ) const
 {
    out << "Trk::TrackSegment (generic track segment) " << std::endl;
    out << "  - it contains   : " << numberOfMeasurementBases() << " RIO_OnTrack object" << std::endl;
    out << "  - parmaters     : " << std::endl;
    out << "  - parameter key : " << std::endl;
    //TODO - out proper output (see MuonSegment) EJWM
    return out;
 }  

std::ostream& Trk::TrackSegment::dump( std::ostream& out ) const
 {
    out << "Trk::TrackSegment (generic track segment) " << std::endl;
    out << "  - it contains   : " << numberOfMeasurementBases() << " RIO_OnTrack object" << std::endl;
    out << "  - parmaters     : " << std::endl;
    out << "  - parameter key : " << std::endl;
    //TODO - out proper output (see MuonSegment) EJWM
    return out;
 }

