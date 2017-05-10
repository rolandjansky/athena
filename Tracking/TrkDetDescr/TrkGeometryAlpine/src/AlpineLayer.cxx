/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TMath.h>
#include "TrkGeometryAlpine/AlpineStave.h"
#include "TrkGeometryAlpine/AlpineLayer.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkGeometryAlpine/AlpineApproachDescriptor.h"

Trk::AlpineLayer::AlpineLayer(CylinderBounds* cbounds,
			      const LayerMaterialProperties& laymatprop,
			      double thickness, OverlapDescriptor* od,
			      IApproachDescriptor* ad,
			      int laytyp) : CylinderLayer(cbounds,laymatprop,thickness,od,laytyp)
{ 
  m_approachDescriptor = ad;
  m_surfaceArray = 0; 
  if (m_approachDescriptor) m_approachDescriptor->registerLayer(*this);
}

Trk::AlpineLayer::~AlpineLayer()
{
}

// SURFACE REPRESENTATION
const Trk::CylinderSurface& Trk::AlpineLayer::surfaceRepresentation() const
{
  return (*this);
}

// SURFACE ON APPROACH
const Trk::Surface& Trk::AlpineLayer::surfaceOnApproach(const Amg::Vector3D& pos,
							const Amg::Vector3D& dir,
							Trk::PropDirection,
							const Trk::BoundaryCheck&,
							bool,
							const Trk::ICompatibilityEstimator*) const
{ 
  if(m_approachDescriptor) {
    
    const Trk::ApproachSurfaces* aSurfaces = m_approachDescriptor->approachSurfaces(pos,dir);
    const CylinderSurface *approachSurface;

    const int naSurf = aSurfaces->size();
    //std::cout << "----Found " << naSurf << " approach surfaces" << std::endl; 

    ///// FIXME - return different surfaces for forward/backward tracking ////
    // standard tracking - approach surface is inner: aSurfaces->at(naSurf-1); ////
    // backwards tracking - approach surface is outer: aSurfaces->at(0); ////

    approachSurface = (CylinderSurface*) aSurfaces->at(naSurf-1); 
    
    return *approachSurface;
  }

  // if no approach descriptor found, use default
  std::cout << "WARNING: No approach descriptor found - use surface representation instead" << std::endl;
  return surfaceRepresentation(); 
}

// UTILITIES
std::vector<const Trk::Surface*>& Trk::AlpineLayer::getSurfaces()
{
  // Are surfaces already filled in?
  if(m_surfaces.size()>0) return m_surfaces;

  // No, do it then.
  int nstaves = m_staves.size();

  for(int istave=0;istave<nstaves;istave++) {
    Trk::AlpineStave* stave = (Trk::AlpineStave*) m_staves.at(istave);   
    std::vector<const Trk::Surface*> staveSurfaces = stave->getSurfaces();
    if(staveSurfaces.size() == 0) continue;
    m_surfaces.insert(m_surfaces.end(), staveSurfaces.begin(), staveSurfaces.end());
  }
  
  return m_surfaces;
}

void Trk::AlpineLayer::dump2D(std::ofstream &fout)
{
  fout << "  // ------------------------------ Layer ---------------------------------------// " << std::endl;

  // Since it is 2D only draw only one stave (-PI)
  int nstaves = m_staves.size();
  if(nstaves<=0) return;
  m_staves[0]->dump2D(fout);
}

bool Trk::AlpineLayer::hasSubStructure(bool) const 
{
  if(m_staves.size()==0) return false;
  return true;
}

// find closest stave
std::vector<int >Trk::AlpineLayer::getClosestStaves(double phi_inner) const	{

  int nstaves = m_staves.size();
  std::vector<int> result;

  // Stave 0 starts at -PI 
  if (phi_inner > TMath::Pi() ) phi_inner -= TMath::TwoPi();

  double iphi = nstaves * (phi_inner/TMath::TwoPi()+0.5);

  int closest = int(iphi);
  //int closest = iphi+0.5;
  if (closest >= nstaves) closest=0;

  int next = closest+1;
  if (next >= nstaves) next=0;
  
  int previous = closest-1;
  if (previous < 0) previous=nstaves-1;

  result.push_back(previous);
  result.push_back(closest);
  result.push_back(next);

  return result;
}
