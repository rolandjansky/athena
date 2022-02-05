 
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
// STL include
#include <atomic>

namespace Trk {
  class Surface;
}

namespace InDetDD {   
  class HGTD_DetectorElement;
}

class HGTD_ID;

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
  HGTD_OverlapDescriptor(const Trk::BinnedArray<const Trk::Surface>* bin_array = nullptr,
                         std::vector < float > valuesR = {},
                         std::vector < std::vector< float> > valuesPhi = {},
                         int nStepsR=3, int nStepsPhi=10);

  /** Destructor */
  virtual ~HGTD_OverlapDescriptor() {
  }

  ///Delete copy
  HGTD_OverlapDescriptor(const HGTD_OverlapDescriptor &) = delete;

  ///Delete assignment
  HGTD_OverlapDescriptor & operator=(const HGTD_OverlapDescriptor &) = delete;

  /**Pseudo-Constructor*/
  virtual HGTD_OverlapDescriptor* clone() const override;

  /** get the compatible surfaces 
      - return vector : surfaces
      - primary bin surface : sf
      - position & direction : pos, dir
  */
  bool reachableSurfaces(std::vector<Trk::SurfaceIntersection>& surfaces,
                         const Trk::Surface& sf,
                         const Amg::Vector3D& pos,
                         const Amg::Vector3D& dir) const override;

private:
  bool dumpSurfaces(std::vector<Trk::SurfaceIntersection>& surfaces) const;

  const Trk::BinnedArray<const Trk::Surface>*   m_binnedArray;
  std::vector < float >                   m_valuesR;
  std::vector < std::vector< float> >     m_valuesPhi;
  int                                     m_nStepsR;
  int                                     m_nStepsPhi;
  mutable std::atomic<const HGTD_ID*>     m_hgtdIdHelper{nullptr};

};


inline HGTD_OverlapDescriptor* HGTD_OverlapDescriptor::clone() const {
  return new HGTD_OverlapDescriptor();
}

#endif // end of HGTDET_HGTDTRACKINGGEOMETRY_HGTDOVERLAPDESCRIPTOR
