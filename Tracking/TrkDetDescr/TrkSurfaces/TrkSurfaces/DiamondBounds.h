/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DiamondBounds.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_DIAMONDDBOUNDS_H
#define TRKSURFACES_DIAMONDDBOUNDS_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkSurfaces/SurfaceBounds.h"

#include <math.h>

class MsgStream;
class DiamondBoundsCnv_p1;

#ifdef TRKDETDESCR_USEFLOATPRECISON
typedef float TDD_real_t;
#else
typedef double TDD_real_t;
#endif

namespace Trk {

/**
 @class DiamondBounds
 Bounds for a double trapezoidal ("diamond"), planar Surface.

 @author Andreas.Salzburger@cern.ch, Sarka.Todorova@cern.ch
 */

class DiamondBounds : public SurfaceBounds
{

public:
  /** BoundValues for better readability */
  enum BoundValues
  {
    bv_minHalfX = 0,
    bv_medHalfX = 1,
    bv_maxHalfX = 2,
    bv_halfY1 = 3,
    bv_halfY2 = 4,
    bv_length = 5
  };

  /**Default Constructor, needed for persistency*/
  DiamondBounds();

  /**Constructor for symmetric Diamond*/
  DiamondBounds(double minhalex, double medhalex, double maxhalex, double haley1, double haley2);

  /**Copy constructor*/
  DiamondBounds(const DiamondBounds& diabo) = default;

  /**Destructor*/
  virtual ~DiamondBounds() = default;

  /**Virtual constructor*/
  DiamondBounds* clone() const override;

  /**Assignment operator*/
  DiamondBounds& operator=(const DiamondBounds& sbo) = default;

  /**Equality operator*/
  virtual bool operator==(const SurfaceBounds& diabo) const override;

  /** Return the bounds type */
  virtual BoundsType type() const override { return SurfaceBounds::Diamond; }

  /**This method returns the halflength in X at minimal Y (first coordinate of local surface frame)*/
  double minHalflengthX() const;

  /**This method returns the (maximal) halflength in X (first coordinate of local surface frame)*/
  double medHalflengthX() const;

  /**This method returns the halflength in X at maximal Y (first coordinate of local surface frame)*/
  double maxHalflengthX() const;

  /**This method returns the halflength in Y of trapezoid at negative/positive Y (second coordinate)*/
  double halflengthY1() const;
  double halflengthY2() const;

  /**This method returns the maximal extension on the local plane*/
  virtual double r() const override;

  /**This method returns the opening angle alpha in point A   */
  double alpha1() const;

  /**This method returns the opening angle alpha in point A'  */
  double alpha2() const;

  /** The orientation of the Diamond is according to the figure */
  virtual bool inside(const Amg::Vector2D& locpo, double tol1 = 0., double tol2 = 0.) const override;
  virtual bool inside(const Amg::Vector2D& locpo, const BoundaryCheck& bchk) const override;

  /** This method checks inside bounds in loc1
  - loc1/loc2 correspond to the natural coordinates of the surface
  - As loc1/loc2 are correlated the single check doesn't make sense :
     -> check is done on enclosing Rectangle !
  */
  virtual bool insideLoc1(const Amg::Vector2D& locpo, double tol1 = 0.) const override;

  /** This method checks inside bounds in loc2
  - loc1/loc2 correspond to the natural coordinates of the surface
  - As loc1/loc2 are correlated the single check doesn't make sense :
     -> check is done on enclosing Rectangle !
  */
  virtual bool insideLoc2(const Amg::Vector2D& locpo, double tol2 = 0.) const override;

  /** Minimal distance to boundary ( > 0 if outside and <=0 if inside) */
  virtual double minDistance(const Amg::Vector2D& pos) const override;

  /** Output Method for MsgStream*/
  virtual MsgStream& dump(MsgStream& sl) const override;

  /** Output Method for std::ostream */
  virtual std::ostream& dump(std::ostream& sl) const override;

private:
  friend class ::DiamondBoundsCnv_p1;

  /** inside() method for a full symmetric diamond */
  bool insideFull(const Amg::Vector2D& locpo, double tol1 = 0., double tol2 = 0.) const;

  /** initialize the alpha1/2 cache - needed also for object persistency */
  virtual void initCache() override;

  /** Internal parameters stored in the geometry */
  std::vector<TDD_real_t> m_boundValues;
  TDD_real_t m_alpha1;
  TDD_real_t m_alpha2;
};

} // end of namespace

#include "TrkSurfaces/DiamondBounds.icc"
#endif // TRKSURFACES_DIAMONDBOUNDS_H
