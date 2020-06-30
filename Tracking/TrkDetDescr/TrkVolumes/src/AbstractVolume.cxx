/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AbstractVolume.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

//Trk
#include "TrkVolumes/AbstractVolume.h"
#include "TrkVolumes/VolumeBounds.h"
#include "TrkVolumes/BoundarySurface.h"
#include "TrkVolumes/BoundaryCylinderSurface.h"
#include "TrkVolumes/BoundaryDiscSurface.h"
#include "TrkVolumes/BoundaryPlaneSurface.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
//Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"
//STD
#include <iostream>
//Eigen accessor
#include "GeoPrimitives/GeoPrimitives.h"


// Default constructor
Trk::AbstractVolume::AbstractVolume() :
  Volume(),
  m_boundarySurfaces(nullptr)
{}

// constructor with Amg::Transform3D
Trk::AbstractVolume::AbstractVolume(Amg::Transform3D* htrans, Trk::VolumeBounds* volbounds) :
  Volume(htrans, volbounds),
  m_boundarySurfaces(nullptr)
{
 createBoundarySurfaces();
}

// copy constructor - will up to now not copy the sub structure!
Trk::AbstractVolume::AbstractVolume(const Trk::AbstractVolume& vol) :
  Volume(vol),
  m_boundarySurfaces(nullptr)
{}

// destructor
Trk::AbstractVolume::~AbstractVolume()
{
  delete m_boundarySurfaces;
}

// assignment operator
Trk::AbstractVolume& Trk::AbstractVolume::operator=(const Trk::AbstractVolume& vol)
{
  if (this!=&vol)
	{
    Volume::operator=(vol);
    delete m_boundarySurfaces;
    m_boundarySurfaces = new std::vector< Trk::SharedObject<const Trk::BoundarySurface<Trk::AbstractVolume> > >(*vol.m_boundarySurfaces);
  }
	return *this;
}
    
const std::vector< Trk::SharedObject<const Trk::BoundarySurface<Trk::AbstractVolume> > >&  Trk::AbstractVolume::boundarySurfaces() const
{ return (*m_boundarySurfaces); }


void Trk::AbstractVolume::createBoundarySurfaces ATLAS_NOT_THREAD_SAFE ()
{
  // prepare the BoundarySurfaces
  m_boundarySurfaces = new std::vector< Trk::SharedObject<const Trk::BoundarySurface<Trk::AbstractVolume> > >;
  // transform Surfaces To BoundarySurfaces
  const std::vector<const Trk::Surface*>* surfaces = Trk::Volume::volumeBounds().decomposeToSurfaces(this->transform());
  std::vector<const Trk::Surface*>::const_iterator surfIter = surfaces->begin();
  
  // counter to flip the inner/outer position for Cylinders
  int sfCounter = 0;
  int sfNumber  = surfaces->size();
  
  for ( ; surfIter != surfaces->end(); ++surfIter){
      sfCounter++;
      const Trk::PlaneSurface*      psf = dynamic_cast<const Trk::PlaneSurface*>(*surfIter);
      if (psf){ m_boundarySurfaces->push_back(Trk::SharedObject<const Trk::BoundarySurface<Trk::AbstractVolume> >
                  (new Trk::BoundaryPlaneSurface<Trk::AbstractVolume>(this, nullptr, *psf)));    
        delete psf; continue;
      }
      const Trk::DiscSurface*       dsf = dynamic_cast<const Trk::DiscSurface*>(*surfIter);
      if (dsf){ m_boundarySurfaces->push_back(Trk::SharedObject<const Trk::BoundarySurface<Trk::AbstractVolume> >
                  (new Trk::BoundaryDiscSurface<Trk::AbstractVolume>(this, nullptr, *dsf)));
        delete dsf; continue;
      }
      const Trk::CylinderSurface*   csf = dynamic_cast<const Trk::CylinderSurface*>(*surfIter);
      if (csf){
        Trk::AbstractVolume* inner = (sfCounter == 3 && sfNumber > 3) ? nullptr : this;
        Trk::AbstractVolume* outer = (inner) ? nullptr : this;
        m_boundarySurfaces->push_back(Trk::SharedObject<const Trk::BoundarySurface< Trk::AbstractVolume> >
            (new Trk::BoundaryCylinderSurface<Trk::AbstractVolume>(inner, outer, *csf)));
        delete csf; continue;
      }
   }
  
   delete surfaces;
}

