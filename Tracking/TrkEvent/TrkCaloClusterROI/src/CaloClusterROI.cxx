/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// CaloClusterROI.cxx

// Trk
#include <memory>



#include "TrkCaloClusterROI/CaloClusterROI.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkSurfaces/Surface.h"

// Constructor with parameters:
Trk::CaloClusterROI::CaloClusterROI(const Trk::LocalParameters* locpars,
                                    const Trk::Surface& surface,
                                    double energy,
                                    double widthphi,
                                    double widtheta)
  : SurfacePtrHolder(surface)
  , m_localParams(locpars)
  , m_energy(energy)
  , m_phiWidth(widthphi)
  , m_etaWidth(widtheta)
{}

// Destructor:
Trk::CaloClusterROI::~CaloClusterROI() = default;

// default constructor:
Trk::CaloClusterROI::CaloClusterROI()
  : SurfacePtrHolder()
  , m_localParams(nullptr)
  , m_energy(0.0)
  , m_phiWidth(0.0)
  , m_etaWidth(0.0)
{}

// copy constructor:
Trk::CaloClusterROI::CaloClusterROI(const Trk::CaloClusterROI& cot)
  : SurfacePtrHolder(cot)
  , m_localParams(std::make_unique<Trk::LocalParameters>(*cot.m_localParams))
  , m_energy(cot.m_energy)
  , m_phiWidth(cot.m_phiWidth)
  , m_etaWidth(cot.m_etaWidth)
{}

Trk::CaloClusterROI*
Trk::CaloClusterROI::clone() const
{
  return new Trk::CaloClusterROI(*this);
}

// assignment operator:
Trk::CaloClusterROI&
Trk::CaloClusterROI::operator=(const Trk::CaloClusterROI& cot)
{
  if (&cot != this) {
    SurfacePtrHolder::operator=(cot);
    m_localParams = std::make_unique<Trk::LocalParameters>(*cot.m_localParams);
    m_energy = cot.m_energy;
    m_phiWidth = cot.m_phiWidth;
    m_etaWidth = cot.m_etaWidth;
  }
  return *this;
}

Amg::Vector3D
Trk::CaloClusterROI::globalPosition() const
{
  if (m_localParams && m_associatedSurface) {
    return m_associatedSurface->localToGlobal(*m_localParams);
  }
  if (m_associatedSurface) {
    return m_associatedSurface->center();
  }
  return {};
}

