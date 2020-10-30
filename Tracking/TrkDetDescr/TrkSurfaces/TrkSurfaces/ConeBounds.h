/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ConeBounds.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSURFACES_ConeBounds_H
#define TRKSURFACES_ConeBounds_H

#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkSurfaces/SurfaceBounds.h"

#include <cfloat>

#include "GeoPrimitives/GeoPrimitives.h"

#ifdef TRKDETDESCR_USEFLOATPRECISON
typedef float TDD_real_t;
const float MAXBOUNDVALUE = FLT_MAX;
#else
typedef double TDD_real_t;
const double MAXBOUNDVALUE = DBL_MAX;
#endif

class MsgStream;

namespace Trk {

/**
    @class ConeBounds

    Bounds for a conical Surface,
    the opening angle is stored in \f$ \tan(\alpha) \f$ and always positively defined.
    The cone can open to both sides, steered by \f$ z_min \f$ and \f$ z_max \f$.

    @image html ConeBounds.gif

    @author Ian.Watson@cern.ch, Andreas.Salzburger@cern.ch, Robert.Langenberg@cern.ch
*/

class ConeBounds : public SurfaceBounds
{
public:
  /** BoundValues for readablility */
  enum BoundValues
  {
    bv_alpha = 0,
    bv_minZ = 1,
    bv_maxZ = 2,
    bv_averagePhi = 3,
    bv_halfPhiSector = 4,
    bv_length = 5
  };

  /**Default Constructor*/
  ConeBounds();

  /**Constructor - open cone with  alpha, by default a full cone
     but optionally can make a conical section */
  ConeBounds(double alpha, bool symm, double halfphi = M_PI, double avphi = 0.);

  /**Constructor - open cone with alpha, minz and maxz, by
     default a full cone but can optionally make it a conical section */
  ConeBounds(double alpha, double zmin, double zmax, double halfphi = M_PI, double avphi = 0.);

  /**Copy Constructor */
  ConeBounds(const ConeBounds& cylbo) = default;

  /**Assignment operator*/
  ConeBounds& operator=(const ConeBounds& cylbo) = default;

  /**Destructor */
  virtual ~ConeBounds() = default;

  /**Equality operator*/
  virtual bool operator==(const SurfaceBounds& sbo) const override;

  /**Virtual constructor */
  virtual ConeBounds* clone() const override;

  /** Return the bounds type */
  virtual BoundsType type() const override { return SurfaceBounds::Cone; }

  /**This method checks if a LocalPosition is inside z bounds and rphi value- interface method */
  virtual bool inside(const Amg::Vector2D& locpo, double tol1, double tol2) const override;
  virtual bool inside(const Amg::Vector2D& locpo, const BoundaryCheck& bchk = true) const override;

  /**This method checks if a GlobalPosition is inside the Cylinder - not an interface method,
        assumes that GlobalPosition is in the right frame*/
  virtual bool inside(const Amg::Vector3D& gp, double tol1 = 0., double tol2 = 0.) const;
  virtual bool inside(const Amg::Vector3D& locpo, const BoundaryCheck& bchk) const;

  /** This method checks inside bounds in loc1
      - loc1/loc2 correspond to the natural coordinates of the surface */
  virtual bool insideLoc1(const Amg::Vector2D& locpo, double tol1 = 0.) const override;

  /** This method checks inside bounds in loc1
      - loc1/loc2 correspond to the natural coordinates of the surface */
  virtual bool insideLoc2(const Amg::Vector2D& locpo, double tol2 = 0.) const override;

  /** Minimal distance to boundary ( > 0 if outside and <=0 if inside) */
  virtual double minDistance(const Amg::Vector2D& pos) const override;

  /**This method returns the maximal radius - for an unbound cone it returns MAXBOUNDVALUE*/
  virtual double r() const override;

  /** Return the radius at a specific z values */
  double r(double z) const;

  /**This method returns the average phi*/
  double tanAlpha() const;
  double sinAlpha() const;
  double cosAlpha() const;
  double alpha() const;

  /**This method returns the minimum z value in the local framee
     - for an unbound symmetric cone, it returns -MAXBOUNDVALUE*/
  double minZ() const;

  /**This method returns the maximum z value in the local framee
     - for an unbound cone, it returns MAXBOUNDVALUE*/
  double maxZ() const;

  /**This method returns the average phi value (i.e. the "middle"
     phi value for the conical sector we  are describing) */

  double averagePhi() const;
  /**This method returns the half-phi width of the sector (so that
     averagePhi +/- halfPhiSector gives the phi bounds of the
     cone) */
  double halfPhiSector() const;

  /** Output Method for MsgStream*/
  virtual MsgStream& dump(MsgStream& sl) const override;
  /** Output Method for std::ostream */
  virtual std::ostream& dump(std::ostream& sl) const override;

private:
  /** internal storage of the geometry parameters */
  std::vector<TDD_real_t> m_boundValues;
  TDD_real_t m_tanAlpha;
  TDD_real_t m_sinAlpha;
  TDD_real_t m_cosAlpha;

  /** Helper function for angle parameter initialization */
  virtual void initCache() override;

  /** Helpers for inside() functions */
  inline double minPhi() const
  {
    return m_boundValues[ConeBounds::bv_averagePhi] - m_boundValues[ConeBounds::bv_halfPhiSector];
  }
  inline double maxPhi() const
  {
    return m_boundValues[ConeBounds::bv_averagePhi] + m_boundValues[ConeBounds::bv_halfPhiSector];
  }
};

}

#include "TrkSurfaces/ConeBounds.icc"
#endif // TRKSURFACES_CONEBOUNDS_H
