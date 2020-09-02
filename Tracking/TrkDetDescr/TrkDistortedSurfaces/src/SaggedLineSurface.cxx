/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SaggedLineSurface.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkDistortedSurfaces/SaggedLineSurface.h"
#include "TrkSurfaces/CylinderBounds.h"

// From Dan Levin: MDT 
// linear density of wire: lambda=wireLinearDensity=19.3 [gm/cm^3] * PI*
//(25 *10^-4 )^2 [cm^2] = 378.954 microgram/cm
// From Dan Levin: MDT
// wireTen=350 for most chambers,  285 gm for some NIKHEF chambers (BOL),

Trk::SaggedLineSurface::SaggedLineSurface() :
  Trk::DistortedSurface(),
  Trk::StraightLineSurface(),
  m_saggingDescriptor(nullptr),
  m_lineDirection(nullptr)
{}

Trk::SaggedLineSurface::SaggedLineSurface(Amg::Transform3D* htrans,
                                          double radius, 
                                          double halez, 
                                          Trk::LineSaggingDescriptor* lsd) :
  Trk::DistortedSurface(),
  Trk::StraightLineSurface(htrans, radius, halez),
  m_saggingDescriptor(lsd),
  m_lineDirection(nullptr)
{}

Trk::SaggedLineSurface::SaggedLineSurface(std::unique_ptr<Amg::Transform3D> htrans) :
  Trk::DistortedSurface(),
  Trk::StraightLineSurface(std::move(htrans)),
  m_saggingDescriptor(nullptr),
  m_lineDirection(nullptr)
{}

Trk::SaggedLineSurface::SaggedLineSurface(const Trk::TrkDetElementBase& detelement, const Identifier& id) :
  Trk::DistortedSurface(),
  Trk::StraightLineSurface(detelement,id),
  m_saggingDescriptor(),
  m_lineDirection(nullptr)
{}

Trk::SaggedLineSurface::SaggedLineSurface(const Trk::TrkDetElementBase& detelement, const Identifier& id,
                                          double wireLength, double wireTension, double linearDensity) :
  Trk::DistortedSurface(),
  Trk::StraightLineSurface(detelement,id),
  m_saggingDescriptor(),
  m_lineDirection(nullptr)
{
     m_saggingDescriptor = new Trk::LineSaggingDescriptor(wireLength, wireTension, linearDensity);
}

Trk::SaggedLineSurface::SaggedLineSurface(const Trk::SaggedLineSurface& sls) :
  Trk::DistortedSurface(sls),
  Trk::StraightLineSurface(sls),
  m_saggingDescriptor(new Trk::LineSaggingDescriptor(sls.distortionDescriptor())),
  m_lineDirection(nullptr)
{}

Trk::SaggedLineSurface::~SaggedLineSurface()
{
  delete m_saggingDescriptor;
}

Trk::SaggedLineSurface& Trk::SaggedLineSurface::operator=(const Trk::SaggedLineSurface& sls) 
{
  if (this != &sls ) {
    Trk::DistortedSurface::operator=(sls);
    Trk::StraightLineSurface::operator=(sls);
    delete m_saggingDescriptor;
    m_lineDirection.store(nullptr);
    m_saggingDescriptor=new Trk::LineSaggingDescriptor(sls.distortionDescriptor());
  }
  return *this;
}

Trk::StraightLineSurface* Trk::SaggedLineSurface::correctedSurface(const Amg::Vector2D& lp) const
{
  // prepare
  Amg::Transform3D* newHepTransform =nullptr;
  if (!m_lineDirection) {
    m_lineDirection.set(std::make_unique<Amg::Vector3D>(transform().rotation().col(3)));
  }
  if (m_saggingDescriptor){
    // first get the hep transform from the distortion descriptor
    newHepTransform = m_saggingDescriptor->correctedSurfaceTransform(lp,
                                                                     center(),
                                                                     *m_lineDirection);
  } else if (Trk::Surface::associatedDetectorElement()) {
     // get the sagging descriptor and the endpoints from GeoModel
     // m_saggingDescriptor
    throw std::logic_error("Condition not implemented ( Trk::SaggedLineSurface::correctedSurface (1) ).");
  }else{
    throw std::logic_error("Condition not implemented ( Trk::SaggedLineSurface::correctedSurface (2) ).");
  }
  return (newHepTransform) ? new Trk::StraightLineSurface(newHepTransform,bounds().r(),10e3) : nullptr;
}


