/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONNSWASBUILT_ELEMENTMODELSTGC_H
#define MUONNSWASBUILT_ELEMENTMODELSTGC_H

#include "MuonNSWAsBuilt/ElementModel.h"

namespace NswAsBuilt {
  /**
   * An element model implementing the deformations used for the NSW sTGC
   * as-built parameters.
   *
   * The first 6 parameters represent the rigid component of the transform (a 3D
   * translation and 3 Euler angles to be applied in the order ZYX)
   *
   * The following parameters are providing deformations, in accordance with the:
   * 4-parameter alingment model implemented for the sTGC quadruplets:
   *
   *    dy = off + rot*x + scl*y + npar*x*y
   *
   * For more information check https://cds.cern.ch/record/2688394?ln=en
   * 
   */
  class ElementModelSTGC : public ElementModel {
    public:
      enum parameter_t {
        X=0,
        Y=1,
        Z=2,
        THX=3,
        THY=4,
        THZ=5,
        ROT=6,
        OFF=7,
        SCL=8,
        NPAR=9
      };

      ElementModelSTGC(double lenX, double lenY, Amg::Vector3D defo0);
      ElementModelSTGC() = delete;

      /**
       * The number of parameters used
       */
      virtual int nParameters() const override { return 10; }

      /**
       * Transform a set of vectors expressed in local frame, stored in a matrix
       */
      virtual void transform(const ParameterVector& parvec, VectorSetRef local) const override;

      /**
       * Cache the rigid component of this deformation model
       */
      virtual void cacheTransform(ParameterVector& parvec) const override;

      /*
       * Helper methods to convert parameter index to string representation
       */
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

      Amg::Vector3D stgcScale(double scl, Amg::Vector3D d0) const;
      Amg::Vector3D stgcNonPar(double npar, Amg::Vector3D d0) const;

      // Properties and scales needed for the calculation
      double m_lenX = 0.0;
      double m_lenY = 0.0;
      Amg::Vector3D m_defo0 = Amg::Vector3D::Zero();
  };
}

#endif
