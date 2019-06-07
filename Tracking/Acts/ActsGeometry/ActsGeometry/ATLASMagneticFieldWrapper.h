/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ATLASMAGNETICFIELDWRAPPER_H
#define ACTSGEOMETRY_ATLASMAGNETICFIELDWRAPPER_H

#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/MagneticFieldContext.hpp"

class ATLASMagneticFieldWrapper
{


public:

  struct Cache {
    // empty, no cache for now

    Cache(std::reference_wrapper<const Acts::MagneticFieldContext> /*mctx*/) {
      // does nothing, but is required
    }
  };

  // FieldCell is not needed anymore, keep it for backwards compatibility right now.
  struct FieldCell {
  public:
    FieldCell(MagField::IMagFieldSvc* fieldService)
      : m_fieldService(fieldService)
    {
    }

    Acts::Vector3D
    getField(const Acts::Vector3D& pos) const
    {
      Acts::Vector3D bfield;
      m_fieldService->getField(&pos, &bfield);

      bfield *= m_bFieldUnit; // kT -> T;

      return bfield;
    }

    Acts::Vector3D
    getFieldGradient(const Acts::Vector3D& position, Acts::ActsMatrixD<3, 3>& gradient) const
    {
      Acts::Vector3D bfield;
      m_fieldService->getField(&position, &bfield, &gradient);

      bfield *= m_bFieldUnit; // kT -> T;
      gradient *= m_bFieldUnit;

      return bfield;
    }

    inline
    bool isInside(const Acts::Vector3D&) const {
      return true;
    }

  private:
    MagField::IMagFieldSvc *m_fieldService;
    const double m_bFieldUnit = 1000.*Acts::units::_T;
  };

  ATLASMagneticFieldWrapper(MagField::IMagFieldSvc *fieldService)
    : m_fieldCell(fieldService),
      m_fieldService(fieldService)
  {
  }

  Acts::Vector3D
  getField(const Acts::Vector3D& pos) const
  {
    Acts::Vector3D bfield;
    m_fieldService->getField(&pos, &bfield);

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
    Acts::Vector3D bfield;
    m_fieldService->getField(&position, &bfield, &gradient);

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

  // only kept for backwards compatibility
  FieldCell
  getFieldCell(const Acts::Vector3D& /*position*/) const
  {
    return m_fieldCell;
  }

private:
  // only kept for backwards compatibility
  FieldCell m_fieldCell;

  MagField::IMagFieldSvc *m_fieldService;
  const double m_bFieldUnit = 1000.*Acts::units::_T;
};


#endif
