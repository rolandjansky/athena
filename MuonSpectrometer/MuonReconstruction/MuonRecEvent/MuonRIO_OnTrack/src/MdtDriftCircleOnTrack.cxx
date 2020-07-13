/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MdtDriftCircleOnTrack.cxx
//   Implementation file for class MdtDriftCircleOnTrack
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 06/07/2004 Ketevi A. Assamagan
// adapted from Veronique Boisvert
///////////////////////////////////////////////////////////////////

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/StatusCode.h"
#include <cmath>


// Full Constructor :
Muon::MdtDriftCircleOnTrack::MdtDriftCircleOnTrack( 
    const Muon::MdtPrepData*        RIO, 
    const Trk::LocalParameters&     locPos, 
    const Amg::MatrixX&             errDriftRadius, 
    const double                    driftTime,
    const Trk::DriftCircleStatus    status,  
    const Amg::Vector3D&            predictedTrackDirection, 
    double                          positionAlongWire, 
    const MuonDriftCircleErrorStrategy& errorStrategy,
    const Trk::StraightLineSurface* surface) 
  :
  RIO_OnTrack( locPos, errDriftRadius, RIO->identify() ), //call base class constructor
  m_status(status),
  m_globalPosition(),
  m_saggedSurface(surface),
  m_detEl( RIO->detectorElement() ),
  m_localAngle(0.0), 
  m_positionAlongWire(positionAlongWire),
  m_driftTime(driftTime),
  m_errorStrategy(errorStrategy)
{
  assert(RIO!=0); //probably redundant because used to set m_detEl
  assert(m_status!=Trk::UNDECIDED); // use of this constructor implies that the side is decided

  const Trk::StraightLineSurface* slsf = dynamic_cast<const Trk::StraightLineSurface*>(&(m_detEl->surface(RIO->identify())));
 
  if(slsf) m_globalPosition.store(std::unique_ptr<const Amg::Vector3D>(slsf->localToGlobal(locPos, predictedTrackDirection, positionAlongWire)));
  Amg::Vector3D loc_gDirection = predictedTrackDirection; 
   
  //scaling the direction with drift radius   
  if(driftRadius() !=0.)   { 
    // Set loc_gDirection's perpendicular distance equal to driftRadius
    Amg::setPerp(loc_gDirection,driftRadius());
    float ratio = loc_gDirection.x()/driftRadius();
    float calc_angle = (ratio >= 1.) ?  0. : acos(ratio);
    m_localAngle = (loc_gDirection.y()<0.) ? 2*M_PI - calc_angle : calc_angle;
  }
  m_rio.setElement(RIO);
}

// Partial Constructor :
Muon::MdtDriftCircleOnTrack::MdtDriftCircleOnTrack( 
        const Muon::MdtPrepData*        RIO, 
        const Trk::LocalParameters&     locPos, 
        const Amg::MatrixX&             errDriftRadius, 
        const double                    driftTime,
        const Trk::DriftCircleStatus    status,  
        double                          positionAlongWire, 
        const MuonDriftCircleErrorStrategy& errorStrategy,
        const Trk::StraightLineSurface* surface) 
    :
    RIO_OnTrack( locPos, errDriftRadius, RIO->identify() ), //call base class constructor
    m_status(status),
    m_globalPosition(),
    m_saggedSurface(surface),
    m_detEl( RIO->detectorElement() ),
    m_localAngle(99.0), // TODO - Remove dummy value,
    m_positionAlongWire(positionAlongWire),
    m_driftTime(driftTime),
    m_errorStrategy(errorStrategy)
{
    assert(RIO!=0); //probably redundant because used to set m_detEl
    assert(m_status!=Trk::DECIDED); // use of this constructor implies that the side is not decided
    m_rio.setElement(RIO);
}

Muon::MdtDriftCircleOnTrack::MdtDriftCircleOnTrack( 
        const ElementLinkToIDC_MDT_Container& RIO, 
        const Trk::LocalParameters&     locPos, 
        const Amg::MatrixX&             errDriftRadius, 
        const Identifier&               id,
        const MuonGM::MdtReadoutElement* detEl,
        const double                    driftTime,
        const Trk::DriftCircleStatus    status,  
        double                          positionAlongWire, 
        float                           localAngle,
        const MuonDriftCircleErrorStrategy& errorStrategy,
        const Trk::StraightLineSurface* surface) 
    :
    RIO_OnTrack( locPos, errDriftRadius, id ), //call base class constructor
    m_status(status),
    m_rio(RIO),
    m_globalPosition(),
    m_saggedSurface(surface),
    m_detEl( detEl ),
    m_localAngle(localAngle),
    m_positionAlongWire(positionAlongWire),
    m_driftTime(driftTime),
    m_errorStrategy(errorStrategy)
{
}

// Destructor:
    Muon::MdtDriftCircleOnTrack::~MdtDriftCircleOnTrack()
    {
    // m_rio is owned by SG and so is not deleted.
    // m_detEl is owned by MuonGeoModel and so is not deleted.
        delete m_saggedSurface;
    }

// default constructor:
Muon::MdtDriftCircleOnTrack::MdtDriftCircleOnTrack()
    :
    RIO_OnTrack(),
    m_status(Trk::UNDECIDED),
    m_rio(),
    m_globalPosition(),
    m_saggedSurface(0),
    m_detEl(0),
    m_localAngle(0.0), 
    m_positionAlongWire(0.0),
    m_driftTime(0),
    m_errorStrategy(0)
{ 
}

//copy constructor:
Muon::MdtDriftCircleOnTrack::MdtDriftCircleOnTrack( const Muon::MdtDriftCircleOnTrack& rot)
    :
    RIO_OnTrack(rot),
    m_status(rot.m_status),
    m_rio(rot.m_rio),
    m_globalPosition(),
    m_saggedSurface(0),
    m_detEl(rot.m_detEl),
    m_localAngle(rot.m_localAngle), 
    m_positionAlongWire(rot.m_positionAlongWire),
    m_driftTime(rot.m_driftTime),
    m_errorStrategy(rot.m_errorStrategy)
{
    if ( rot.m_globalPosition ) m_globalPosition.store(std::make_unique<const Amg::Vector3D>(*rot.m_globalPosition));
    if ( rot.m_saggedSurface!=0 ) m_saggedSurface= new Trk::StraightLineSurface( *(rot.m_saggedSurface) );
}

//assignment operator:
Muon::MdtDriftCircleOnTrack& Muon::MdtDriftCircleOnTrack::operator=( const Muon::MdtDriftCircleOnTrack& rot)
{
    if ( &rot != this)
    {
        Trk::RIO_OnTrack::operator=(rot);//base class ass. op.
        if (rot.m_globalPosition) m_globalPosition.store(std::make_unique<const Amg::Vector3D>(*rot.m_globalPosition));
        else if (m_globalPosition) m_globalPosition.release().reset();
        delete m_saggedSurface;
        if( rot.m_saggedSurface!=0 )
            m_saggedSurface= new Trk::StraightLineSurface( *(rot.m_saggedSurface) );
        else
            m_saggedSurface=0;
        m_driftTime = rot.m_driftTime;
        m_status = rot.m_status;
        m_rio = rot.m_rio;
        m_detEl = rot.m_detEl;
        m_localAngle = rot.m_localAngle; 
        m_positionAlongWire = rot.m_positionAlongWire;
        m_errorStrategy= rot.m_errorStrategy;
    }
    return *this;
}
    
Muon::MdtDriftCircleOnTrack& Muon::MdtDriftCircleOnTrack::operator=( Muon::MdtDriftCircleOnTrack&& rot)
{
    if ( &rot != this)
    {
        Trk::RIO_OnTrack::operator=(std::move(rot));//base class ass. op.
        m_globalPosition = std::move(rot.m_globalPosition);

        delete m_saggedSurface;
        m_saggedSurface = rot.m_saggedSurface;
        rot.m_saggedSurface = nullptr;

        m_driftTime = rot.m_driftTime;
        m_status = rot.m_status;
        m_rio = rot.m_rio;
        m_detEl = rot.m_detEl;
        m_localAngle = rot.m_localAngle; 
        m_positionAlongWire = rot.m_positionAlongWire;
        m_errorStrategy= std::move(rot.m_errorStrategy);
    }
    return *this;
}
    
const Amg::Vector3D& Muon::MdtDriftCircleOnTrack::globalPosition() const
{
    if (not m_globalPosition){
        if (side()==Trk::NONE) {
            // side not defined, so we cannot determine the global position better than the position along the wire
            HepGeom::Point3D<double> loc3Dframe(0., 0., m_positionAlongWire);
            setGlobalPosition(loc3Dframe);
        } else {
            // get global position where track and drift radius intersect.
            double x = driftRadius()*std::sin(m_localAngle);
            double y = driftRadius()*std::cos(m_localAngle);
            HepGeom::Point3D<double> loc3Dframe(x, y, m_positionAlongWire);
            setGlobalPosition(loc3Dframe);
        }
    }
    return *m_globalPosition;
}

void Muon::MdtDriftCircleOnTrack::setGlobalPosition(HepGeom::Point3D<double>& loc3Dframe) const
{
    if (0!=m_saggedSurface){
        Amg::Vector3D tempFrame(loc3Dframe.x(), loc3Dframe.y(), loc3Dframe.z());
        m_globalPosition.set(std::make_unique<const Amg::Vector3D>(m_saggedSurface->transform() * tempFrame));
    } else {
        // hack because  MdtReadoutElement only returns generic pointer.
        const Trk::StraightLineSurface* slsf = dynamic_cast<const Trk::StraightLineSurface*>( &(associatedSurface()) );
        if (slsf) {
            Amg::Vector3D tempFrame(loc3Dframe.x(), loc3Dframe.y(), loc3Dframe.z());
            m_globalPosition.set(std::make_unique<const Amg::Vector3D>(slsf->transform() * tempFrame));
        }else{
            throw GaudiException(   "Dynamic_cast to StraightLineSurface failed!",
                "MdtDriftCircleOnTrack::setGlobalPosition()",
                StatusCode::FAILURE);
        }
    }
}

MsgStream& Muon::MdtDriftCircleOnTrack::dump( MsgStream&    stream) const
{
    stream << MSG::INFO<<"MdtDriftCircleOnTrack {"<<std::endl;
    Trk::RIO_OnTrack::dump(stream); 
    stream << "DriftTime: "<<m_driftTime<<std::endl;
    stream << "Status: "<<m_status<<std::endl;
    stream << "Global position (x,y,z) = (";
    stream  <<this->globalPosition().x()<<", "
            <<this->globalPosition().y()<<", "
            <<this->globalPosition().z()<<")"<<std::endl;

    if (m_saggedSurface!=0) {
        stream << "Sagged surface: "<<(*m_saggedSurface)<<std::endl;
    }else{
        stream << "Sagged surface: NONE "<<std::endl;
    }
    
    stream << "Position along wire: "<<positionAlongWire()<<", \tlocal angle="<<localAngle()<<std::endl;
    stream << "Creation strategy: "<<m_errorStrategy;
    stream<<"}"<<endmsg;
    return stream;
}

std::ostream& Muon::MdtDriftCircleOnTrack::dump( std::ostream&    stream) const
{
    stream << "MdtDriftCircleOnTrack {"<<std::endl;
    Trk::RIO_OnTrack::dump(stream); 
    stream << "DriftTime: "<<m_driftTime<<std::endl;
    stream << "Status: "<<m_status<<std::endl;
    //stream << "Global position (x,y,z) = (";
    // if ( &(this->globalPosition() )!=0 ){
    //     stream  <<this->globalPosition().x()<<", "
    //             <<this->globalPosition().y()<<", "
    //             <<this->globalPosition().z()<<")"<<std::endl;
    // } else {
    //     stream<<"NULL!), "<<std::endl;
    // }

    if (m_saggedSurface!=0) {
        stream << "Sagged surface: "<<(*m_saggedSurface)<<std::endl;
    }else{
        stream << "Sagged surface: NONE "<<std::endl;
    }
    stream << "Position along wire: "<<positionAlongWire()<<", \tlocal angle="<<localAngle()<<std::endl;
    // stream << "Creation strategy: "<<m_errorStrategy; //FIXME!
    stream<<"}"<<std::endl;
    return stream;
}



