#ifndef TRKSURFACES_ANNULUSBOUNDSPC_H
#define TRKSURFACES_ANNULUSBOUNDSPC_H

#include "TrkSurfaces/SurfaceBounds.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include <iosfwd>

class BoundaryCheck;
class MsgStream;

#ifdef TRKDETDESCR_USEFLOATPRECISON
typedef float TDD_real_t;
#else
typedef double TDD_real_t;
#endif

class AnnulusTestAlg;

namespace Trk {

class AnnulusBounds;

class AnnulusBoundsPC : public SurfaceBounds {

  using Rotation2D = Eigen::Rotation2D<double>;
  using Transform2D = Eigen::Transform<double, 2, Eigen::Affine>;
  using Matrix2D = Eigen::Matrix<double, 2, 2>;

  friend AnnulusTestAlg;

public:

  enum BoundValues {
    bv_minR,
    bv_maxR,
    bv_phiMin,
    bv_phiMax,
    bv_phiAvg,
    bv_originX,
    bv_originY
  };

  AnnulusBoundsPC(double minR,
                  double maxR,
                  double phiMin,
                  double phiMax,
                  Amg::Vector2D moduleOrigin = {0, 0},
                  double phiAvg = 0);


  static
  std::pair<AnnulusBoundsPC, double>
  fromCartesian(const AnnulusBounds& annbo);

  AnnulusBoundsPC* clone() const override;
  bool operator==(const SurfaceBounds& sb) const override;

  SurfaceBounds::BoundsType type() const override;

  bool inside(const Amg::Vector2D& locpo, double tol1 = 0., double tol2 = 0.) const override;
  bool inside(const Amg::Vector2D& locpo, const BoundaryCheck& bchk) const override;

  bool insideLoc1(const Amg::Vector2D& locpo, double tol1 = 0.) const override;
  bool insideLoc2(const Amg::Vector2D& locpo, double tol2 = 0.) const override;

  double minDistance(const Amg::Vector2D& locpo) const override;
  double r() const override;

  // returns moduleOrigin, but rotated out, so avgPhi is already considered.
  Amg::Vector2D moduleOrigin() const;

  MsgStream& dump(MsgStream& sl) const override;
  std::ostream& dump(std::ostream& sl) const override;
      
  std::array<Amg::Vector2D, 4>
  cornerPoints() const {
    auto rot = m_rotationStripPC.inverse();

    return {
      rot * m_inLeftStripPC,
      rot * m_outLeftStripPC,
      rot * m_outRightStripPC,
      rot * m_inRightStripPC,
    };

  }

private:
  double m_minR;
  double m_maxR;
  double m_phiMin;
  double m_phiMax;
  // @TODO: Does this need to be in bound values?
  Amg::Vector2D m_moduleOrigin;
  Amg::Vector2D m_shiftXY; // == -m_moduleOrigin
  Amg::Vector2D m_shiftPC;
  double m_phiAvg;
  Transform2D m_rotationStripPC;
  Transform2D m_translation;

  // vectors needed for inside checking
  Amg::Vector2D m_outLeftStripPC;
  Amg::Vector2D m_inLeftStripPC;
  Amg::Vector2D m_outRightStripPC;
  Amg::Vector2D m_inRightStripPC;

  Amg::Vector2D m_outLeftModulePC;
  Amg::Vector2D m_inLeftModulePC;
  Amg::Vector2D m_outRightModulePC;
  Amg::Vector2D m_inRightModulePC;

  Amg::Vector2D m_outLeftStripXY;
  Amg::Vector2D m_inLeftStripXY;
  Amg::Vector2D m_outRightStripXY;
  Amg::Vector2D m_inRightStripXY;

  std::vector<TDD_real_t>   m_boundValues;

  Amg::Vector2D stripXYToModulePC(const Amg::Vector2D& vStripXY) const;
  Amg::Vector2D
  closestOnSegment(const Amg::Vector2D& a,
                   const Amg::Vector2D& b,
                   const Amg::Vector2D& p,
                   const Eigen::Matrix<double, 2, 2>& weight) const;
  double
  squaredNorm(const Amg::Vector2D& v, const Eigen::Matrix<double, 2, 2>& weight) const;
};




}

inline
Trk::AnnulusBoundsPC*
Trk::AnnulusBoundsPC::clone() const
{
  return new Trk::AnnulusBoundsPC(m_minR, m_maxR, m_phiMin, m_phiMax, m_moduleOrigin, m_phiAvg);
}

inline
bool Trk::AnnulusBoundsPC::operator==(const SurfaceBounds& sb) const
{
  if (sb.type() != SurfaceBounds::Annulus) return false;

  const AnnulusBoundsPC* ab = dynamic_cast<const AnnulusBoundsPC*>(&sb);
  if (ab == nullptr) return false; // is AnnulusBounds, but not PC

  if (m_boundValues.size() != ab->m_boundValues.size()) return false;

  for (size_t i=0;i<m_boundValues.size();i++) {
    if (m_boundValues[i] != ab->m_boundValues[i]) return false;
  }

  return true;
}

inline
Trk::SurfaceBounds::BoundsType Trk::AnnulusBoundsPC::type() const
{
  return Trk::SurfaceBounds::Annulus;
}

#endif
