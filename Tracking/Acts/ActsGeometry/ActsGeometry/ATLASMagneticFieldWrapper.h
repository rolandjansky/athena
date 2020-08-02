/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ATLASMAGNETICFIELDWRAPPER_H
#define ACTSGEOMETRY_ATLASMAGNETICFIELDWRAPPER_H

#include "StoreGate/ReadCondHandleKey.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/Units.hpp"
#include "Acts/MagneticField/MagneticFieldContext.hpp"

class ATLASMagneticFieldWrapper
{


public:

  struct Cache {

    Cache(std::reference_wrapper<const Acts::MagneticFieldContext> mctx) {
      std::any_cast<const AtlasFieldCacheCondObj*>(mctx)->getInitializedCache(fieldCache);
    }

    MagField::AtlasFieldCache fieldCache;
  };

  ATLASMagneticFieldWrapper() = default;

  Acts::Vector3D
  getField(const Acts::Vector3D& position, Cache& cache) const
  {
    double posXYZ[3];
    posXYZ[0] = position.x();
    posXYZ[1] = position.y();
    posXYZ[2] = position.z();
    double BField[3];

    cache.fieldCache.getField(posXYZ, BField);

    // Magnetic field
    Acts::Vector3D bfield{BField[0],BField[1],BField[2]};

    bfield *= m_bFieldUnit; // kT -> T;

    return bfield;
  }

  Acts::Vector3D
  getFieldGradient(const Acts::Vector3D& position,
                   Acts::ActsMatrixD<3, 3>& gradient,
                   Cache& cache) const
  {
    double posXYZ[3];
    posXYZ[0] = position.x();
    posXYZ[1] = position.y();
    posXYZ[2] = position.z();
    double BField[3];
    double grad[9];

    cache.fieldCache.getField(posXYZ, BField, grad);

    // Magnetic field
    Acts::Vector3D bfield{BField[0], BField[1],BField[2]};
    Acts::ActsMatrixD<3, 3> tempGrad;
    tempGrad << grad[0], grad[1], grad[2], grad[3], grad[4], grad[5], grad[6], grad[7], grad[8]; 
    gradient = tempGrad;


    bfield *= m_bFieldUnit; // kT -> T;
    gradient *= m_bFieldUnit;

    return bfield;
  }

private:
  const double m_bFieldUnit = 1000.*Acts::UnitConstants::T;
};


#endif
