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

// ALPINE CHARGED COMPATIBLE SURFACES
size_t Trk::AlpineLayer::compatibleSurfaces(std::vector<Trk::SurfaceIntersection>& cSurfaces,
				      const Trk::TrackParameters& pars,
				      Trk::PropDirection pDir,
				      const Trk::BoundaryCheck& bcheck,
  				      bool /* materialSurfacesOnly */,
				      const Trk::Surface* ,
				      const Trk::Surface*,
				      const Trk::ICompatibilityEstimator* ) const
{
  // Clear the vector, just in case
  cSurfaces.clear();

  // position and momentum/dir 
  const Amg::Vector3D& pos = pars.position();
  const Amg::Vector3D& mom = pars.momentum();
  const Amg::Vector3D  dir = (pDir == oppositeMomentum) ? Amg::Vector3D(-1.*mom.unit()) : mom.unit();

  // check if you need to force the momentum direction
  bool fDirection = ( pDir == anyDirection ? false : true );

  double maxPathLength = 10e10;

  // Needed track parameters
  double phi_inner_track = mom.phi();
  double z_inner_track = pos.z();

  double z_inner_layer = z_inner_track;
  double z_outer_layer = z_inner_track;

  // Get z inner and outer for layer
  if(m_approachDescriptor) {

    const Trk::ApproachSurfaces* aSurfaces = m_approachDescriptor->approachSurfaces(pos,dir);
    int NaSurf = int(aSurfaces->size());

    ///////////////////////////////////
    // Outer Surface: aSurfaces->at(0); 
    // Inner Surface: aSurfaces->at(1); 
    ///////////////////////////////////

    if(NaSurf>=1){
      const Surface *layerOuterSurface = aSurfaces->at(0); 
      Intersection layerOuterInter = layerOuterSurface->straightLineIntersection(pos,dir,fDirection,bcheck);
      z_outer_layer = layerOuterInter.position.z();

      //double r_outer_layer = layerOuterInter.position.perp();
      //double r_inner_layer = pos.perp();
      //std::cout << "DEBUG--- LAYER r_inner=" << r_inner_layer << " r_outer=" << r_outer_layer <<std::endl;
    }
  }

  // Get compatible surfaces 
  std::vector<const Trk::Surface*> testSurfacesLayer = getCompatibleSurfaces(phi_inner_track,z_inner_layer,z_outer_layer);

  // Create SurfaceIntersection objects 
  for(unsigned int is=0;is<testSurfacesLayer.size();is++) {
    const Trk::Surface * tSurface = testSurfacesLayer.at(is);
    if(tSurface==0) continue;
    Intersection tsfInter = tSurface->straightLineIntersection(pos,dir,fDirection,bcheck);
    // check if the intersection is valid and the maxPathLength has not been exceeded
    if (tsfInter.valid && tsfInter.pathLength < maxPathLength ){
      // resulting propDirection
      PropDirection rDir = fDirection ? pDir : ( tsfInter.pathLength > 0 ? alongMomentum : oppositeMomentum );
      // push back surfaces & direction 
      cSurfaces.push_back(SurfaceIntersection(tsfInter,tSurface,rDir));
    }
  }

  // now sort it 
  std::sort(cSurfaces.begin(),cSurfaces.end());
  // return vector size
  return cSurfaces.size();
}

// ALPINE NEUTRAL COMPATIBLE SURFACES
size_t Trk::AlpineLayer::compatibleSurfaces(std::vector<Trk::SurfaceIntersection>& cSurfaces,
				      const Trk::NeutralParameters& pars,
				      Trk::PropDirection pDir,
				      const Trk::BoundaryCheck& bcheck,
				      bool ,
				      const Trk::Surface* ,
				      const Trk::Surface* ,
				      const Trk::ICompatibilityEstimator* ) const
{
  // Clear the vector, just in case
  cSurfaces.clear();

  // position and momentum/dir 
  const Amg::Vector3D& pos = pars.position();
  const Amg::Vector3D& mom = pars.momentum();
  const Amg::Vector3D  dir = (pDir == oppositeMomentum) ? Amg::Vector3D(-1.*mom.unit()) : mom.unit();

  // check if you need to force the momentum direction
  bool fDirection = ( pDir == anyDirection ? false : true );

  double maxPathLength = 10e10;

  // Needed track parameters
  double phi_inner_track = mom.phi();
  double z_inner_track = pos.z();

  double z_inner_layer = z_inner_track;
  double z_outer_layer = z_inner_track;

  // Get z inner and outer for layer
  if(m_approachDescriptor) {

    const Trk::ApproachSurfaces* aSurfaces = m_approachDescriptor->approachSurfaces(pos,dir);
    int NaSurf = int(aSurfaces->size());

    ///////////////////////////////////
    // Outer Surface: aSurfaces->at(0); 
    // Inner Surface: aSurfaces->at(1); 
    ///////////////////////////////////

    if(NaSurf>=1){
      const Surface *layerOuterSurface = aSurfaces->at(0); 
      Intersection layerOuterInter = layerOuterSurface->straightLineIntersection(pos,dir,fDirection,bcheck);
      z_outer_layer = layerOuterInter.position.z();

      //double r_outer_layer = layerOuterInter.position.perp();
      //double r_inner_layer = pos.perp();
      //std::cout << "DEBUG--- LAYER r_inner=" << r_inner_layer << " r_outer=" << r_outer_layer <<std::endl;
    }
  }

  // Get compatible surfaces 
  std::vector<const Trk::Surface*> testSurfacesLayer = getCompatibleSurfaces(phi_inner_track,z_inner_layer,z_outer_layer);

  // Create SurfaceIntersection objects 
  for(unsigned int is=0;is<testSurfacesLayer.size();is++) {
    const Trk::Surface * tSurface = testSurfacesLayer.at(is);
    if(tSurface==0) continue;
    Intersection tsfInter = tSurface->straightLineIntersection(pos,dir,fDirection,bcheck);
    // check if the intersection is valid and the maxPathLength has not been exceeded
    if (tsfInter.valid && tsfInter.pathLength < maxPathLength ){
      // resulting propDirection
      PropDirection rDir = fDirection ? pDir : ( tsfInter.pathLength > 0 ? alongMomentum : oppositeMomentum );
      // push back surfaces & direction 
      cSurfaces.push_back(SurfaceIntersection(tsfInter,tSurface,rDir));
    }
  }

  // now sort it 
  std::sort(cSurfaces.begin(),cSurfaces.end());
  // return vector size
  return cSurfaces.size();
}

// GET COMPATIBLE SURFACES
std::vector<const Trk::Surface*> Trk::AlpineLayer::getCompatibleSurfaces(double phi_inner, double z_inner, double z_outer) const
{

  std::vector<const Trk::Surface*> results;

  // find closest staves
  std::vector<int> closest = getClosestStaves(phi_inner);

  for (unsigned int i=0;i<closest.size();i++){
    int istave = closest[i];
    AlpineStave *stave = m_staves[istave];
    stave->pushPlainModules(results,z_inner,z_outer);
    stave->pushMountainModules(results,z_inner,z_outer);
  }

  return results;
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
