/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SlidingDiscSurface.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRYSURFACES_SLIDINGDISCSURFACE_H
#define TRKGEOMETRYSURFACES_SLIDINGDISCSURFACE_H

// Trk
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkParametersBase/ParametersT.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/NoBounds.h"
#include "TrkSurfaces/SurfaceBounds.h"
// Amg
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;

namespace Trk {

class DiscBounds;
class TrkDetElementBase;
class LocalParameters;

template<int DIM, class T, class S>
class ParametersT;

/**
 @class SlidingDiscSurface
 Class for a Calo DiscSurface with variable depth in the ATLAS detector.
 The variable depth is stored as a binned vector of shifts of the center along
 the normal. Local eta bin is defined by locR and z position for base transform
 ( corrected for misalignement ). Inherits from DiscSurface.

 @author sarka.todorova@cern.ch
 */

class SlidingDiscSurface : public DiscSurface
{

public:
  /**Default Constructor*/
  SlidingDiscSurface();

  /**Constructor */
  SlidingDiscSurface(DiscSurface& surf,
                     Trk::BinUtility* bu = nullptr,
                     const std::vector<float>* offset = nullptr,
                     Amg::Transform3D* align = nullptr);

  /**Constructor for DiscSegment from DetectorElement*/
  // DiscSurface(const TrkDetElementBase& dmnt);

  /**Copy Constructor*/
  SlidingDiscSurface(const SlidingDiscSurface& psf);

  /**Copy Constructor with shift*/
  SlidingDiscSurface(const SlidingDiscSurface& psf,
                     const Amg::Transform3D& transf);

  /**Constructor */
  SlidingDiscSurface(const DiscSurface& surf,
                     Trk::BinUtility* bu = nullptr,
                     const std::vector<float>* offset = nullptr,
                     Amg::Transform3D* align = nullptr);

  /**Destructor*/
  virtual ~SlidingDiscSurface();

  /**Assignement operator*/
  SlidingDiscSurface& operator=(const SlidingDiscSurface& dsf);

  /**Equality operator*/
  virtual bool operator==(const Surface& sf) const override;

  /** Virtual constructor*/
  virtual SlidingDiscSurface* clone() const override;

  /** This method returns true if the GlobalPosition is on the Surface for both,
    within or without check of whether the local position is inside boundaries
    or not */
  virtual bool isOnSurface(const Amg::Vector3D& glopo,
                           BoundaryCheck bchk = true,
                           double tol1 = 0.,
                           double tol2 = 0.) const override;

  /** Specialized for DiscSurface: LocalToGlobal method without dynamic memory
   * allocation */
  virtual void localToGlobal(const Amg::Vector2D& locp,
                             const Amg::Vector3D& mom,
                             Amg::Vector3D& glob) const override;

  /** Specialized for DiscSurface: GlobalToLocal method without dynamic memory
   * allocation - boolean checks if on surface
   */
  virtual bool globalToLocal(const Amg::Vector3D& glob,
                             const Amg::Vector3D& mom,
                             Amg::Vector2D& loc) const override;

  /** fast straight line intersection schema - standard: provides closest
     intersection and (signed) path length forceDir is to provide the closest
     forward solution

      <b>mathematical motivation:</b>

      the equation of the plane is given by: <br>
      @f$ \vec n \cdot \vec x = \vec n \cdot \vec p,@f$ <br>
      where @f$ \vec n = (n_{x}, n_{y}, n_{z})@f$ denotes the normal vector of
     the plane,
      @f$ \vec p = (p_{x}, p_{y}, p_{z})@f$ one specific point on the plane and
     @f$ \vec x = (x,y,z) @f$ all possible points on the plane.<br> Given a line
     with:<br>
      @f$ \vec l(u) = \vec l_{1} + u \cdot \vec v @f$, <br>
      the solution for @f$ u @f$ can be written:
      @f$ u = \frac{\vec n (\vec p - \vec l_{1})}{\vec n \vec v}@f$ <br>
      If the denominator is 0 then the line lies:
      - either in the plane
      - perpenticular to the normal of the plane
   */

  /** fast straight line distance evaluation to Surface */
  virtual DistanceSolution straightLineDistanceEstimate(
    const Amg::Vector3D& pos,
    const Amg::Vector3D& dir) const override;

  /** fast straight line distance evaluation to Surface - with bound option*/
  virtual DistanceSolution straightLineDistanceEstimate(
    const Amg::Vector3D& pos,
    const Amg::Vector3D& dir,
    bool Bound) const override;

  /**This method allows access to the bin utility*/
  const Trk::BinUtility* binUtility() const { return m_etaBin; }

  /**This method allows access to the radial offset values*/
  const std::vector<float>* offset() const { return m_depth; }

  /** Return properly formatted class name for screen output */
  virtual std::string name() const override
  {
    return "Trk::SlidingDiscSurface";
  }

protected: //!< data members
  const std::vector<float>* m_depth;
  Trk::BinUtility* m_etaBin;
  Amg::Transform3D* m_align;
};

inline SlidingDiscSurface*
SlidingDiscSurface::clone() const
{
  return new SlidingDiscSurface(*this);
}

} // end of namespace

#endif // TRKSURFACES_SLIDINGDISCSURFACE_H
