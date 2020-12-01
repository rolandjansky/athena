/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DiscTrapezoidalBounds.h, c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_DISCTRAPEZOIDALBOUNDS_H
#define TRKSURFACES_DISCTRAPEZOIDALBOUNDS_H

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
   @class DiscTrapezoidalBounds

   Class to describe the bounds for a planar DiscSurface.
   By providing an argument for hphisec, the bounds can
   be restricted to a phirange around the center position.

   @author Noemi.Calace@cern.ch , Andreas.Salzburger@cern.ch
*/

class DiscTrapezoidalBounds final : public SurfaceBounds
{

public:
  /** enumeration for readability */
  enum BoundValues
  {
    bv_rMin = 0,
    bv_rMax = 1,
    bv_minHalfX = 2,
    bv_maxHalfX = 3,
    bv_halfY = 4,
    bv_halfPhiSector = 5,
    bv_averagePhi = 6,
    bv_rCenter = 7,
    bv_stereo = 8,
    bv_length = 9
  };

  /**Default Constructor*/
  DiscTrapezoidalBounds();

  /**Constructor for a symmetric Trapezoid giving min X lenght, max X lenght, Rmin and R max */
  DiscTrapezoidalBounds(double minhalfx, double maxhalfx, double rMin, double rMax, double avephi, double stereo = 0.);

  /**Copy constructor*/
  DiscTrapezoidalBounds(const DiscTrapezoidalBounds& disctrbo);

  /**Destructor*/
  virtual ~DiscTrapezoidalBounds();

  /**Assignment operator*/
  DiscTrapezoidalBounds& operator=(const DiscTrapezoidalBounds& disctrbo);

  /**Equality operator*/
  virtual bool operator==(const SurfaceBounds& sbo) const override;

  /**Virtual constructor*/
  virtual DiscTrapezoidalBounds* clone() const override;

  /** Return the type - mainly for persistency */
  virtual SurfaceBounds::BoundsType type() const override final { return SurfaceBounds::DiscTrapezoidal; }

  /**This method cheks if the radius given in the LocalPosition is inside [rMin,rMax]
     if only tol1 is given and additional in the phi sector is tol2 is given */
  virtual bool inside(const Amg::Vector2D& locpo, double tol1 = 0., double tol2 = 0.) const override final;
  virtual bool inside(const Amg::Vector2D& locpo, const BoundaryCheck& bchk) const override final;

  /** This method checks inside bounds in loc1
      - loc1/loc2 correspond to the natural coordinates of the surface */
  virtual bool insideLoc1(const Amg::Vector2D& locpo, double tol1 = 0.) const override final;

  /** This method checks inside bounds in loc2
      - loc1/loc2 correspond to the natural coordinates of the surface */
  virtual bool insideLoc2(const Amg::Vector2D& locpo, double tol2 = 0.) const override final;

  /** Minimal distance to boundary ( > 0 if outside and <=0 if inside) */
  virtual double minDistance(const Amg::Vector2D& pos) const override final;

  /**This method returns inner radius*/
  double rMin() const;

  /**This method returns outer radius*/
  double rMax() const;

  /**This method returns the maximum expansion on the plane (=rMax)*/
  virtual double r() const override;

  /**This method returns the average phi*/
  double averagePhi() const;

  /**This method returns the center radius*/
  double rCenter() const;

  /**This method returns the stereo angle*/
  double stereo() const;

  /**This method returns the halfPhiSector which is covered by the disc*/
  double halfPhiSector() const;

  /**This method returns the minimal halflength in X */
  double minHalflengthX() const;

  /**This method returns the maximal halflength in X */
  double maxHalflengthX() const;

  /**This method returns the halflength in Y (this is Rmax -Rmin)*/
  double halflengthY() const;

  /** Output Method for MsgStream*/
  virtual MsgStream& dump(MsgStream& sl) const override;

  /** Output Method for std::ostream */
  virtual std::ostream& dump(std::ostream& sl) const override;

private:
  /** Internal members of the bounds (float/double)*/
  std::vector<TDD_real_t> m_boundValues;
};

} // end of namespace
#include "TrkSurfaces/DiscTrapezoidalBounds.icc"
#endif // TRKSURFACES_DISCTRAPEZOIDALBOUNDS_H
