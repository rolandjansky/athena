/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IHECGeometry.h
// Common interface for Geometry Calculator classes

#ifndef __LARG4HEC_IHECGEOMETRY_H__
#define __LARG4HEC_IHECGEOMETRY_H__

#include "GaudiKernel/IService.h"

//#include "globals.hh"

//#include "LArG4Code/LArG4Identifier.h"

#include <string>

class G4Step;
class LArG4Identifier;

namespace LArG4
{
  namespace HEC
  {

    enum eHECGeometryType { kWheelActive, kWheelInactive, kWheelDead };

    class IHECGeometry : virtual public IService
    {
    public:
      IHECGeometry() {}
      virtual ~IHECGeometry() {}

      static const InterfaceID& interfaceID() {
        static const InterfaceID  IID_IHECGeometry("IHECGeometry",1,0);
        return IID_IHECGeometry;
      }

      virtual LArG4Identifier CalculateIdentifier( const G4Step* a_step, const eHECGeometryType type = kWheelActive, int *subgap=NULL) const = 0;

    };

  }
}
#endif //__LARG4HEC_IHECGEOMETRY_H__
