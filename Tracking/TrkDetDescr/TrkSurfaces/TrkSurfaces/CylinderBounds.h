/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

class CylinderBounds : public SurfaceBounds
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

  /**Virtual constructor */
  virtual CylinderBounds* clone() const override;

  /** Return the bounds type */
  virtual BoundsType type() const override { return SurfaceBounds::Cylinder; }

  /**This method checks if a LocalPosition is inside z bounds and rphi value- interface method */
  virtual bool inside(const Amg::Vector2D& locpo, double tol1, double tol2) const override;
  virtual bool inside(const Amg::Vector2D& locpo, const BoundaryCheck& bchk) const override;

  /**This method checks if a GlobalPosition is inside the Cylinder - not an interface method,
    assumes that GlobalPosition is in the right frame*/
  bool inside3D(const Amg::Vector3D& gp, double tol1 = 0., double tol2 = 0.) const;

  /** This method checks inside bounds in loc1
    - loc1/loc2 correspond to the natural coordinates of the surface */
  virtual bool insideLoc1(const Amg::Vector2D& locpo, double tol1 = 0.) const override;

  /** This method checks inside bounds in loc1
    - loc1/loc2 correspond to the natural coordinates of the surface */
  virtual bool insideLoc2(const Amg::Vector2D& locpo, double tol2 = 0.) const override;

  /** Minimal distance to boundary ( > 0 if outside and <=0 if inside) */
  virtual double minDistance(const Amg::Vector2D& pos) const override;

  /**This method checks if a LocalPosition is inside z bounds and inside the radius (for straws) */
  bool insideRadius(const Amg::Vector2D& locpo, double tol) const;

  /**This method returns the radius*/
  virtual double r() const override;

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

inline CylinderBounds*
CylinderBounds::clone() const
{
  return new CylinderBounds(*this);
}

inline bool
CylinderBounds::inside(const Amg::Vector2D& locpo, double tol1, double tol2) const
{
  double z = locpo[locZ];
  bool insideZ = insideLocZ(z, tol2);
  if (!insideZ)
    return false;
  // no check on Phi neccesary
  if (!m_checkPhi)
    return true;
  // now check insidePhi
  double localPhi =
    (locpo[locRPhi] / m_boundValues[CylinderBounds::bv_radius]) - m_boundValues[CylinderBounds::bv_averagePhi];
  localPhi -= (localPhi > M_PI) ? 2. * M_PI : 0.;
  return (localPhi * localPhi < (m_boundValues[CylinderBounds::bv_halfPhiSector] + tol1) *
                                  (m_boundValues[CylinderBounds::bv_halfPhiSector] + tol1));
}

inline bool
CylinderBounds::inside(const Amg::Vector2D& locpo, const BoundaryCheck& bchk) const
{
  if (bchk.bcType == 0 || bchk.nSigmas == 0 || m_boundValues[CylinderBounds::bv_halfPhiSector] != M_PI)
    return CylinderBounds::inside(locpo, bchk.toleranceLoc1, bchk.toleranceLoc2);

  float theta = (bchk.lCovariance(1, 0) != 0 && (bchk.lCovariance(1, 1) - bchk.lCovariance(0, 0)) != 0)
                  ? .5 * bchk.FastArcTan(2 * bchk.lCovariance(1, 0) / (bchk.lCovariance(1, 1) - bchk.lCovariance(0, 0)))
                  : 0.;
  sincosCache scResult = bchk.FastSinCos(theta);
  double dphi = scResult.sinC * scResult.sinC * bchk.lCovariance(0, 0);
  double dz = scResult.cosC * scResult.cosC * bchk.lCovariance(0, 1);
  double max_ell = dphi > dz ? dphi : dz;
  double limit = bchk.nSigmas * sqrt(max_ell);
  return insideLocZ(locpo[locZ], limit);
}

inline bool
CylinderBounds::inside3D(const Amg::Vector3D& glopo, double tol1, double tol2) const
{
  return inside(glopo.perp(), glopo.phi(), glopo.z(), tol1, tol2);
}

inline bool
CylinderBounds::inside(double r, double phi, double z, double tol1, double tol2) const
{

  bool insideZ = insideLocZ(z, tol2);
  if (!insideZ)
    return false;
  double diffR = (m_boundValues[CylinderBounds::bv_radius] - r);
  bool insideR = diffR * diffR < tol1 * tol1;
  if (!insideR)
    return false;
  // now check insidePhi if needed
  if (!m_checkPhi)
    return true;
  // phi needs to be checked
  double localPhi = phi - m_boundValues[CylinderBounds::bv_averagePhi];
  localPhi -= (localPhi > M_PI) ? 2. * M_PI : 0.;
  return (localPhi * localPhi <
          m_boundValues[CylinderBounds::bv_halfPhiSector] * m_boundValues[CylinderBounds::bv_halfPhiSector]);
}

inline bool
CylinderBounds::insideLocZ(double z, double tol2) const
{
  return (m_boundValues[CylinderBounds::bv_halfZ] + tol2) - fabs(z) > 0.;
}

inline bool
CylinderBounds::insideLoc1(const Amg::Vector2D& locpo, double tol1) const
{
  bool insideRphi = false;
  if (fabs(m_boundValues[CylinderBounds::bv_averagePhi]) < 10e-7)
    insideRphi = (fabs(locpo[locRPhi] / m_boundValues[CylinderBounds::bv_radius]) <
                  (m_boundValues[CylinderBounds::bv_halfPhiSector] + tol1));
  else {
    double localPhi =
      (locpo[locRPhi] / m_boundValues[CylinderBounds::bv_radius]) - m_boundValues[CylinderBounds::bv_averagePhi];
    localPhi -= (localPhi > M_PI) ? 2. * M_PI : 0.;
    insideRphi = (localPhi < (m_boundValues[CylinderBounds::bv_halfPhiSector] + tol1));
  }
  return (insideRphi);
}

inline bool
CylinderBounds::insideLoc2(const Amg::Vector2D& locpo, double tol2) const
{
  return insideLocZ(locpo[locZ], tol2);
}

inline bool
CylinderBounds::insideRadius(const Amg::Vector2D& locpo, double tol) const
{
  return (this->inside(locpo, tol, 0) && fabs(locpo[locR]) < m_boundValues[CylinderBounds::bv_radius] + tol);
}

inline double
CylinderBounds::r() const
{
  return m_boundValues[CylinderBounds::bv_radius];
}

inline double
CylinderBounds::averagePhi() const
{
  return m_boundValues[CylinderBounds::bv_averagePhi];
}

inline double
CylinderBounds::halfPhiSector() const
{
  return m_boundValues[CylinderBounds::bv_halfPhiSector];
}

inline double
CylinderBounds::halflengthZ() const
{
  return m_boundValues[CylinderBounds::bv_halfZ];
}

}

#endif // TRKSURFACES_CYLINDERBOUNDS_H
