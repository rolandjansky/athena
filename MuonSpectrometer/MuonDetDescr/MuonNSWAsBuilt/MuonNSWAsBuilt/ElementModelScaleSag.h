/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONNSWASBUILT_ELEMENTMODELSCALESAG_H
#define MUONNSWASBUILT_ELEMENTMODELSCALESAG_H

#include "MuonNSWAsBuilt/ElementModel.h"

namespace NswAsBuilt {
  /**
  * An element model implementing the deformations used for the NSW MM
  * as-built parameters.
  *
  * The first 6 parameters represent the rigid component of the transform (a 3D
  * translation and 3 Euler angles to be applied in the order ZYX)
  *
  * The following parameters are providing deformations, as follows:
  *
  * - EGX, EGY, EGZ: elongations, expressed in permil units
  * - SAGX and SAGY: bending along the X and Y directions, expressed in
  *   millimeter. SAGY(X) leaves unchanged the points at X(Y) = +- 0.5*lenX(Y),
  *   and they move the center point by SAGY(X), respectively
  * - DEGX, DEGY: gradients en elongation along X and Y. The elongation along
  *   Y(X) at points X(Y) = +- 0.5*lenX(Y) is EGY(X)+-DEGY(X), respectively.
  *   Expressed in permil units.
  * - PGX, PGY: Paralellogram (shear) transforms, expressed in millimeters.
  *   PGX(Y) is the displacement along X(Y) of point at Y(X)=0.5*lenY(X).
  *   Central point is not moved
  * - DSAGX, DSAGY: gradients of sag along X and Y. sag along Y(X) at Y(X)=0 is
  *   unchanged by this transform, and is increased by DSAGY(X) at
  *   Y(X)=0.5*lenY
  *
  */
  class ElementModelScaleSag : public ElementModel {
    public:
      enum parameter_t {
        X=0,
        Y=1,
        Z=2,
        THX=3,
        THY=4,
        THZ=5,
        EGX=6,
        EGY=7,
        EGZ=8,
        SAGX=9,
        SAGY=10,
        DEGX=11,
        DEGY=12,
        PGX=13,
        PGY=14,
        DSAGX=15,
        DSAGY=16
      };

      ElementModelScaleSag(double lenX, double lenY, Amg::Vector3D defo0);
      ElementModelScaleSag() = delete;

      // Number of parameters used
      virtual int nParameters() const override { return 17; }

      // Transform a set of vectors expressed in local frame, stored in a matrix
      virtual void transform(const ParameterVector& parvec, VectorSetRef local) const override;

      // Cache the rigid component of this deformation model
      virtual void cacheTransform(ParameterVector& parvec) const override;

      // Helper methods to convert parameter index to string representation
      virtual ipar_t getParameterIndex(const std::string& parname) const override;
      virtual std::string getParameterName(ipar_t ipar) const override;

    private:
      // The actual implementation: applyDeformation is the reference
      // implementation, which does not make use of Eigen optimizations
      // Not actually used, provided for comparison purposes
      void applyDeformation(const ParameterVector& parvec, Eigen::Ref<Amg::Vector3D> local) const;
      // The actual implementation: applyDeformation makes use of the Eigen
      // optimizations.
      void applyDeformation2(const ParameterVector& parvec, VectorSetRef local) const;

      // Basic ingredients of the calculation, called by method applyDeformation
      Amg::Vector3D DEg( double egx, double egy, double egz, const Amg::Vector3D& d0) const;
      Amg::Vector3D DSagX(double sagx, const Amg::Vector3D& d0) const;
      Amg::Vector3D DSagY(double sagy, const Amg::Vector3D& d0) const;
      Amg::Vector3D DDSagX(double dsagx, const Amg::Vector3D& d0) const;
      Amg::Vector3D DDSagY(double dsagy, const Amg::Vector3D& d0) const;
      Amg::Vector3D DDegX(double degx, const Amg::Vector3D& d0) const;
      Amg::Vector3D DDegY(double degy, const Amg::Vector3D& d0) const;
      Amg::Vector3D DPgX(double pgx, const Amg::Vector3D& d0) const;
      Amg::Vector3D DPgY(double pgy, const Amg::Vector3D& d0) const;

      // Properties and scales needed for the calculation
      double m_lenX = 0.0;
      double m_lenY = 0.0;
      Amg::Vector3D m_defo0 = Amg::Vector3D::Zero();
  };
}

#endif

