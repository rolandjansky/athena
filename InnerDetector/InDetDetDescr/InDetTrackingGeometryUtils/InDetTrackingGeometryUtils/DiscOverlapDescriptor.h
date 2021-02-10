/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DiscOverlapDescriptor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETTRACKINGGEOMETRYUTILS_DISCOVERLAPDESCRIPTOR_H
#define INDETTRACKINGGEOMETRYUTILS_DISCOVERLAPDESCRIPTOR_H

// Trk
#include "TrkGeometry/OverlapDescriptor.h"
// Trk inlcude
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/BinnedArray1D1D.h"
#include "TrkDetDescrUtils/Intersection.h"

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"

#ifndef INDETTRACKINGGEOMETRY_DISCDETADDNEXTPHIETA
#define INDETTRACKINGGEOMETRY_DISCDETADDNEXTPHIETA
#define addSurfaceDO(cur,surfaces) if (cur) surfaces.push_back(Trk::SurfaceIntersection(Trk::Intersection(Amg::Vector3D(0.,0.,0.),0.,true),&(cur->surface(cur->identify()))))
#define addOtherSideDO(cur, surfaces) if (cur && cur->otherSide()) surfaces.push_back(Trk::SurfaceIntersection(Trk::Intersection(Amg::Vector3D(0.,0.,0.),0.,true),&(cur->otherSide()->surface(cur->otherSide()->identify()))))
#define addNextInPhiDO(cur, surfaces) addSurfaceDO(cur->nextInPhi(), surfaces); addOtherSideDO(cur->nextInPhi(),surfaces)
#define addPrevInPhiDO(cur, surfaces) addSurfaceDO(cur->prevInPhi(), surfaces); addOtherSideDO(cur->prevInPhi(),surfaces)
#define addNextInEtaDO(cur, surfaces) addSurfaceDO(cur->nextInEta(), surfaces); addOtherSideDO(cur->nextInEta(),surfaces)
#define addPrevInEtaDO(cur, surfaces) addSurfaceDO(cur->prevInEta(), surfaces); addOtherSideDO(cur->prevInEta(),surfaces)
#endif // INDETTRACKINGGEOMETRY_DISCDETADDNEXTPHIETA

namespace Trk {
  class Surface;
}

namespace InDetDD {   
     class SiDetectorElement;
}

namespace InDet {
    
  /** @class DiscOverlapDescriptor
         
      Class to describe overlaps in the custom detector,
      it extends the Trk::OverlapDescriptor base class.
        
      There are two interface methods, one provides the most probably overlapcell,
      the second provides a list of overlap cells, based on an restricted area
        
      @author Noemi.Calace@cern.ch
        
  */
  class DiscOverlapDescriptor : public Trk::OverlapDescriptor {
  public:
    
    /** Constructor */
    DiscOverlapDescriptor(bool isPixel = false,
			  const Trk::BinnedArray<Trk::Surface>* bin_array = 0,
			  std::vector<Trk::BinUtility*>* singleBinUtils = 0);
    
    /** Destructor */
    virtual ~DiscOverlapDescriptor() {
      if (m_singleBinUtils) {
	std::vector<Trk::BinUtility*>::iterator binIter = m_singleBinUtils->begin();
	for ( ; binIter != m_singleBinUtils->end(); ++binIter)
	  delete *binIter;
      }
      delete m_singleBinUtils;
    }
    
    /**Pseudo-Constructor*/
    DiscOverlapDescriptor* clone() const;
    
    /** get the compatible surfaces 
        - return vector : surfaces
        - primary bin surface : sf
        - position & direction : pos, dir
    */
    bool reachableSurfaces(std::vector<Trk::SurfaceIntersection>& surfaces, 
                           const Trk::Surface& sf,
                           const Amg::Vector3D& pos,
                           const Amg::Vector3D& dir) const override;
                           
    bool reachableSurfaces(std::vector<Trk::SurfaceIntersection>& surfaces, 
                           const Amg::Vector3D& pos,
                           const Amg::Vector3D& dir) const override;
    
  private:
    void dumpSurfaces(const PixelID* pixId, const SCT_ID* sctId, std::vector<Trk::SurfaceIntersection>& surfaces) const;
    
    const Trk::BinnedArray<Trk::Surface>*           m_bin_array;
    std::vector<Trk::BinUtility*>*                  m_singleBinUtils;
    bool                                            m_pixelCase;
    
    
  };
  
  
  inline DiscOverlapDescriptor* DiscOverlapDescriptor::clone() const { return new DiscOverlapDescriptor(); }     
  
  inline bool DiscOverlapDescriptor::reachableSurfaces(std::vector<Trk::SurfaceIntersection>& /*surfaces*/, 
                                                       const Amg::Vector3D& /*pos*/,
                                                       const Amg::Vector3D& /*dir*/) const {
    return false;
  }
  
}

#endif // end of INDETTRACKINGGEOMETRYUTILS_DISCOVERLAPDESCRIPTOR_H
