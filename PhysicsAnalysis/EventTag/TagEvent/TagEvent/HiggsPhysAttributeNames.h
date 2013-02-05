/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIGGSPHYSATTRIBUTENAMES_H
#define HIGGSPHYSATTRIBUTENAMES_H

/****************************************
 Author - Ketevi A. Assamagan
 Feb 2006
****************************************/
#include "GlobalGroupAndUnitNames.h"
#include <string>

/** list of ALL the Higgs attributes and their types */
namespace HiggsPhys {
  static const int HiggsAttr=1;       // This is the number of Higgs Phys TAG attributes
  static const int NumHiggsPhys = 1;  // This is the max number of Higgs Phys words in the TAG per some attributes
}

/** This is the list of the Higgs Phys TAG attributes */
static const std::string HiggsAttributeNames[HiggsPhys::HiggsAttr] = {"HiggsWord"};

/** This is the list of the Higgs Phys TAG attributes unit names*/
static const std::string HiggsAttributeUnitNames[HiggsPhys::HiggsAttr] = {UnitNames[Unt::Bitmask]};

/** This is the list of the Higgs Phys TAG attributes group names*/
static const std::string HiggsAttributeGroupNames[HiggsPhys::HiggsAttr] = {GroupNames[Grp::QualityPhysics]};

#endif


