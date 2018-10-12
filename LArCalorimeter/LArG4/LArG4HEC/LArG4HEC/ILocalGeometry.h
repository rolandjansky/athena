/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ILocalGeometry.h

#ifndef __LARG4HEC_ILOCALGEOMETRY_H__
#define __LARG4HEC_ILOCALGEOMETRY_H__

#include "GaudiKernel/IService.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <string>

class G4Step;
class LArG4Identifier;

namespace LArG4
{
  namespace HEC
  {

    enum eLocalGeometryType { kLocActive, kLocInactive, kLocDead };

    class ILocalGeometry : virtual public IService
    {
    public:
      ILocalGeometry() {}
      virtual ~ILocalGeometry() {}

      DeclareInterfaceID(ILocalGeometry,1,0);

      virtual LArG4Identifier CalculateIdentifier( const G4Step* a_step, const eLocalGeometryType type = kLocActive,
                                                   int depthadd = 0, double deadzone = 4.*CLHEP::mm, double locyadd = 0.*CLHEP::mm) const = 0;

    };

  }
}
#endif //__LARG4HEC_ILOCALGEOMETRY_H__
