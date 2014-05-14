/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EntryDiscLayerProvider.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkGeometry/EntryDiscLayerProvider.h"
#include "TrkGeometry/DiscLayer.h"
#include "TrkVolumes/CylinderIntersector.h"
#include "TrkSurfaces/DiscBounds.h"

Trk::EntryDiscLayerProvider::EntryDiscLayerProvider(Trk::DiscLayer* layerAtZmin,
                                                    Trk::DiscLayer* layerAtZmax,
                                                    double iTolerance) :
 Trk::EntryLayerProvider(),
 m_rMin(10e10),
 m_rMax(0.),
 m_zMin(10e10),
 m_zMax(-10e10),
 m_tolerance(iTolerance),
 m_layerAtZmin(layerAtZmin),
 m_layerAtZmax(layerAtZmax)
{

  // zMin / zMax
  m_zMin = m_layerAtZmin ? m_layerAtZmin->surfaceRepresentation().center().z() : 0.;
  m_zMax = m_layerAtZmax ? m_layerAtZmax->surfaceRepresentation().center().z() : 0.;
  // protection
  if (m_zMin > m_zMax){
     // swap
     m_layerAtZmax = m_layerAtZmin;
     m_layerAtZmin = layerAtZmax;
     m_zMin = m_zMax;
     // redo the exercise with swapped values
     m_zMax = m_layerAtZmax ? m_layerAtZmax->surfaceRepresentation().center().z() : 0.;
  }

  // the layers for the z extends
  std::vector<const Trk::DiscLayer*> discLayers;
	discLayers.reserve(2);
    if (m_layerAtZmin){
       discLayers.push_back(m_layerAtZmin);
       m_cachedBaseClassLayers.push_back(m_layerAtZmin);
   } 
    if (m_layerAtZmax){
       discLayers.push_back(m_layerAtZmax);
       m_cachedBaseClassLayers.push_back(m_layerAtZmax);
   }

  // get the z extend through parsing
  std::vector<const Trk::DiscLayer*>::const_iterator discIter    =  discLayers.begin();
  std::vector<const Trk::DiscLayer*>::const_iterator discIterEnd =  discLayers.end();
  for ( ; discIter != discIterEnd; ++discIter){
    // get rMin/rMax
    const Trk::DiscBounds* discBo = dynamic_cast<const Trk::DiscBounds*>(&((*discIter)->surfaceRepresentation().bounds()));
    if (discBo){
      double layerRmin = discBo->rMin();
      double layerRmax = discBo->rMax();
      // set the min/max values
      m_rMin = layerRmin < m_rMin ? layerRmin : m_rMin;
      m_rMax = layerRmax > m_rMax ? layerRmax : m_rMax;
     }
   }

}       


Trk::EntryDiscLayerProvider::~EntryDiscLayerProvider()
{
   delete m_layerAtZmin;
   delete m_layerAtZmax;
}

const Trk::DiscLayer* Trk::EntryDiscLayerProvider::associatedLayer(const Amg::Vector3D& gp) const
{
   double posZ = gp.z();
   //  in 1D: closest distance
   return fabs(posZ-m_zMin) < fabs(posZ-m_zMax) ? m_layerAtZmin : m_layerAtZmax;

}

const Trk::DiscLayer* Trk::EntryDiscLayerProvider::entryLayer(const Amg::Vector3D& gp,
							      const Amg::Vector3D& dir) const
{
  // only give entry layers when arriving through an entry surface
  double posZ = gp.z();
  double deltaZ = dir.z();
  if (posZ < m_zMin && deltaZ > 0.) return m_layerAtZmin;
  if (posZ > m_zMax && deltaZ < 0.) return m_layerAtZmax;
  return 0;
}

const Trk::DiscLayer* Trk::EntryDiscLayerProvider::exitLayer(const Amg::Vector3D& gp,
                                                             const Amg::Vector3D& dir) const
{

  // the easy way: the vetos 
  // (a) in z
  double posZ = gp.z();
  //if (posZ < m_zMin || posZ > m_zMax ) return 0;
  // (b) in r
  double posR = gp.perp();
  //if (posR < m_rMin || posR > m_rMax ) return 0;
  // the relatiions of the global direction
  double deltaZ = dir.z();
  Amg::Vector3D postGp(gp+dir);
  double deltaR = postGp.perp() - posR;
  // numerical stability 
  if (deltaZ*deltaZ < 10e-10 || deltaR*deltaR < 10e-10) return 0;
  // now this is save
  double slopeRZ = deltaR/deltaZ;
  // decision on intersection cylinder
  const Trk::DiscLayer* exitCandidate =  m_layerAtZmin;
  double layerZpos = m_zMin;
  if (deltaZ > 0.) {      
      exitCandidate = m_layerAtZmax;
      layerZpos = m_zMax;
  }  

  // (a) in the the outside (deltaR > 0) case - check if there's a chance
  if (deltaR > 0. && posR + slopeRZ * (layerZpos - posZ) < m_rMax + m_tolerance) 
      return exitCandidate;   

  // (b) in the inside check: punch through middle cylinder vetoes 
  double slopeZR = deltaZ/deltaR;
  // intersect disctance to inner cylinder (squared)
  double deltaInnerRR = posR - m_rMin;
  double deltaInnerRZ = deltaInnerRR * slopeZR;
  // intersect distance to disc
  double deltaDiscZ = layerZpos - posZ;
  double deltaDiscR = deltaDiscZ * slopeRZ;
  // return 0 if the inner cylinder is hit before the disc
  if (deltaInnerRR*deltaInnerRR + deltaInnerRZ*deltaInnerRZ 
      < deltaDiscR*deltaDiscR + deltaDiscZ*deltaDiscZ)
      return 0;
   // check if the candidate holds
  return (posR + deltaDiscR > m_rMin - m_tolerance) ? exitCandidate : 0;

}
