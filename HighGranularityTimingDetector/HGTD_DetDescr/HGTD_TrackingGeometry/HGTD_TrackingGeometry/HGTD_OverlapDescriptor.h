/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HGTD_OverlapDescriptor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef HGTDET_HGTDTRACKINGGEOMETRY_HGTDOVERLAPDESCRIPTOR
#define HGTDET_HGTDTRACKINGGEOMETRY_HGTDOVERLAPDESCRIPTOR

// Trk
#include "TrkGeometry/OverlapDescriptor.h"
// Trk inlcude
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/Intersection.h"

namespace Trk {
  class Surface;
}

namespace InDetDD {   
  class HGTD_DetectorElement;
}

class HGTD_ID;


namespace HGTDet {
    
  /** @class HGTD_OverlapDescriptor
   * Class to describe overlaps in the HGTD detector.
   * It extends the Trk::OverlapDescriptor base class.
   * 
   * There are two interface methods, one provides the most probably overlapcell,
   * the second provides a list of overlap cells, based on an restricted area
   *
   */
  
  class HGTD_OverlapDescriptor : public Trk::OverlapDescriptor {
  public:
    
    /** Constructor */
    HGTD_OverlapDescriptor(const Trk::BinnedArray<Trk::Surface>* bin_array = nullptr,
                           std::vector < float > valuesR = {},
                           std::vector < std::vector< float> > valuesPhi = {},
                           int nStepsR=3, int nStepsPhi=10);
    
    /** Destructor */
    virtual ~HGTD_OverlapDescriptor() {
    }
    
    /**Pseudo-Constructor*/
    HGTD_OverlapDescriptor* clone() const;
    
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
    bool getSurfaces(std::vector<Trk::SurfaceIntersection>& surfaces, const Amg::Vector3D& pos) const;
    
    void dumpSurfaces(std::vector<Trk::SurfaceIntersection>& surfaces) const;
    
    
    float                                   m_binSize;
    const Trk::BinnedArray<Trk::Surface>*   m_binnedArray;
    std::vector < float >                   m_valuesR;
    std::vector < std::vector< float> >     m_valuesPhi;
    int                                     m_nStepsR;
    int                                     m_nStepsPhi;

  };
  
  
  inline HGTD_OverlapDescriptor* HGTD_OverlapDescriptor::clone() const { 
    return new HGTD_OverlapDescriptor(); 
  }
  
}

#endif // end of HGTDET_HGTDTRACKINGGEOMETRY_HGTDOVERLAPDESCRIPTOR
