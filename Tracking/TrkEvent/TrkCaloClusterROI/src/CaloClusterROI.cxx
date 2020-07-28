/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//CaloClusterROI.cxx


//Trk
#include "TrkCaloClusterROI/CaloClusterROI.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkSurfaces/Surface.h"


// Constructor with parameters:
Trk::CaloClusterROI::CaloClusterROI(  const Trk::LocalParameters* locpars,
                                      const Trk::Surface& surface,
                                      double energy,
                                      double widthphi,
                                      double widtheta) 
  :
  m_localParams(locpars),
  m_energy( energy ),
  m_phiWidth( widthphi ),
  m_etaWidth( widtheta )
{
  m_surface = surface.isFree() ? surface.clone() : &surface;
}

// Destructor:
Trk::CaloClusterROI::~CaloClusterROI()
{
	delete m_localParams;
  if (m_surface->isFree()) {delete m_surface;} // Don't delete surfaces belonging to DEs!
}

// default constructor:
Trk::CaloClusterROI::CaloClusterROI() :
    m_localParams(nullptr),
    m_surface(nullptr),
    m_energy(0.0),
    m_phiWidth(0.0),
    m_etaWidth(0.0)
{
}  

// copy constructor:
Trk::CaloClusterROI::CaloClusterROI( const Trk::CaloClusterROI& cot) :
  m_localParams( new Trk::LocalParameters(*cot.m_localParams) ),
  m_surface( cot.m_surface? (cot.m_surface->isFree() ? cot.m_surface->clone():cot.m_surface) : nullptr),
  m_energy(cot.m_energy),
  m_phiWidth(cot.m_phiWidth),
  m_etaWidth(cot.m_etaWidth)
{
}

Trk::CaloClusterROI* Trk::CaloClusterROI::clone() const  
{
  return  new Trk::CaloClusterROI ( *this );
}


// assignment operator:
Trk::CaloClusterROI& Trk::CaloClusterROI::operator=(const Trk::CaloClusterROI& cot)
{
  if ( &cot != this) {
    delete m_localParams;
    delete m_surface;
    m_localParams     = new Trk::LocalParameters(*cot.m_localParams);
    m_surface =  (cot.m_surface? (cot.m_surface->isFree() ? cot.m_surface->clone():cot.m_surface) : nullptr);
    m_energy=cot.m_energy;
    m_phiWidth=cot.m_phiWidth;
    m_etaWidth=cot.m_etaWidth;
  }
  return *this;
}

Amg::Vector3D Trk::CaloClusterROI::globalPosition() const
{
    if(m_localParams){
      return m_surface->localToGlobalPos(*m_localParams);
    }
    if(m_surface){
      return m_surface->center();
    }
    return Amg::Vector3D();
}


