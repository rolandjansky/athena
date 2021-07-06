/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TriangleBounds.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_TRIANGLEBOUNDS_H
#define TRKSURFACES_TRIANGLEBOUNDS_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkSurfaces/SurfaceBounds.h"
#include <utility>

class MsgStream;

#ifdef TRKDETDESCR_USEFLOATPRECISON
typedef float TDD_real_t;
#else
typedef double TDD_real_t;
#endif

namespace Trk {

/**
 @class TriangleBounds

 Bounds for a triangular, planar surface.

 @internal
 @image html TriangularBounds.gif
 @endinternal

 @author sarka.todorova@cern.ch */

class TriangleBounds final : public SurfaceBounds
{

public:
  /** @enum BoundValues for readability */
  enum BoundValues
  {
    bv_x1 = 0,
    bv_y1 = 1,
    bv_x2 = 2,
    bv_y2 = 3,
    bv_x3 = 4,
    bv_y3 = 5,
    bv_length = 6
  };

  /**Default Constructor - needed for persistency*/
  TriangleBounds();

  /**Copy constructor*/
  TriangleBounds(const TriangleBounds& tribo) = default;

  /**Assignment Operator*/
  TriangleBounds& operator=(const TriangleBounds& recbo) = default;

  /**Move constructor*/
  TriangleBounds(TriangleBounds&& tribo) noexcept = default;

  /**Move Assignment Operator*/
  TriangleBounds& operator=(TriangleBounds&& recbo) noexcept = default;

  /**Destructor*/
  virtual ~TriangleBounds() = default;

  /**Constructor with coordinates of vertices - floats*/
  TriangleBounds(const std::vector<std::pair<float, float>>&);

  /**Constructor with coordinates of vertices - double*/
  TriangleBounds(const std::vector<std::pair<double, double>>&);

  /**Constructor from three 2 Vectors */
  TriangleBounds(const Amg::Vector2D& p1, const Amg::Vector2D& p2, const Amg::Vector2D& p3);

  /**Equality operator*/
  virtual bool operator==(const SurfaceBounds& sbo) const override;

  /**Virtual constructor*/
  virtual TriangleBounds* clone() const override;

  /** Return the type of the bounds for persistency */
  virtual BoundsType type() const override final { return SurfaceBounds::Triangle; }

  /**This method checks if the provided local coordinates are inside the surface bounds*/
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

  /**This method returns the coordinates of vertices*/
  std::vector<std::pair<TDD_real_t, TDD_real_t>> vertices() const;

  /**This method returns the maximal extension on the local plane, i.e. @f$s\sqrt{h_{\phi}^2 + h_{\eta}^2}\f$*/
  virtual double r() const override final;

  /** Output Method for MsgStream*/
  virtual MsgStream& dump(MsgStream& sl) const override;

  /** Output Method for std::ostream */
  virtual std::ostream& dump(std::ostream& sl) const override;

private:
  std::vector<TDD_real_t> m_boundValues;
};
} // end of namespace

#include "TrkSurfaces/TriangleBounds.icc"
#endif // TRKSURFACES_RECTANGLEBOUNDS_H
