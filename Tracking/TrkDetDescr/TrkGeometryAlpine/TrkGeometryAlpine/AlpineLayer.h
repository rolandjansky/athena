/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AlpineLayer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_ALPINELAYER_H
#define TRKGEOMETRY_ALPINELAYER_H

#include "TrkGeometry/CylinderLayer.h"
#include <fstream>
#include <vector>

namespace Trk {
  class AlpineStave;
  class Surface;

  class AlpineLayer : public CylinderLayer {
  public:
    // Default constructor
    AlpineLayer() {};
    
    // Concentric Layer: Constructor with CylinderSurface components and  MaterialProperties
    AlpineLayer(CylinderBounds* cbounds,
		const LayerMaterialProperties& laymatprop,
		double thickness = 0.,
		OverlapDescriptor* od = 0,
		IApproachDescriptor* ad = 0,
		int laytyp=int(Trk::active));

    virtual ~AlpineLayer();

    // Transforms the layer into a Surface representation for extrapolation
    const CylinderSurface& surfaceRepresentation() const override;

    virtual const Surface& surfaceOnApproach(const Amg::Vector3D& pos,
                                             const Amg::Vector3D& dir,
                                             PropDirection pDir,
                                             const BoundaryCheck& bcheck,
                                             bool resolveSubSurfaces = 0,
                                             const ICompatibilityEstimator* ice = 0) const;

    inline void addStave(AlpineStave *stave) { m_staves.push_back(stave); }
    inline std::vector<AlpineStave *> &getStaves() { return m_staves; }
    std::vector<const Surface*>& getSurfaces();
    inline void fillSurfaceArray(Trk::SurfaceArray* surfaceArray) { m_surfaceArray=surfaceArray; }
      
    // 2D root file dump
    void dump2D(std::ofstream &fout);

    // Layer has substructure if it has staves
    virtual bool hasSubStructure(bool resolveSensitive=false) const;
    
  private:
    // Find the closest staves in phi
    std::vector<int> getClosestStaves(double phi) const;
    // The layer contains staves ordered from -PI to PI
    std::vector<AlpineStave *> m_staves;
    // The layer contains surfaces (used for drawing the geometry)
    std::vector<const Surface *> m_surfaces;
  };
}

#endif // TRKGEOMETRY_ALPINELAYER_H

