/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// VertexOnTrack.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

//Trk
#include "TrkVertexOnTrack/VertexOnTrack.h"
#include "VxVertex/RecVertex.h"

// Gaudi & AthenaCommon
#include "GaudiKernel/MsgStream.h"
#include <string>
#include <typeinfo>

 
// Constructor with parameters:
Trk::VertexOnTrack::VertexOnTrack( const LocalParameters& locpars,
                                   const Amg::MatrixX& locerr,
                                   const PerigeeSurface& assocSurf) :
  Trk::MeasurementBase(locpars,locerr),
  m_associatedSurface(assocSurf.clone()),
  m_globalPosition(0)
{
}


Trk::VertexOnTrack::VertexOnTrack( const LocalParameters& locpars,
                                   const Amg::MatrixX& locerr,
                                   Trk::SurfaceUniquePtrT<const PerigeeSurface> assocSurf) :
  Trk::MeasurementBase(locpars,locerr),
  m_associatedSurface(assocSurf.release()),
  m_globalPosition(0)
{
}


// Constructor with parameters:
Trk::VertexOnTrack::VertexOnTrack( const Trk::RecVertex& rvertex,
                                   const Trk::Perigee& perigee ) :
  Trk::MeasurementBase(),
  m_associatedSurface(perigee.associatedSurface().clone()),
  m_globalPosition(new Amg::Vector3D(perigee.position()))
{
  if ( (rvertex.position()-perigee.associatedSurface().center()).mag() > 10e-5 )
    throw GaudiException("Inconsistent input data, Perigee not expressed at vertex!","Trk::VertexOnTrack", StatusCode::RECOVERABLE);
    
  // the local parameters are by definition at (0,0)
  Amg::Vector2D localPosition(0,0);
  m_localParams = Trk::LocalParameters(localPosition);
  
  // transform Cartesian (x,y,z) to perigee (d0,z0)
  Amg::MatrixX jacobian(2,3);
  jacobian.setZero();
  double ptInv                               =  1./perigee.momentum().perp();
  jacobian(0,0)                              = -ptInv*perigee.momentum().y();
  jacobian(0,1)                              =  ptInv*perigee.momentum().x();
  jacobian(1,2)                              =  1.0;
  // the covariance on matrix of the vertex
  const Amg::MatrixX& vtxCov = rvertex.covariancePosition();
  // the local coordinate
  m_localCovariance = vtxCov.similarity(jacobian);
}

// Destructor:
Trk::VertexOnTrack::~VertexOnTrack()
{
  delete m_associatedSurface;
  delete m_globalPosition;
}

// default constructor:
Trk::VertexOnTrack::VertexOnTrack() :
  Trk::MeasurementBase(),
  m_associatedSurface(0),
  m_globalPosition(0)
{}

// copy constructor:
Trk::VertexOnTrack::VertexOnTrack( const Trk::VertexOnTrack& vot) :
  Trk::MeasurementBase(vot),
  m_associatedSurface( new Trk::PerigeeSurface(*vot.m_associatedSurface) ),
  m_globalPosition(0)
{}

// assignment operator:
Trk::VertexOnTrack& Trk::VertexOnTrack::operator=(const VertexOnTrack& vot)
{
  if ( &vot != this) {
    delete m_associatedSurface;
    delete m_globalPosition;
    Trk::MeasurementBase::operator=(vot);
    m_globalPosition = 0;
    m_associatedSurface  = new Trk::PerigeeSurface(*vot.m_associatedSurface);
  }
  return *this;
}


MsgStream& Trk::VertexOnTrack::dump( MsgStream& sl ) const
{
  std::string name(typeid(*this).name());
  sl << "Concrete dump method not implemented - using base class" << std::endl;
  sl << name << "\t  local position = "<< this->localParameters() << std::endl;
  sl << name << "\t  global position ( " << this->globalPosition().x() << " , "
                                         << this->globalPosition().y() << " , "
                                         << this->globalPosition().z() << " ) " << std::endl;
  sl << name << "\t  has Error Matrix: " << std::endl;
  sl << this->localCovariance() << std::endl;
  sl << name << "\t  has associated surface:" << std::endl;
  sl<< this->associatedSurface() << std::endl;
  return sl;
}

std::ostream& Trk::VertexOnTrack::dump( std::ostream& sl ) const
{
  std::string name(typeid(*this).name());
  sl << "Concrete dump method not implemented - using base class" << std::endl;
  sl << name << "\t  local position = "<< this->localParameters() << std::endl;
  sl << name << "\t  global position ( " << this->globalPosition().x() << " , "
                                         << this->globalPosition().y() << " , "
                                         << this->globalPosition().z() << " ) " << std::endl;
  sl << name << "\t  has Error Matrix: " << std::endl;
  sl << this->localCovariance() << std::endl;
  sl << name << "\t  has associated surface:" << std::endl;
  sl<< this->associatedSurface() << std::endl;
  return sl;
}




