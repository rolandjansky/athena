/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CylinderBounds.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_CYLINDERBOUNDS_H
#define TRKSURFACES_CYLINDERBOUNDS_H

#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkSurfaces/SurfaceBounds.h"
// Eigen Primitives
#include "GeoPrimitives/GeoPrimitives.h"

#ifdef TRKDETDESCR_USEFLOATPRECISON
typedef float TDD_real_t;
#else
typedef double TDD_real_t;
#endif

class MsgStream;

namespace Trk {

/**
 @class CylinderBounds

 Bounds for a cylindrical Surface.
 These bounds may be used for both, CylinderSurface and StraightLineSurface.
 In case of bounds for a StraightLineSurface the radius determines the radius within a localPosition
 is regarded as inside bounds.

 Trk::CylinderBounds also enhance the possibility of a cylinder segment with an opening angle @f$ 2\cdot\phi_{half}@f$
 around an average @f$ \phi @f$ angle @f$ \phi_{ave} @f$.

 @todo update the documentation picture for cylinder segments

 @image html CylinderBounds.gif

 @author Andreas.Salzburger@cern.ch
 */

class CylinderBounds final: public SurfaceBounds
{
public:
  /** BoundValues for readablility */
  enum BoundValues
  {
    bv_radius = 0,
    bv_averagePhi = 1,
    bv_halfPhiSector = 2,
    bv_halfZ = 3,
    bv_length = 4
  };

  /**Default Constructor*/
  CylinderBounds();

  /**Constructor - full cylinder */
  CylinderBounds(double radius, double halez);

  /**Constructor - cylinder segment */
  CylinderBounds(double radius, double halfphi, double halez);

  /**Constructor - cylinder segment with given averagePhi, not supposed for CylinderSurfaces*/
  CylinderBounds(double radius, double halfphi, double avphi, double halez);

  /**Copy Constructor */
  CylinderBounds(const CylinderBounds& cylbo) = default;

  /**Move Constructor */
  CylinderBounds(CylinderBounds&& cylbo) = default;

  /**Destructor */
  virtual ~CylinderBounds() = default;

  /**Assignment operator*/
  CylinderBounds& operator=(const CylinderBounds& cylbo) = default;

  /**Move assignment operator*/
  CylinderBounds& operator=(CylinderBounds&& cylbo) = default;
  
  /**Equality operator*/
  virtual bool operator==(const SurfaceBounds& sbo) const override;

  // Needed to prevent ambiguities with c++20.
  bool operator==(const CylinderBounds& bo) const;

  /**Virtual constructor */
  virtual CylinderBounds* clone() const override;

  /** Return the bounds type */
  virtual BoundsType type() const override final { return SurfaceBounds::Cylinder; }

  /**This method checks if a LocalPosition is inside z bounds and rphi value- interface method */
  virtual bool inside(const Amg::Vector2D& locpo, double tol1, double tol2) const override final;
  virtual bool inside(const Amg::Vector2D& locpo, const BoundaryCheck& bchk) const override final;

  /**This method checks if a GlobalPosition is inside the Cylinder - not an interface method,
    assumes that GlobalPosition is in the right frame*/
  bool inside3D(const Amg::Vector3D& gp, double tol1 = 0., double tol2 = 0.) const;

  /** This method checks inside bounds in loc1
    - loc1/loc2 correspond to the natural coordinates of the surface */
  virtual bool insideLoc1(const Amg::Vector2D& locpo, double tol1 = 0.) const override final;

  /** This method checks inside bounds in loc1
    - loc1/loc2 correspond to the natural coordinates of the surface */
  virtual bool insideLoc2(const Amg::Vector2D& locpo, double tol2 = 0.) const override final;

  /** Minimal distance to boundary ( > 0 if outside and <=0 if inside) */
  virtual double minDistance(const Amg::Vector2D& pos) const override final;

  /**This method checks if a LocalPosition is inside z bounds and inside the radius (for straws) */
  bool insideRadius(const Amg::Vector2D& locpo, double tol) const;

  /**This method returns the radius*/
  virtual double r() const override final;

  /**This method returns the average phi*/
  double averagePhi() const;

  /**This method returns the halfPhiSector angle*/
  double halfPhiSector() const;

  /**This method returns the halflengthZ*/
  double halflengthZ() const;

  /** Output Method for MsgStream*/
  virtual MsgStream& dump(MsgStream& sl) const override;

  /** Output Method for std::ostream */
  virtual std::ostream& dump(std::ostream& sl) const override;

private:
  /** helper methods for the inside check */
  bool inside(double r, double phi, double z, double tol1, double tol2) const;
  bool insideLocZ(double z, double tol2) const;

  /** internal storage of the geometry parameters */
  std::vector<TDD_real_t> m_boundValues;
  bool m_checkPhi;
};
}
#include "TrkSurfaces/CylinderBounds.icc"
#endif // TRKSURFACES_CYLINDERBOUNDS_H
