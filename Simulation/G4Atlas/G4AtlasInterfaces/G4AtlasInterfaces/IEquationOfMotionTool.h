/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_IEQUATIONOFMOTIONTOOL_H
#define G4ATLASINTERFACES_IEQUATIONOFMOTIONTOOL_H

#include "GaudiKernel/IAlgTool.h"

//#include "G4MagneticField.hh"
//#include "G4Mag_EqRhs.hh"
class G4Mag_EqRhs;
class G4MagneticField;

/** @class IEquationOfMotionTool IEquationOfMotionTool.h "G4AtlasInterfaces/IEquationOfMotionTool.h"
 *
 *  Abstract interface to Geant4 Equation of motion class
 *
 */

class IEquationOfMotionTool : virtual public IAlgTool
{
public:
  IEquationOfMotionTool() {}
  virtual ~IEquationOfMotionTool() {}
  static const InterfaceID& interfaceID() {
    static const InterfaceID IID_IEquationOfMotionTool( "IEquationOfMotionTool" , 1 , 0 ) ;
    return IID_IEquationOfMotionTool ;
  }

  // Method needed to create G4Mag_EqRhs
  virtual G4Mag_EqRhs* makeEquationOfMotion(G4MagneticField *emField) const = 0 ;
};
#endif
