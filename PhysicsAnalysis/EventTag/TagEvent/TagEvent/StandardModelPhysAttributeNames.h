/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SMPHYSATTRIBUTENAMES_H
#define SMPHYSATTRIBUTENAMES_H

/****************************************
 Author - Ketevi A. Assamagan
 Feb 2006
****************************************/
#include "GlobalGroupAndUnitNames.h"
#include <string>

/** list of ALL the SM attributes and their types */
namespace SMPhys {
  static const int NsmAttr=1;         // This is the number of SM TAG attributes
  static const int NumSmPhys = 1;     // This is max number of SM words in the TAG per some attributes
}

/** This is the list of SM TAG attributes */
static const std::string SmAttributeNames[SMPhys::NsmAttr] = {"SMWord"};

/** This is the list of SM TAG attributes unit names*/
static const std::string SmAttributeUnitNames[SMPhys::NsmAttr] = {UnitNames[Unt::Bitmask]};


/** This is the list of SM TAG attributes group names*/
static const std::string SmAttributeGroupNames[SMPhys::NsmAttr] = {GroupNames[Grp::QualityPhysics]};

#endif


