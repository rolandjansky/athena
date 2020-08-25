/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ParametersBase.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKPARAMETERSBASE_PARAMETERSBASE_H
#define TRKPARAMETERSBASE_PARAMETERSBASE_H

// Amg
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkParametersBase/Charged.h"
#include "TrkParametersBase/Neutral.h"
#include <memory>
#include <type_traits>

class MsgStream;

template<typename T>
class TrackParametersCovarianceCnv;
class TrackParametersCnv_p2;
class MeasuredPerigeeCnv_p1;
template<class SURFACE_CNV, class ATA_SURFACE>
class AtaSurfaceCnv_p1;

namespace Trk {
class Surface;
class MaterialEffectsEngine;

/**
   @enum ParametersType
   Enum to avoid dynamic cast for different parameter types.
*/
enum ParametersType
{
  AtaSurface = 0,
  Curvilinear = 1
};

namespace InvalidParam {
constexpr double INVALID = std::numeric_limits<double>::quiet_NaN();
constexpr double INVALID_P(10e9);
constexpr double INVALID_QOP(10e-9);
}
/**
   @class ParametersBase

   The base class for neutral and charged Track parameters.
   It represents the free state of a trajectory, represented by
   the track parameters.
   The position and the momentum are both given in the tracking
   reference frame.

   @tparam DIM number of track parameters (usually 5)
   @tparam T   charge of track (either <tt>Trk::Charged</tt> or
   <tt>Trk::Neutral</tt>)

   The relevant allowed aliases and specialization are under
   TrkParameters and TrkNeutralParameters and not in this package.

   @author Andreas.Salzburger@cern.ch
   @author Christos Anastopoulos (Athena MT modifications)
*/

template<int DIM, class T>
class ParametersBase
{
public:
  static_assert((std::is_same<T, Trk::Charged>::value ||
                 std::is_same<T, Trk::Neutral>::value),
                "Parameters must be Charged or Neutral");
  static constexpr int dim = DIM;
  /** virtual Destructor */
  virtual ~ParametersBase() = default;

  /** Access method for the parameters */
  const AmgVector(DIM) & parameters() const;

  /** Access method for the covariance matrix - returns nullptr if no covariance
   * matrix is given */
  const AmgSymMatrix(DIM) * covariance() const;

  /** Access method for the position */
  const Amg::Vector3D& position() const;

  /** Access method for the momentum */
  const Amg::Vector3D& momentum() const;

  /** Access method for transverse momentum */
  double pT() const;

  /** Access method for pseudorapidity - from momentum */
  double eta() const;

  /** Returns true if Charged or false if Neutral
   */
  constexpr bool isCharged() const;
  /** Returns the charge
   * */
  double charge() const;

  /** Access method for the local coordinates, \f$(loc1,loc2)\f$
      local parameter definitions differ for each surface type. */
  Amg::Vector2D localPosition() const;

  /** Update parameters and covariance.
   * 
   * Derived classes override the
   * implementation via updateParametersHelper
   */
  void updateParameters(const AmgVector(DIM) &, AmgSymMatrix(DIM) * = nullptr);

  /** Update parameters  and covariance , passing covariance by ref. A
   * covariance is created if one does not exist.  Otherwise in place update
   * occurs via assignment.
   *
   * Derived classes override the
   * implementation via updateParametersHelper
   */
  void updateParameters(const AmgVector(DIM) &, const AmgSymMatrix(DIM) &);

  //** equality operator */
  virtual bool operator==(const ParametersBase<DIM, T>&) const;

  /** Test to see if there's a not null surface ptr. */
  virtual bool hasSurface() const = 0;

  /** Access to the Surface associated to the Parameters*/
  virtual const Surface& associatedSurface() const = 0;

  /** Return the measurement frame - this is needed for alignment, in
     particular for StraightLine and Perigee Surface
      - the default implementation is the RotationMatrix3D of the
     transform */
  virtual Amg::RotationMatrix3D measurementFrame() const = 0;

  /** clone method for polymorphic deep copy 
       @return new object copied from the concrete type of this object.*/
  virtual ParametersBase<DIM, T>* clone() const = 0;

  /** Return the ParametersType enum */
  virtual ParametersType type() const = 0;

  /** Returns the Surface Type enum for the surface used
   * to define the derived class*/
  virtual int surfaceType() const = 0;

  /** Dumps relevant information about the track parameters into the ostream */
  virtual MsgStream& dump(MsgStream& out) const;
  virtual std::ostream& dump(std::ostream& out) const;

protected:
  /*
   * This is an abstract class and we can not instanticate objects directly.
   * In the other hand derived classed can use ctors
   */

  ParametersBase() = default;

  /* Helper ctors for derived classes*/
  ParametersBase(const AmgVector(DIM) parameters,
                 AmgSymMatrix(DIM) * covariance,
                 const Amg::Vector3D& position,
                 const Amg::Vector3D& momentum,
                 const T chargeDef);

  ParametersBase(const Amg::Vector3D& pos,
                 const Amg::Vector3D& mom,
                 AmgSymMatrix(DIM) * covariance = nullptr);

  ParametersBase(const AmgVector(DIM) & parameters,
                 AmgSymMatrix(DIM) * covariance = nullptr);

  /*
   * Default Move ctor/assignment, private can be used
   * only by derived classes.
   */
  ParametersBase(ParametersBase&&) = default;
  ParametersBase& operator=(ParametersBase&&) = default;

  /*
   * Default copy ctor/assignment
   * Deleted due unique_ptr.
   *
   * Derived classes can implement them explicitly.
   * Polymorphic deep copy can happen via the clone
   * method
   */
  ParametersBase(const ParametersBase&) = delete;
  ParametersBase& operator=(const ParametersBase&) = delete;

  /* Helper implementing the specific per derived class logic for
   * the update of parameters*/
  virtual void updateParametersHelper(const AmgVector(DIM) &) = 0;

  AmgVector(DIM) m_parameters; //!< contains the n parameters
  //!< contains the n x n covariance matrix
  std::unique_ptr<AmgSymMatrix(DIM)> m_covariance;
  Amg::Vector3D m_position; //!< point on track
  Amg::Vector3D m_momentum; //!< momentum at this point on track
  T m_chargeDef;            //!< charge definition for this track
};

/**Overload of << operator for both, MsgStream and std::ostream for debug
 * output*/
template<int DIM, class T>
MsgStream&
operator<<(MsgStream& sl, const Trk::ParametersBase<DIM, T>& tp);

template<int DIM, class T>
std::ostream&
operator<<(std::ostream& sl, const Trk::ParametersBase<DIM, T>& tp);

} // end of namespace Trk

#include "TrkParametersBase/ParametersBase.icc"

#endif
