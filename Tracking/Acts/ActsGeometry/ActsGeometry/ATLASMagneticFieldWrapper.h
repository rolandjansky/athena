/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ATLASMAGNETICFIELDWRAPPER_H
#define ACTSGEOMETRY_ATLASMAGNETICFIELDWRAPPER_H

#include "StoreGate/ReadCondHandleKey.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "Acts/Definitions/Algebra.hpp"
#include "Acts/Definitions/Units.hpp"
#include "Acts/MagneticField/MagneticFieldContext.hpp"
#include "Acts/MagneticField/MagneticFieldProvider.hpp"

class ATLASMagneticFieldWrapper final : public Acts::MagneticFieldProvider {

public:

  struct Cache {

    Cache(const Acts::MagneticFieldContext mctx) {
      const auto* atlasField = mctx.get<const AtlasFieldCacheCondObj*>();
      atlasField->getInitializedCache(fieldCache);

    }

    MagField::AtlasFieldCache fieldCache;
  };

  ATLASMagneticFieldWrapper() = default;


  MagneticFieldProvider::Cache 
  makeCache(const Acts::MagneticFieldContext& mctx) const override {
    return Acts::MagneticFieldProvider::Cache::make<Cache>(mctx);
  }

  Acts::Result<Acts::Vector3>
  getField(const Acts::Vector3& position, Acts::MagneticFieldProvider::Cache& gcache) const override {
    Cache& cache = gcache.get<Cache>();
    double posXYZ[3];
    posXYZ[0] = position.x();
    posXYZ[1] = position.y();
    posXYZ[2] = position.z();
    double BField[3];

    cache.fieldCache.getField(posXYZ, BField);

    // Magnetic field
    Acts::Vector3 bfield{BField[0],BField[1],BField[2]};

    bfield *= m_bFieldUnit; // kT -> T;

    return Acts::Result<Acts::Vector3>::success(bfield);
  }

  Acts::Result<Acts::Vector3>
  getFieldGradient(const Acts::Vector3& position,
                   Acts::ActsMatrix<3, 3>& gradient,
                   Acts::MagneticFieldProvider::Cache& gcache) const override
  {
    Cache& cache = gcache.get<Cache>();
    double posXYZ[3];
    posXYZ[0] = position.x();
    posXYZ[1] = position.y();
    posXYZ[2] = position.z();
    double BField[3];
    double grad[9];

    cache.fieldCache.getField(posXYZ, BField, grad);

    // Magnetic field
    Acts::Vector3 bfield{BField[0], BField[1],BField[2]};
    Acts::ActsMatrix<3, 3> tempGrad;
    tempGrad << grad[0], grad[1], grad[2], grad[3], grad[4], grad[5], grad[6], grad[7], grad[8]; 
    gradient = tempGrad;


    bfield *= m_bFieldUnit; // kT -> T;
    gradient *= m_bFieldUnit;

    return Acts::Result<Acts::Vector3>::success(bfield);
  }

private:
  const double m_bFieldUnit = 1000.*Acts::UnitConstants::T;
};


#endif
