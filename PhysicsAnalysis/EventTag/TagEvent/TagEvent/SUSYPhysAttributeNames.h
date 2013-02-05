/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SUSYPHYSATTRIBUTENAMES_H
#define SUSYPHYSATTRIBUTENAMES_H

/****************************************
 Author - Ketevi A. Assamagan
 Feb 2006
****************************************/
#include "GlobalGroupAndUnitNames.h"
#include <string>

/** list of ALL the SUSY attributes and their types */
namespace SUSYPhys {
  static const int NsusyAttr=1;        // This the number of SUSY Phys TAG attributes
  static const int NumSusyPhys = 1;    // This is the max number of SUSY Phys words in the TAG per some Attributes
}

/** This is the list of SUSY Phys TAG attributes */
static const std::string SusyAttributeNames[SUSYPhys::NsusyAttr] = {"SUSYWord"};

/** This is the list of SUSY Phys TAG attributes */
static const std::string SusyAttributeUnitNames[SUSYPhys::NsusyAttr] = {UnitNames[Unt::Bitmask]};

/** This is the list of SUSY Phys TAG attributes */
static const std::string SusyAttributeGroupNames[SUSYPhys::NsusyAttr] = {GroupNames[Grp::QualityPhysics]};

#endif


