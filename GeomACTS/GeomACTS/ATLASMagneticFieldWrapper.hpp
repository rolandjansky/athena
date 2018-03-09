#ifndef GeomACTS_ATLASMagneticFieldWrapper_h
#define GeomACTS_ATLASMagneticFieldWrapper_h

#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "ACTS/Utilities/Definitions.hpp"
#include "ACTS/MagneticField/concept/AnyFieldLookup.hpp"

class ATLASMagneticFieldWrapper
{


public:

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
    : m_fieldCell(fieldService)
  {
  }

  Acts::Vector3D
  getField(const Acts::Vector3D& pos) const
  {
    return m_fieldCell.getField(pos);
  }
  
  Acts::Vector3D
  getFieldGradient(const Acts::Vector3D& position, Acts::ActsMatrixD<3, 3>& gradient) const
  {
    return m_fieldCell.getFieldGradient(position, gradient);
  }


  Acts::concept::AnyFieldCell<>
  getFieldCell(const Acts::Vector3D& /*position*/) const
  {
    return m_fieldCell;
  }
        
private:
  FieldCell m_fieldCell;
};


#endif
