/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */


//Trk
#include "TrkCaloCluster_OnTrack/CaloCluster_OnTrack.h"
#include "TrkSurfaces/Surface.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"

// Gaudi & AthenaCommon
#include "GaudiKernel/MsgStream.h"
#include <string>
#include <limits>
#include <ostream>

namespace{
const double NaN(std::numeric_limits<double>::quiet_NaN());
alignas(16) const Amg::Vector3D INVALID_VECTOR3D(NaN, NaN, NaN);
}

Trk::CaloCluster_OnTrack::CaloCluster_OnTrack(  const Trk::LocalParameters& locpars,
                                                const Amg::MatrixX& locerr,
                                                const Trk::Surface& surface,
                                                const Trk::EnergyLoss* eloss) 
: MeasurementBase(locpars,locerr)
{
  m_eloss = eloss;
  m_surface = surface.isFree() ? surface.clone() : &surface;
  m_globalpos =
    std::make_unique<const Amg::Vector3D>(surface.localToGlobal(m_localParams));
}

// Destructor:
Trk::CaloCluster_OnTrack::~CaloCluster_OnTrack()
{
 // Don't delete surfaces belonging to DEs!
  if (m_surface && m_surface->isFree()) {delete m_surface;}  
  if (m_eloss) {delete m_eloss;}
}

// default constructor:
Trk::CaloCluster_OnTrack::CaloCluster_OnTrack() :
  m_surface{nullptr},
  m_globalpos{nullptr},
  m_eloss{nullptr}
{
}  

// copy constructor:
Trk::CaloCluster_OnTrack::CaloCluster_OnTrack( const Trk::CaloCluster_OnTrack& cot) :
  Trk::MeasurementBase(cot)
{
  m_surface= cot.m_surface? (cot.m_surface->isFree() ? cot.m_surface->clone():cot.m_surface) : nullptr;
  m_globalpos = cot.m_globalpos ? std::make_unique<Amg::Vector3D>(*cot.m_globalpos) : nullptr;
  m_eloss= cot.m_eloss ? new Trk::EnergyLoss(*cot.m_eloss) : nullptr ;
}

Trk::CaloCluster_OnTrack* Trk::CaloCluster_OnTrack::clone() const  
{
  return  new Trk::CaloCluster_OnTrack ( *this );
}



// assignment operator:
Trk::CaloCluster_OnTrack& Trk::CaloCluster_OnTrack::operator=(const Trk::CaloCluster_OnTrack& cot)
{
  if ( &cot != this) {
    if (m_surface && m_surface->isFree()) {delete m_surface;}  
    if (m_eloss) {     delete m_eloss;}
    Trk::MeasurementBase::operator=(cot);
    m_surface =  cot.m_surface? (cot.m_surface->isFree() ? cot.m_surface->clone():cot.m_surface) : nullptr;
    m_globalpos = cot.m_globalpos ? std::make_unique<Amg::Vector3D>(*cot.m_globalpos) : nullptr;
    m_eloss= cot.m_eloss ? new Trk::EnergyLoss(*cot.m_eloss) : nullptr ;
  }
  return *this;
}

const Amg::Vector3D& Trk::CaloCluster_OnTrack::globalPosition() const
{
  if (m_globalpos) { return *m_globalpos;}
  return INVALID_VECTOR3D;
}

MsgStream& Trk::CaloCluster_OnTrack::dump( MsgStream& sl ) const
{
  sl << "Trk::CaloCluster_OnTrack { "<<"\n";
  sl << "\t  surface = "<< associatedSurface()<<"\n";
  sl << "\t  position = (" 
    << localParameters() 
    << endmsg;
  sl << "\t  has Error Matrix: "<<"\n";
  sl<<localCovariance()<<"}"<<"\n"; 

  return sl;
}

std::ostream& Trk::CaloCluster_OnTrack::dump( std::ostream& sl ) const
{
  sl << "\t  surface = "<< associatedSurface()<<"\n";
  sl << "\t  position = (" 
    << localParameters() 
    << "\n";
  sl << "\t  has Error Matrix: "<<"\n";
  sl<<localCovariance()<<"}"<<"\n"; 
  return sl;
}


