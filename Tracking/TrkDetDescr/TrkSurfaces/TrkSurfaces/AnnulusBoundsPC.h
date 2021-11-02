/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

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

/// @brief Class that implements the asymmetric shape of the ITk strip
///        endcap modules.
class AnnulusBoundsPC final : public SurfaceBounds {

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

  /// @brief Default constructor from parameters
  /// @param minR inner radius, in module system
  /// @param maxR outer radius, in module system
  /// @param phiMin right angular edge, in strip system
  /// @param phiMax left angular edge, in strip system
  /// @param moduleOrigin The origin offset between the two systems. 
  /// @param phiAvg (Optional) internal rotation of this bounds object's local frame
  /// @note For @c moduleOrigin you need to actually calculate the cartesian offset
  AnnulusBoundsPC(double minR,
                  double maxR,
                  double phiMin,
                  double phiMax,
                  Amg::Vector2D moduleOrigin = {0, 0},
                  double phiAvg = 0);

  /**Copy constructor*/
  AnnulusBoundsPC(const AnnulusBoundsPC& annbo) = default;
  /**Assignment operator*/
  AnnulusBoundsPC& operator=(const AnnulusBoundsPC& sbo) = default;
  /** Move constructor */
  AnnulusBoundsPC(AnnulusBoundsPC&& annbo) = default;
  /** Move assignment */
  AnnulusBoundsPC& operator=(AnnulusBoundsPC&& sbo) = default;



  /// @brief Static factory method to produce an instance of this class from the 
  ///        cartesian implementation
  /// @param annbo A reference to the original cartesian bounds object
  /// @return pair containing the PC bounds, and an angular shift @c phiShift
  ///
  /// @note The local frame of the cartesian implementation is such that the
  ///       module sits around the y-axis. This implementation sits around
  ///       the x axis. The local frame needs to be rotated to account for this.
  ///       @c phiShift contains necessary information to (re)construct a transform
  ///       that will perform this rotation.
  static
  std::pair<AnnulusBoundsPC, double> fromCartesian(AnnulusBounds& annbo);

  virtual AnnulusBoundsPC* clone() const override;
  bool operator==(const SurfaceBounds& sb) const override;

  SurfaceBounds::BoundsType type() const override;

  /// @brief Returns if a point in local coordinates is inside the bounds
  /// @param locpo Local position
  /// @param tol1 Tolerance in r
  /// @param tol2 Tolerance in phi
  /// @return true if is inside, false if not
  bool inside(const Amg::Vector2D& locpo, double tol1 = 0., double tol2 = 0.) const override final;
  
  /// @brief Returns if a point in local coordinates is inside the bounds
  /// @param locpo Local position
  /// @param bchk The boundary check object to consult for inside checks
  /// @return true if is inside, false if not
  bool inside(const Amg::Vector2D& locpo, const BoundaryCheck& bchk) const override final;

  /// @brief Check if local point is inside of r bounds
  /// @param locpo Local position
  /// @param tol1 Tolerance in r
  /// @return true if is inside, false if not
  bool insideLoc1(const Amg::Vector2D& locpo, double tol1 = 0.) const override final;
  
  /// @brief Check if local point is inside of phi bounds
  /// @param locpo Local position
  /// @param tol2 Tolerance in phi
  /// @return true if is inside, false if not
  bool insideLoc2(const Amg::Vector2D& locpo, double tol2 = 0.) const override final;

  /// @brief Return minimum distance a point is away from the bounds
  /// @param locpo Local position
  /// @note Even though @c locpo is considered in STRIP system, the distance
  ///       will be calculated with cartesian metric.
  double minDistance(const Amg::Vector2D& locpo) const override final;

  /// @brief Returns middle radius
  /// @return The middle radius
  double r() const override;

  /// @brief Returns inner radial bounds (module system)
  /// @return The inner radius
  double rMin() const { return m_minR; }

  /// @brief Returns outer radial bounds (module system)
  /// @return The outer radius
  double rMax() const { return m_maxR; }

  /// @brief Returns the right angular edge of the module
  /// @return The right side angle
  double phiMin() const { return m_phiMin + m_phiAvg; }

  /// @brief Returns the left angular edge of the module
  /// @return The left side angle
  double phiMax() const { return m_phiMax + m_phiAvg; }

  /// @brief Returns moduleOrigin, but rotated out, so @c avgPhi is already considered.
  /// The module origin needs to consider the rotation introduced by @c avgPhi
  /// @return The origin of the local frame
  Amg::Vector2D moduleOrigin() const;

  /// @brief helper which dumps the configuration into a stream.
  /// @param sl The stream
  /// @return the stream given in @c sl
  MsgStream& dump(MsgStream& sl) const override;
  std::ostream& dump(std::ostream& sl) const override;
      
  /**
   * @brief Returns the four corners of the bounds
   * 
   * Returns the module corners starting from the upper right (max R, pos locPhi) and proceding clock-wise, 
   * i.e. (max R; pos locPhi), (min R; pos locPhi), (min R; neg loc X), (max R; neg locPhi).
   * 
   * This method is only intended for debug purposes. If used for production code, this should be changed to a
   * return-by-reference. This will necessitate the vector being stored in the class.
   * 
   * @return array of pairs of doubles giving the (R, phi) of each corner clockwise from upper-right
   * */
  std::array<std::pair<double, double>, 4> corners() const;

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
  static Amg::Vector2D
  closestOnSegment(const Amg::Vector2D& a,
                   const Amg::Vector2D& b,
                   const Amg::Vector2D& p,
                   const Eigen::Matrix<double, 2, 2>& weight) ;
  static double
  squaredNorm(const Amg::Vector2D& v, const Eigen::Matrix<double, 2, 2>& weight) ;
};

} // End of Trk Namespace

#include "TrkSurfaces/AnnulusBoundsPC.icc"

#endif
