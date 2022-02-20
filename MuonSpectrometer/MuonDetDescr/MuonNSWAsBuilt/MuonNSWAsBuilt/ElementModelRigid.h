/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONNSWASBUILT_ELEMENTMODELRIGID_H
#define MUONNSWASBUILT_ELEMENTMODELRIGID_H

#include "MuonNSWAsBuilt/ElementModel.h"

namespace NswAsBuilt {

  /**
  * An element model without deformations, i.e. with only a (3D) translation and a (3D) rotation
  */
  
  class ElementModelRigid : public ElementModel {
    public:
      enum parameter_t { X=0, Y=1, Z=2, THX=3, THY=4, THZ=5 };

      // The number of parameters used
      virtual int nParameters() const override { return 6; }

      // Transform a set of vectors expressed in local frame, stored in a matrix
      virtual void transform(const ParameterVector& parvec, VectorSetRef local) const override;

      // Cache the rigid component of this deformation model
      virtual void cacheTransform(ParameterVector& parvec) const override;

      // Helper methods to convert parameter index to string representation
      virtual ipar_t getParameterIndex(const std::string& parname) const override;
      virtual std::string getParameterName(ipar_t ipar) const override;
  };
}

#endif

