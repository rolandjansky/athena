/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGGINGATTRIBUTENAMES_H
#define JETTAGGINGATTRIBUTENAMES_H

/****************************************
 Author - Ketevi A. Assamagan
 Feb 2006
****************************************/

#include "GlobalGroupAndUnitNames.h"
#include <string>

/** list of ALL the Jet-Tagging attributes and their types */
namespace JetTagging {
  static const int NjetTagAttr=1;      // This is the number of JetTagging TAG attributes
  static const int NumJetTagging = 1;  // This is the max number of JetTagging TAG words per some attributes
}

/** This is the list of the JetTagging TAG attributes */
static const std::string JetTaggingAttributeNames[JetTagging::NjetTagAttr] = {"JetTagWord"};

/** This is the list of the JetTagging TAG attributes unit names*/
static const std::string JetTaggingAttributeUnitNames[JetTagging::NjetTagAttr] = {UnitNames[Unt::Bitmask]};

/** This is the list of the JetTagging TAG attributes unit names*/
static const std::string JetTaggingAttributeGroupNames[JetTagging::NjetTagAttr] = {GroupNames[Grp::QualityPhysics]};

#endif
