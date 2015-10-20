/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PseudoMeasurementOnTrack.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
//#include "TrkEventPrimitives/LocalParameters.h"
//#include "TrkEventPrimitives/ErrorMatrix.h"
#include "TrkSurfaces/Surface.h"
#include "GaudiKernel/MsgStream.h"
#include <string>
#include <typeinfo>



Trk::PseudoMeasurementOnTrack::PseudoMeasurementOnTrack(const LocalParameters& locpars,
                                                        const Amg::MatrixX& locerr,
                                                        const Surface&     assocSurf) :
  Trk::MeasurementBase(locpars,locerr),
  m_globalPosition(0)
{
    // Copy if belongs to DE, clone() otherwise...
    m_associatedSurface = assocSurf.isFree() ? assocSurf.clone() : &assocSurf;
}

Trk::PseudoMeasurementOnTrack::PseudoMeasurementOnTrack(const LocalParameters& locpars,
                                                        const Amg::MatrixX& locerr,
                                                        Trk::ConstSurfaceUniquePtr  assocSurf) :
  Trk::MeasurementBase(locpars,locerr),
  m_associatedSurface (assocSurf.release()),
  m_globalPosition(0)
{
}

// Destructor:
Trk::PseudoMeasurementOnTrack::~PseudoMeasurementOnTrack()
{
  if (m_associatedSurface && m_associatedSurface->isFree())
    delete m_associatedSurface; // Don't delete surfaces belonging to DEs!
  delete m_globalPosition;
}

// default constructor:
Trk::PseudoMeasurementOnTrack::PseudoMeasurementOnTrack() :
  Trk::MeasurementBase(),
  m_associatedSurface(0),
  m_globalPosition(0)
{}

// copy constructor:
Trk::PseudoMeasurementOnTrack::PseudoMeasurementOnTrack( const Trk::PseudoMeasurementOnTrack& pmot) :
  Trk::MeasurementBase(pmot),
  m_associatedSurface( pmot.m_associatedSurface? (pmot.m_associatedSurface->isFree() ? pmot.m_associatedSurface->clone():pmot.m_associatedSurface) : 0),
  m_globalPosition(0)
{}

// assignment operator:
Trk::PseudoMeasurementOnTrack& Trk::PseudoMeasurementOnTrack::operator=(const PseudoMeasurementOnTrack& pmot)
{
  if ( &pmot != this) {
    if (m_associatedSurface && m_associatedSurface->isFree())
      delete m_associatedSurface;
    delete m_globalPosition;
    m_globalPosition = 0;
    Trk::MeasurementBase::operator=(pmot);
    m_associatedSurface  = 
      ( pmot.m_associatedSurface? (pmot.m_associatedSurface->isFree() ? pmot.m_associatedSurface->clone():pmot.m_associatedSurface) : 0);
  }
  return *this;
}

const Amg::Vector3D& Trk::PseudoMeasurementOnTrack::globalPosition() const
{
  if (m_globalPosition == 0) {m_globalPosition = new Amg::Vector3D(m_associatedSurface->center());}
  return *m_globalPosition;
}

MsgStream& Trk::PseudoMeasurementOnTrack::dump( MsgStream& sl ) const
{
  std::string name(typeid(*this).name());
  sl << "Concrete dump method not implemented - using base class" << std::endl;
  sl << name << "\t  local position = "<< this->localParameters() << std::endl;
  if (m_globalPosition)
    sl << name << "\t  global position ( " << this->globalPosition().x() << " , "
       << this->globalPosition().y() << " , " << this->globalPosition().z()
       << " ) " << std::endl;
  sl << name << "\t  has error matrix: " << std::endl;
  sl << this->localCovariance() << std::endl;
  sl << name << "\t  has associated surface:" << std::endl;
  sl << this->associatedSurface() << std::endl;
  return sl;
}

std::ostream& Trk::PseudoMeasurementOnTrack::dump( std::ostream& sl ) const
{
  std::string name(typeid(*this).name());
  sl << "Concrete dump method not implemented - using base class" << std::endl;
  sl << name << "\t  local position = "<< this->localParameters() << std::endl;
  if (m_globalPosition)
    sl << name << "\t  global position ( " << this->globalPosition().x() << " , "
       << this->globalPosition().y() << " , " << this->globalPosition().z()<<" ) "
       << std::endl;
  sl << name << "\t  has error matrix: " << std::endl;
  sl << this->localCovariance() << std::endl;
  sl << name << "\t  has associated surface:" << std::endl;
  sl << this->associatedSurface() << std::endl;
  return sl;
}




