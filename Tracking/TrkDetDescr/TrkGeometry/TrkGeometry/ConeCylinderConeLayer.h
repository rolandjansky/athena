/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ConeCylinderConeLayer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_CONECYLINDERCONELAYER_H
#define TRKGEOMETRY_CONECYLINDERCONELAYER_H

class MsgStream;

#include "TrkGeometry/Layer.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/CompoundLayer.h"
#include "TrkSurfaces/ConeSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkEventPrimitives/PropDirection.h"
// STL sorting
#include <algorithm>

namespace Trk {
    
  class ConeCylinderConeLayer : public CompoundLayer, public CylinderLayer {
    public:
    /** Default constructor */
    ConeCylinderConeLayer() {}

    /** Build the layer out of the given parameters */
    ConeCylinderConeLayer(Amg::Transform3D* transform,
			  double radius,
			  double halfLength,
			  double zStitch,
			  double alpha,
			  SurfaceArray* surfaceArray,
			  const LayerMaterialProperties& laymatprop,
			  double thickness = 0.,
			  OverlapDescriptor* od = 0,
			  int laytyp=int(Trk::active));

    virtual const ConeCylinderConeLayer& layerRepresentation() const override {return (*this);}
    
    virtual const CylinderSurface& surfaceRepresentation() const override {return (*this);}
    
    virtual bool isOnLayer(const Amg::Vector3D& gp, const BoundaryCheck&) const override {return isOnCompoundLayer(gp,0.5*thickness());}
    
    double radius(const Amg::Vector3D& gp) const
    {
      Amg::Vector3D gploc;
      if(m_transform)
	      gploc = (*m_transform).inverse()*gp;
      else
	      gploc = gp;
      
      if(fabs(gploc.z()) > m_halfLength)
	return 0.;
      if(fabs(gploc.z()) < m_zStitch)
	return m_radius;
      /// its on one of the cones
      return m_radius - (fabs(gploc.z()) - m_zStitch) * tan(m_alpha);
    }
    double pathCorr(double pathTheta, const Amg::Vector3D& gp) const
    {
      Amg::Vector3D gploc;
      if(m_transform)
	gploc = (*m_transform).inverse()*gp;
      else
	gploc = gp;
      
      if(fabs(gploc.z()) > m_halfLength)
	return 0.;
      if(fabs(gploc.z()) < m_zStitch)
	return fabs(sin(pathTheta));
      /// its on one of the cones
      return fabs(sin(pathTheta+m_alpha));
    }
  private:
    Amg::Transform3D* m_transform;
    double m_radius;
    double m_halfLength;
    double m_zStitch;
    double m_alpha;
  };
} // end of namespace

#endif // TRKGEOMETY_CONECYLINDERCONELAYER_H
