/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ELECTRONATTRIBUTENAMES_H
#define ELECTRONATTRIBUTENAMES_H

/****************************************
 Author - Ketevi A. Assamagan
 Feb 2006
****************************************/
#include "GlobalGroupAndUnitNames.h"
#include <string>

/** list of ALL the Electron attributes  */
namespace ElectronID { 
  static const int NelecAttr = 6;                     // This the number of electron TAG attributes
  static const int NumElectron = 6;                   // this maximum number of electrons in the TAG
  enum { NElectron=0, Pt=1, Eta=2, Phi=3, Tight=4, Isol=5 };
}

/** This is the list of the Electorn TAG attributes */
static const std::string ElectronAttributeNames[ElectronID::NelecAttr] = {
                                                      "NLooseElectron",
                                                      "LooseElectronPt",
                                                      "LooseElectronEta",
                                                      "LooseElectronPhi",
                                                      "LooseElectronTightness",
                                                      "LooseElectronIsolation"
                                                    };

static const std::string ElectronAttributeUnitNames[ElectronID::NelecAttr] = {
                                                  UnitNames[Unt::Number],
                                                  UnitNames[Unt::Mevc],
                                                  UnitNames[Unt::Rap],
                                                  UnitNames[Unt::Rad],
                                                  UnitNames[Unt::Bitmask],
                                                  UnitNames[Unt::Bitmask]
                                                    };

static const std::string ElectronAttributeGroupNames[ElectronID::NelecAttr] = {
                                                 GroupNames[Grp::PhysicsElectron],
                                                 GroupNames[Grp::PhysicsElectron],
                                                 GroupNames[Grp::PhysicsElectron],
                                                 GroupNames[Grp::PhysicsElectron],
                                                 GroupNames[Grp::PhysicsElectron],
                                                 GroupNames[Grp::PhysicsElectron]
                                                    };

#endif
