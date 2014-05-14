/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EntryCylinderLayerProvider.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkGeometry/EntryCylinderLayerProvider.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkVolumes/CylinderIntersector.h"

Trk::EntryCylinderLayerProvider::EntryCylinderLayerProvider(Trk::CylinderLayer* layerAtInnerR,
                                                            Trk::CylinderLayer* layerAtOuterR,
                                                            double iTolerance) :
 Trk::EntryLayerProvider(),
 m_rMin(0.),
 m_rMax(0.),
 m_zMin(10e10),
 m_zMax(-10e10),
 m_tolerance(iTolerance),
 m_layerAtInnerRadius(layerAtInnerR),
 m_layerAtOuterRadius(layerAtOuterR)
{

  // rMin / rMax
  m_rMin = m_layerAtInnerRadius ? m_layerAtInnerRadius->surfaceRepresentation().bounds().r() : 0.;
  m_rMax = m_layerAtOuterRadius ? m_layerAtOuterRadius->surfaceRepresentation().bounds().r() : 0.;
  // protection
  if (m_rMin > m_rMax){
      // swap
      m_layerAtOuterRadius = m_layerAtInnerRadius;
      m_layerAtInnerRadius = layerAtOuterR;
      m_rMin = m_rMax;
      // redo the exercise with swapped values
      m_rMax = m_layerAtOuterRadius ? m_layerAtOuterRadius->surfaceRepresentation().bounds().r() : 0.;    
  }

  // the layers for the z extends
  std::vector<const Trk::CylinderLayer*> cylLayers;
	cylLayers.reserve(2);
    if (m_layerAtInnerRadius){
       cylLayers.push_back(m_layerAtInnerRadius);
       m_cachedBaseClassLayers.push_back(m_layerAtInnerRadius);
   } 
    if (m_layerAtOuterRadius){
       cylLayers.push_back(m_layerAtOuterRadius);
       m_cachedBaseClassLayers.push_back(m_layerAtOuterRadius);
   }
  // get the z extend through parsing
  std::vector<const Trk::CylinderLayer*>::const_iterator cylIter    =  cylLayers.begin();
  std::vector<const Trk::CylinderLayer*>::const_iterator cylIterEnd =  cylLayers.end();
  for ( ; cylIter != cylIterEnd; ++cylIter){
    const Amg::Vector3D& cylCenter = (*cylIter)->surfaceRepresentation().center();
    // get zMin/zMax
    double layerZmin = cylCenter.z() - (*cylIter)->surfaceRepresentation().bounds().halflengthZ();
    double layerZmax = cylCenter.z() + (*cylIter)->surfaceRepresentation().bounds().halflengthZ();
    // set the min/max values
    m_zMin = layerZmin < m_zMin ? layerZmin : m_zMin;
    m_zMax = layerZmax > m_zMax ? layerZmax : m_zMax;
   }
}       


Trk::EntryCylinderLayerProvider::~EntryCylinderLayerProvider()
{
   delete m_layerAtInnerRadius;
   delete m_layerAtOuterRadius;
}

const Trk::CylinderLayer* Trk::EntryCylinderLayerProvider::associatedLayer(const Amg::Vector3D& gp) const
{
   // simply the closest one
   double posR = gp.perp();
   ATH_MSG_VERBOSE( "associatedLayer() ... at pos: " << gp );
   if (fabs(posR-m_rMin) < fabs(posR-m_rMax))
	   ATH_MSG_VERBOSE( "associatedLayer() ... returning : m_layerAtInnerRadius = " << m_layerAtInnerRadius->layerIndex() );
   else
	   ATH_MSG_VERBOSE( "associatedLayer() ... returning : m_layerAtOuterRadius = " << m_layerAtOuterRadius->layerIndex() );
   return fabs(posR-m_rMin) < fabs(posR-m_rMax) ? m_layerAtInnerRadius : m_layerAtOuterRadius;

}

const Trk::CylinderLayer* Trk::EntryCylinderLayerProvider::entryLayer(const Amg::Vector3D& gp,
                                                                      const Amg::Vector3D& dir) const
{
  // only give entry layers when arriving through an entry surface
  double posR = gp.perp();
  ATH_MSG_VERBOSE( "entryLayer() ... at pos: " << gp );

  // the vetoes
  if (posR > m_rMin && posR < m_rMax) {
      ATH_MSG_VERBOSE( "entryLayer() ... m_rMin / posR / m_rMax = " << m_rMin << " / " << posR << " / " << m_rMax << " ... already in between - will return 0" );
      return 0;
  }

  // the obvious ones
  Amg::Vector3D postGp(gp+dir.normalized());
  double deltaR = postGp.perp() - posR;

  if (posR < m_rMin && deltaR > 0) {
      ATH_MSG_VERBOSE( "entryLayer() ... going outwards - inner entryLayer chosen: layerIndex = " << m_layerAtInnerRadius->layerIndex() );
      return m_layerAtInnerRadius;
  }
  else if (posR > m_rMax && deltaR < 0) {
      ATH_MSG_VERBOSE( "entryLayer() ... going inwards - outer entryLayer chosen: layerIndex = " << m_layerAtOuterRadius->layerIndex() );
      return m_layerAtOuterRadius;
  }
  // none of it complies
  if (posR < m_rMin && deltaR < 0) ATH_MSG_VERBOSE( "entryLayer() ... inside and going inwards, cannot find entryLayer - will return 0" );
  if (posR > m_rMax && deltaR > 0) ATH_MSG_VERBOSE( "entryLayer() ... outside and going outwards, cannot find entryLayer - will return 0" );
  // return nothing if no entry layer 
  return 0;
}

const Trk::CylinderLayer* Trk::EntryCylinderLayerProvider::exitLayer(const Amg::Vector3D& gp,
                                                                     const Amg::Vector3D& dir) const
{
  double posR = gp.perp();
  double posZ = gp.z();
  // use the CylinderIntersector to find the right one - force the unit vector !
  Amg::Vector3D postGp(gp+dir.normalized());
  double deltaR = postGp.perp() - posR;
  double deltaZ = dir.normalized().z();
  ATH_MSG_VERBOSE( "exitLayer() ... at pos / posR = " << gp << " / " << posR << " ... with direction: deltaR / deltaZ = " << deltaR << " / " << deltaZ );

  // decide which one to intersect - inner or outer cylinder only
  const Trk::CylinderLayer* intersectCylinder = (deltaR < 0) ? m_layerAtInnerRadius : m_layerAtOuterRadius;

  if (intersectCylinder) {
    // do the linear calculation
    double cylinderR = (deltaR < 0) ? m_rMin : m_rMax;
    double cylinderZ = posZ + deltaZ/deltaR * (cylinderR - posR);
    // return only if the intersection has been successful
    if (cylinderZ > (1.-m_tolerance*((m_zMin>0) ? 1 : -1))*m_zMin && cylinderZ < (1.+m_tolerance*((m_zMax>0) ? 1 : -1))*m_zMax) {
        if (deltaR < 0)
            ATH_MSG_VERBOSE( "exitLayer() ... going inwards - inner exitLayer chosen: layerIndex = " << intersectCylinder->layerIndex() );
        else
            ATH_MSG_VERBOSE( "exitLayer() ... going outwards - outer exitLayer chosen: layerIndex = " << intersectCylinder->layerIndex() );
        return intersectCylinder;
    }
    ATH_MSG_VERBOSE( "exitLayer() ... cylinderZ intersection not on exitLayer: m_zMin / cylinderZ / m_zMax = " << m_zMin << " / " << cylinderZ << " / " << m_zMax << " ... will return 0" );
    return 0;
  }
  ATH_MSG_VERBOSE( "exitLayer() ... CylinderLayer for intersection does not exist - will return 0" );
  return 0;
}
