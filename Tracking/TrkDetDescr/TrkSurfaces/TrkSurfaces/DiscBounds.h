/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DiscBounds.h, c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_DISCBOUNDS_H
#define TRKSURFACES_DISCBOUNDS_H

// Trk
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkSurfaces/SurfaceBounds.h"
//
#include <cmath>
#include <math.h>
// Eigen
#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;

#ifdef TRKDETDESCR_USEFLOATPRECISON
typedef float TDD_real_t;
#else
typedef double TDD_real_t;
#endif

namespace Trk {

/**
 @class DiscBounds

 Class to describe the bounds for a planar DiscSurface.
 By providing an argument for hphisec, the bounds can
 be restricted to a phirange around the center position.

 @image html DiscBounds.gif

 @author Andreas.Salzburger@cern.ch
 */

class DiscBounds : public SurfaceBounds
{

public:
  /** enumeration for readability */
  enum BoundValues
  {
    bv_rMin = 0,
    bv_rMax = 1,
    bv_averagePhi = 2,
    bv_halfPhiSector = 3,
    bv_length = 4
  };
  /**Default Constructor*/
  DiscBounds();

  /**Constructor for full disc of symmetric disc around phi=0*/
  DiscBounds(double minrad, double maxrad, double hphisec = M_PI);

  /**Constructor for a symmetric disc around phi != 0*/
  DiscBounds(double minrad, double maxrad, double avephi, double hphisec);

  /**Default copy constructor*/
  DiscBounds(const DiscBounds&) = default;

  /**Destructor*/
  virtual ~DiscBounds();

  /**Default move constructor*/
  DiscBounds(DiscBounds&& discbo) = default;

  /**Default move assignment operator*/
  DiscBounds& operator=(DiscBounds&& discbo) = default;

  /**Equality operator*/
  virtual bool operator==(const SurfaceBounds& sbo) const override;

  /**Virtual constructor*/
  virtual DiscBounds* clone() const override;

  /** Return the type - mainly for persistency */
  virtual SurfaceBounds::BoundsType type() const override { return SurfaceBounds::Disc; }

  /**This method cheks if the radius given in the LocalPosition is inside [rMin,rMax]
     if only tol1 is given and additional in the phi sector is tol2 is given */
  virtual bool inside(const Amg::Vector2D& locpo, double tol1 = 0., double tol2 = 0.) const override;
  virtual bool inside(const Amg::Vector2D& locpo, const BoundaryCheck& bchk) const override;

  /** This method checks inside bounds in loc1
    - loc1/loc2 correspond to the natural coordinates of the surface */
  virtual bool insideLoc1(const Amg::Vector2D& locpo, double tol1 = 0.) const override;

  /** This method checks inside bounds in loc2
    - loc1/loc2 correspond to the natural coordinates of the surface */
  virtual bool insideLoc2(const Amg::Vector2D& locpo, double tol2 = 0.) const override;

  /** Minimal distance to boundary ( > 0 if outside and <=0 if inside) */
  virtual double minDistance(const Amg::Vector2D& pos) const override;

  /**This method returns inner radius*/
  double rMin() const;

  /**This method returns outer radius*/
  double rMax() const;

  /**This method returns the maximum expansion on the plane (=rMax)*/
  virtual double r() const override;

  /**This method returns the average phi*/
  double averagePhi() const;

  /**This method returns the halfPhiSector which is covered by the disc*/
  double halfPhiSector() const;

  /** Output Method for MsgStream*/
  virtual MsgStream& dump(MsgStream& sl) const override;

  /** Output Method for std::ostream */
  virtual std::ostream& dump(std::ostream& sl) const override;

private:
  /** Internal members of the bounds (float/double)*/
  std::vector<TDD_real_t> m_boundValues;
};

} // end of namespace

#include "TrkSurfaces/DiscBounds.icc"
#endif // TRKSURFACES_DISCBOUNDS_H
