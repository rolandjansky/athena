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
    // empty, no cache for now

    Cache(std::reference_wrapper<const Acts::MagneticFieldContext> /*mctx*/) {
      // does nothing, but is required
    }
  };

  ATLASMagneticFieldWrapper(const SG::ReadCondHandleKey<AtlasFieldCacheCondObj>& fieldCacheCondObjInputKey)
    : m_fieldCacheCondObjInputKey(fieldCacheCondObjInputKey)
  {
  }

  Acts::Vector3D
  getField(const Acts::Vector3D& pos) const
  {
    MagField::AtlasFieldCache fieldCache = getFieldCache();

    double posXYZ[3];
    posXYZ[0] = pos.x();
    posXYZ[0] = pos.y();
    posXYZ[0] = pos.z();
    double BField[3];
    fieldCache.getField(posXYZ, BField);

    // Magnetic field
    Acts::Vector3D bfield{BField[0],BField[1],BField[2]};

    bfield *= m_bFieldUnit; // kT -> T;

    return bfield;
  }

  Acts::Vector3D
  getField(const Acts::Vector3D& pos, Cache& /*cache*/) const
  {
    return getField(pos);
  }

  Acts::Vector3D
  getFieldGradient(const Acts::Vector3D& position, Acts::ActsMatrixD<3, 3>& gradient) const
  {

    MagField::AtlasFieldCache fieldCache = getFieldCache();

    double posXYZ[3];
    posXYZ[0] = position.x();
    posXYZ[0] = position.y();
    posXYZ[0] = position.z();
    double BField[3];
    double grad[9];

    fieldCache.getField(posXYZ, BField, grad);

    // Magnetic field
    Acts::Vector3D bfield{BField[0], BField[1],BField[2]};
    Acts::ActsMatrixD<3, 3> tempGrad;
    tempGrad << grad[0], grad[1], grad[2], grad[3], grad[4], grad[5], grad[6], grad[7], grad[8]; 
    gradient = tempGrad;


    bfield *= m_bFieldUnit; // kT -> T;
    gradient *= m_bFieldUnit;

    return bfield;
  }

  Acts::Vector3D
  getFieldGradient(const Acts::Vector3D& position,
                   Acts::ActsMatrixD<3, 3>& gradient,
                   Cache& /*cache*/) const
  {
    return getFieldGradient(position, gradient);
  }

private:

  MagField::AtlasFieldCache getFieldCache() const {
    SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, Gaudi::Hive::currentContext()};
    if (!readHandle.isValid()) {
       std::stringstream msg;
       msg << "Failed to retrieve magmnetic field conditions data " << m_fieldCacheCondObjInputKey.key() << ".";
       throw std::runtime_error(msg.str());
    }
    const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};

    MagField::AtlasFieldCache fieldCache;
    fieldCondObj->getInitializedCache (fieldCache);

    return fieldCache;
  }

  // TODO: store pointer instead?
  SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey;

  const double m_bFieldUnit = 1000.*Acts::UnitConstants::T;
};


#endif
