/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RotatedDiamondBounds.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_ROTATEDDIAMONDDBOUNDS_H
#define TRKSURFACES_ROTATEDDIAMONDDBOUNDS_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkSurfaces/SurfaceBounds.h"

#include <math.h>

class MsgStream;
class RotatedDiamondBoundsCnv_p1;

#ifdef TRKDETDESCR_USEFLOATPRECISON
typedef float TDD_real_t;
#else
typedef double TDD_real_t;
#endif

namespace Trk {

/**
@class RotatedDiamondBounds
Bounds for a double trapezoidal ("diamond"), planar Surface.

The RotatedDiamondBounds Class specifies the same diamond as the DiamondBounds Class. The local X and Y coordinates will
be interchanged internally for boundary checks. (This is opposed to the TrapezoidBounds, where X and Y are interchanged
in the constructor.)

@author Andreas.Salzburger@cern.ch, Sarka.Todorova@cern.ch, Peter.Kluit@cern.ch, Alice.Alfonsi@cern.ch,
Michiel.Jan.Veen@cern.ch
*/

class RotatedDiamondBounds final: public SurfaceBounds
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
  RotatedDiamondBounds();

  /**Constructor for symmetric Diamond*/
  RotatedDiamondBounds(double minhalex, double medhalex, double maxhalex, double haley1, double haley2);

  /**Copy constructor*/
  RotatedDiamondBounds(const RotatedDiamondBounds& diabo) = default;

  /**Destructor*/
  virtual ~RotatedDiamondBounds() = default;

  /**Virtual constructor*/
  RotatedDiamondBounds* clone() const override;

  /**Assignment operator*/
  RotatedDiamondBounds& operator=(const RotatedDiamondBounds& sbo) = default;

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
  friend class ::RotatedDiamondBoundsCnv_p1;

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

#include "TrkSurfaces/RotatedDiamondBounds.icc"
#endif // TRKSURFACES_DIAMONDBOUNDS_H
