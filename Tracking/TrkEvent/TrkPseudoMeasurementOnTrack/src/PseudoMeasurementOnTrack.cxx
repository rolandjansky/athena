/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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


namespace {
  // helper to move pointer and set source to nullptr
  template <class T>
  T move_ptr(T &src) {
    T tmp = src;
    src = nullptr;
    return tmp;
  }

const double NaN(std::numeric_limits<double>::quiet_NaN());
alignas(16) const Amg::Vector3D INVALID_VECTOR3D(NaN, NaN, NaN);
}


Trk::PseudoMeasurementOnTrack::PseudoMeasurementOnTrack(const LocalParameters& locpars,
                                                        const Amg::MatrixX& locerr,
                                                        const Surface&     assocSurf) :
  Trk::MeasurementBase(locpars,locerr),
  m_globalPosition(nullptr)
{
    // Copy if belongs to DE, clone() otherwise...
    m_associatedSurface = assocSurf.isFree() ? assocSurf.clone() : &assocSurf;
    m_globalPosition = new Amg::Vector3D(m_associatedSurface->center());
}

Trk::PseudoMeasurementOnTrack::PseudoMeasurementOnTrack(const LocalParameters& locpars,
                                                        const Amg::MatrixX& locerr,
                                                        Trk::ConstSurfaceUniquePtr  assocSurf) :
  Trk::MeasurementBase(locpars,locerr),
  m_associatedSurface (assocSurf.release()),
  m_globalPosition(nullptr)
{
  m_globalPosition = new Amg::Vector3D(m_associatedSurface->center());
}

// Destructor:
Trk::PseudoMeasurementOnTrack::~PseudoMeasurementOnTrack()
{
  if (m_associatedSurface && m_associatedSurface->isFree()){
    delete m_associatedSurface; // Don't delete surfaces belonging to DEs!
  }
  delete m_globalPosition;
}

// default constructor:
Trk::PseudoMeasurementOnTrack::PseudoMeasurementOnTrack() :
  Trk::MeasurementBase(),
  m_associatedSurface(nullptr),
  m_globalPosition(nullptr)
{}

// copy constructor:
Trk::PseudoMeasurementOnTrack::PseudoMeasurementOnTrack( const Trk::PseudoMeasurementOnTrack& pmot) :
  Trk::MeasurementBase(pmot),
  m_associatedSurface( pmot.m_associatedSurface? (pmot.m_associatedSurface->isFree() ? pmot.m_associatedSurface->clone():pmot.m_associatedSurface) : nullptr),
  m_globalPosition(nullptr)
{
  if(pmot.m_globalPosition){
    m_globalPosition = new Amg::Vector3D(*(pmot.m_globalPosition));
  }
}

// move constructor:
Trk::PseudoMeasurementOnTrack::PseudoMeasurementOnTrack(Trk::PseudoMeasurementOnTrack&& pmot) :
  Trk::MeasurementBase(pmot),
  m_associatedSurface(move_ptr(pmot.m_associatedSurface)),
  m_globalPosition(move_ptr(pmot.m_globalPosition))
{}


// assignment operator:
Trk::PseudoMeasurementOnTrack& Trk::PseudoMeasurementOnTrack::operator=(const PseudoMeasurementOnTrack& pmot)
{
  if ( &pmot != this) {
    if (m_associatedSurface && m_associatedSurface->isFree()){
      delete m_associatedSurface;
    }
    delete m_globalPosition;
    
    Trk::MeasurementBase::operator=(pmot);
    
    m_associatedSurface  = 
      ( pmot.m_associatedSurface? (pmot.m_associatedSurface->isFree() ? pmot.m_associatedSurface->clone():pmot.m_associatedSurface) : nullptr);

    m_globalPosition=nullptr;  
    if(pmot.m_globalPosition){ 
      m_globalPosition = new Amg::Vector3D(*(pmot.m_globalPosition));
    }
  
  }
  return *this;
}

// move assignment operator:
Trk::PseudoMeasurementOnTrack& Trk::PseudoMeasurementOnTrack::operator=(PseudoMeasurementOnTrack&& pmot)
{
  if ( &pmot != this) {
    Trk::MeasurementBase::operator=(pmot);
    if (m_associatedSurface && m_associatedSurface->isFree()){
      delete m_associatedSurface;
    }
    m_associatedSurface = move_ptr(pmot.m_associatedSurface);
    delete m_globalPosition;
    m_globalPosition    = move_ptr(pmot.m_globalPosition);
  }
  return *this;
}

const Amg::Vector3D& Trk::PseudoMeasurementOnTrack::globalPosition() const
{
  if (m_globalPosition) { return *m_globalPosition;}
  return INVALID_VECTOR3D;
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




