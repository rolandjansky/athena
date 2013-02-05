/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUIDENTIFICATIONATTRIBUTENAMES_H
#define TAUIDENTIFICATIONATTRIBUTENAMES_H

/****************************************
 Author - Ketevi A. Assamagan
 Feb 2006
****************************************/

#include "GlobalGroupAndUnitNames.h"
#include <string>

/** list of ALL the Tau ID attributes and their types */
namespace TauID {
  static const int NtauIdAttr=1;  // This is the number of Tau TAG Attributes
  static const int NumTauID = 1;  // This is the max number if Tau ID words in the TAG per some attributes
}

/** this is the list of the Tau ID TAG attributes */
static const std::string TauIdAttributeNames[TauID::NtauIdAttr] = {"TauIdWord"};

/** this is the list of the Tau ID TAG attributes unit names */
static const std::string TauIdAttributeUnitNames[TauID::NtauIdAttr] = {UnitNames[Unt::Bitmask]};

/** this is the list of the Tau ID TAG attributes group names*/
static const std::string TauIdAttributeGroupNames[TauID::NtauIdAttr] = {GroupNames[Grp::QualityPhysics]};

#endif


