/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXOTICPHYSATTRIBUTENAMES_H
#define EXOTICPHYSATTRIBUTENAMES_H

/****************************************
 Author - Ketevi A. Assamagan
 Feb 2006
****************************************/
#include "GlobalGroupAndUnitNames.h"
#include <string>

/** list of ALL the Higgs attributes and their types */
namespace ExoticPhys {
  static const int NexoticAttr=1;       // This is the number of Exotic TAG attributes
  static const int NumExoticPhys = 1;   // This is the max number of Exotic TAG words per some attributes
}

/** This is the list of the exotic phys TAG attributes */
static const std::string ExoticAttributeNames[ExoticPhys::NexoticAttr] = {"ExoticWord"};

/** This is the list of the exotic phys TAG attributes Unit names*/
static const std::string ExoticAttributeUnitNames[ExoticPhys::NexoticAttr] = {UnitNames[Unt::Bitmask]};

/** This is the list of the exotic phys TAG attributes Group names*/
static const std::string ExoticAttributeGroupNames[ExoticPhys::NexoticAttr] = {GroupNames[Grp::QualityPhysics]};
#endif


