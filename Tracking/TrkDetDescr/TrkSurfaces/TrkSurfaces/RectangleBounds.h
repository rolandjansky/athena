/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RectangleBounds.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_RECTANGLEBOUNDS_H
#define TRKSURFACES_RECTANGLEBOUNDS_H

#include "TrkSurfaces/SurfaceBounds.h"

class MsgStream;

#ifdef TRKDETDESCR_USEFLOATPRECISON
typedef float TDD_real_t;
#else
typedef double TDD_real_t;
#endif

namespace Trk {

/**
 @class RectangleBounds

 Bounds for a rectangular, planar surface.
 The two local coordinates locX, locY are for legacy reasons
 also called @f$ phi @f$ respectively @f$ eta @f$. The orientation
 with respect to the local surface framce can be seen in the attached
 illustration.

 @image html RectangularBounds.gif

 @author Andreas.Salzburger@cern.ch */

class RectangleBounds final : public SurfaceBounds
{

public:
  /** @enum BoundValues for readability */
  enum BoundValues
  {
    bv_halfX = 0,
    bv_halfY = 1,
    bv_length = 2
  };

  /**Default Constructor - needed for persistency*/
  RectangleBounds();

  /**Constructor with halflength in x (phi) and halflength in y (eta)*/
  RectangleBounds(double halex, double haley);

  /**Copy constructor*/
  RectangleBounds(const RectangleBounds& recbo);

  /**Destructor*/
  virtual ~RectangleBounds();

  /**Assignment Operator*/
  RectangleBounds& operator=(const RectangleBounds& recbo);

  /**Equality operator*/
  virtual bool operator==(const SurfaceBounds& sbo) const override final;

  /**Virtual constructor*/
  virtual RectangleBounds* clone() const override;

  /** Return the type of the bounds for persistency */
  virtual BoundsType type() const override final { return SurfaceBounds::Rectangle; }

  /**This method checks if the provided local coordinates are inside the surface bounds*/
  virtual bool inside(const Amg::Vector2D& locpo, double tol1 = 0., double tol2 = 0.) const override final;

  /**This method checks if the provided local coordinates are inside the surface bounds*/
  virtual bool inside(const Amg::Vector2D& locpo, const BoundaryCheck& bchk) const override final;

  /** This method checks inside bounds in loc1
    - loc1/loc2 correspond to the natural coordinates of the surface */
  virtual bool insideLoc1(const Amg::Vector2D& locpo, double tol1 = 0.) const override final;

  /** This method checks inside bounds in loc2
    - loc1/loc2 correspond to the natural coordinates of the surface */
  virtual bool insideLoc2(const Amg::Vector2D& locpo, double tol2 = 0.) const override final;

  /** Minimal distance to boundary ( > 0 if outside and <=0 if inside) */
  virtual double minDistance(const Amg::Vector2D& pos) const override final;

  /**This method returns the halflength in phi (first coordinate of local surface frame)*/
  double halflengthPhi() const;

  /**This method returns the halflength in Eta (second coordinate of local surface frame)*/
  double halflengthEta() const;

  /**for consistant naming*/
  double halflengthX() const;

  /**for consitant naming*/
  double halflengthY() const;

  /**This method returns the maximal extension on the local plane, i.e. @f$s\sqrt{h_{\phi}^2 + h_{\eta}^2}\f$*/
  virtual double r() const override;

  /** Output Method for MsgStream*/
  virtual MsgStream& dump(MsgStream& sl) const override;

  /** Output Method for std::ostream */
  virtual std::ostream& dump(std::ostream& sl) const override;

private:
  /** The internal version of the bounds can be float/double*/
  std::vector<TDD_real_t> m_boundValues;
};

} // end of namespace
#include "TrkSurfaces/RectangleBounds.icc"
#endif // TRKSURFACES_RECTANGLEBOUNDS_H
