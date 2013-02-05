/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HEAVYIONPHYSATTRIBUTENAMES_H
#define HEAVYIONPHYSATTRIBUTENAMES_H

/****************************************
 Author - Ketevi A. Assamagan
 June 2006
****************************************/

#include "GlobalGroupAndUnitNames.h"
#include <string>

/** list of ALL the HeavyIon Phys attributes and their types */
namespace HeavyIonPhys {
  static const int NheavyIonAttr=1;       // This is the number of HeavyIon TAG attributes
  static const int NumHeavyIonPhys = 1;   // This is the max number of HeavyIon TAG words per some attributes
}

/** This is the list of the heavyIon phys TAG attributes */
static const std::string HeavyIonAttributeNames[HeavyIonPhys::NheavyIonAttr] = {"HeavyIonWord"};

/** This is the list of the heavyIon phys TAG attributes unit names*/
static const std::string HeavyIonAttributeUnitNames[HeavyIonPhys::NheavyIonAttr] = {UnitNames[Unt::Bitmask]};

/** This is the list of the heavyIon phys TAG attributes group names*/
static const std::string HeavyIonAttributeGroupNames[HeavyIonPhys::NheavyIonAttr] = {GroupNames[Grp::QualityPhysics]};

#endif


