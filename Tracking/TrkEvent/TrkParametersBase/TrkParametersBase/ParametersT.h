/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ParametersT.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKPARAMETERSBASE_PARAMETERS_T_H
#define TRKPARAMETERSBASE_PARAMETERS_T_H

#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkEventPrimitives/SurfaceHolderImpl.h"
#include "TrkEventPrimitives/SurfaceTypes.h"
#include "TrkEventPrimitives/SurfaceUniquePtrT.h"
#include "TrkParametersBase/ParametersBase.h"
namespace Trk {
class MaterialEffectsEngine;

/**
   @class ParametersT

   The class for charged and neutral track representations containing

   - the parameters vector
   - charge
   - position
   - momentum

   Which can be returned by the parameters() method as a Amg::Vector(DIM):
   \f$
   \left(\begin{array}{c}
   x\\y\\z\\p_{x}\\p_{y}\\p_{z}\end{array}\right)
   \f$

   @tparam DIM number of track parameters (usually 5)
   @tparam T charge of track (either <tt>Trk::Charged</tt> or
   <tt>Trk::Neutral</tt>)
   @tparam S type of surface

   @author edward.moyse@cern.ch, andreas.salzburger@cern.ch
   @author Christos Anastopoulos (Athena MT modifications)
*/
template<int DIM, class T, class S>
class ParametersT final
  : public ParametersBase<DIM, T>
  , public SurfaceUniqHolderImpl<S>
{
public:
  static_assert(
    (S::staticType == SurfaceType::Cone ||
     S::staticType == SurfaceType::Cylinder ||
     S::staticType == SurfaceType::Disc ||
     S::staticType == SurfaceType::Perigee ||
     S::staticType == SurfaceType::Plane || S::staticType == SurfaceType::Line),
    "The surface type must be one of Cone, Cylinder, Disc, Perigee, Plane, "
    "Line");

  /**
   * default constructor ONLY for POOL
   */
  ParametersT() = default;

  /** Constructor with local arguments - uses global <-> local for parameters */
  ParametersT(double loc1,
              double loc2,
              double phi,
              double theta,
              double qop,
              const S& surface,
              std::optional<AmgSymMatrix(DIM)> covariance = std::nullopt);

  /** Constructor with parameters - extract position and momentum */
  ParametersT(const AmgVector(DIM) & parameters,
              const S& surface,
              std::optional<AmgSymMatrix(DIM)> covariance = std::nullopt);

  /** Constructor with global arguments - uses global <-> local for parameters
   */
  ParametersT(const Amg::Vector3D& position,
              const Amg::Vector3D& momentum,
              double charge,
              const S& surface,
              std::optional<AmgSymMatrix(DIM)> covariance = std::nullopt);

  /** Constructor with mixed arguments 1 - uses global <-> local for parameters
   */
  ParametersT(const Amg::Vector3D& position,
              double phi,
              double theta,
              double qop,
              const S& surface,
              std::optional<AmgSymMatrix(DIM)> covariance = std::nullopt);

  /** Copy constructor */
  ParametersT(const ParametersT<DIM, T, S>& rhs);

  /** Move constructor */
  ParametersT(ParametersT<DIM, T, S>&& rhs) noexcept = default;

  /** Assignment operator */
  ParametersT<DIM, T, S>& operator=(const ParametersT<DIM, T, S>& rhs);

  /** Move assignment operator */
  ParametersT<DIM, T, S>& operator=(ParametersT<DIM, T, S>&& rhs) noexcept =
    default;

  //** Destructor */
  virtual ~ParametersT() = default;

  /** Returns the charge */
  virtual double charge() const override final;

  /** Access method for the position */
  virtual Amg::Vector3D position() const override final;

  /** Access method for the momentum */
  virtual Amg::Vector3D momentum() const override final;

  /** Test to see if there's a surface there. */
  virtual bool hasSurface() const override final;

  /** Access to the Surface method */
  virtual const S& associatedSurface() const override final;

  /** equality operator */
  virtual bool operator==(
    const ParametersBase<DIM, T>& rhs) const override final;

  // Needed to avoid ambiguities with c++20.
  bool operator==(const ParametersT &rhs) const;

  /** Virtual clone */
  virtual ParametersT<DIM, T, S>* clone() const override final;

  /** Virtual clone returning unique_ptr*/
  std::unique_ptr<ParametersT<DIM, T, S>> uniqueClone() const;

  /** Return the ParametersType enum */
  virtual ParametersType type() const override final;

  /** Return the Surface Type enum */
  virtual SurfaceType surfaceType() const override final;

  /** Return the measurementFrame of the parameters */
  virtual Amg::RotationMatrix3D measurementFrame() const override final;

private:
  /* Helper to factor in update of parameters*/
  virtual void updateParametersHelper(const AmgVector(DIM) &) override final;

protected:
  /*
   * Add dependent names into scope
   */
  using ParametersBase<DIM, T>::m_parameters;
  using ParametersBase<DIM, T>::m_covariance;
  using ParametersBase<DIM, T>::m_chargeDef;
  using SurfaceUniqHolderImpl<S>::m_associatedSurface;
  Amg::Vector3D m_position; //!< point on track
  Amg::Vector3D m_momentum; //!< momentum at this point on track

  /**
   * @brief Constructor for persistency
   */
  ParametersT(const AmgVector(DIM) & parameters,
              const S* surface,
              std::optional<AmgSymMatrix(DIM)> covariance = std::nullopt);
  /*
   * friends needed for Persistency
   */
  template<typename pars>
  friend class ::TrackParametersCovarianceCnv;
  friend class ::TrackParametersCnv_p2;
  friend class ::MeasuredPerigeeCnv_p1;
  template<class SURFACE_CNV, class ATA_SURFACE>
  friend class ::AtaSurfaceCnv_p1;

  /** DESIGN TO BE REVISITED */
  friend class MaterialEffectsEngine;
};
} // end of namespace Trk

#include "TrkParametersBase/ParametersT.icc"
#endif // TRKTRACKPARAMETERS_H

