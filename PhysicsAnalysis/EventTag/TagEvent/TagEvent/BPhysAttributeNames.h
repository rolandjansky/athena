/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BPHYSANALYSISATTRIBUTENAMES_H
#define BPHYSANALYSISATTRIBUTENAMES_H

/****************************************
 Author - Ketevi A. Assamagan 
 Feb 2006
****************************************/

#include "GlobalGroupAndUnitNames.h"
#include <string>

/** list of ALL the BPhys attributes and their types */
namespace BPhys {
  static const int BphysAttr=1;    // This is the number of BPhys TAG attributes
  static const int NumBphys = 1;   // This is the max number of BPhys words in the TAG per some attributes
}

/** This is the list of the TAG attribute names */
static const std::string BPhysAttributeNames[BPhys::BphysAttr] = {"BPhysWord"};

/** This is the list of the TAG attribute Unit names */
static const std::string BPhysAttributeUnitNames[BPhys::BphysAttr] = {UnitNames[Unt::Bitmask]};

/** This is the list of the TAG attribute Group names */
static const std::string BPhysAttributeGroupNames[BPhys::BphysAttr] = {GroupNames[Grp::QualityPhysics]};

#endif


