/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOPPHYSATTRIBUTENAMES_H
#define TOPPHYSATTRIBUTENAMES_H

/****************************************
 Author - Ketevi A. Assamagan
 Feb 2006
****************************************/

#include "GlobalGroupAndUnitNames.h"
#include <string>

/** list of ALL the Top attributes and their types */
namespace TopPhys {
  static const int NtopAttr=1;        // This is the number of Top Phys TAG attributes
  static const int NumTopPhys = 1;    // This is the max number of Top Phys TAG words per some attributes
}

/** This is the list of the Top Phys TAG attributes */
static const std::string TopAttributeNames[TopPhys::NtopAttr] = {"TopWord"};

/** This is the list of the Top Phys TAG attributes unit names*/
static const std::string TopAttributeUnitNames[TopPhys::NtopAttr] = {UnitNames[Unt::Bitmask]};

/** This is the list of the Top Phys TAG attributes group names*/
static const std::string TopAttributeGroupNames[TopPhys::NtopAttr] = {GroupNames[Grp::QualityPhysics]};

#endif


