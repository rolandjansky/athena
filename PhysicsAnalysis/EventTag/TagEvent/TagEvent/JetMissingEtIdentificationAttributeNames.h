/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMISSINGETIDENTIFICATIONATTRIBUTENAMES_H
#define JETMISSINGETIDENTIFICATIONATTRIBUTENAMES_H

/****************************************
 Author - Ketevi A. Assamagan
 Feb 2006
****************************************/
#include "GlobalGroupAndUnitNames.h"
#include <string>

/** list of ALL the Jet and Missing ET ID attributes */
namespace JetMissingEt {
  static const int NjetMetAttr=1;          // This is the number of JetMissingEt ID tag attributes
  static const int NumJetMissingEtID = 1;  // This the max number of JetMissingET ID words in the TAG per some attributes
}
 
/** This is the list of the JetMissingET ID TAG attributes */
static const std::string JetMissingEtAttributeNames[JetMissingEt::NjetMetAttr] = {"JetMissingETWord"};

/** This is the list of the JetMissingET ID TAG attributes unit names*/
static const std::string JetMissingEtAttributeUnitNames[JetMissingEt::NjetMetAttr] = {UnitNames[Unt::Bitmask]};

/** This is the list of the JetMissingET ID TAG attributes group names*/
static const std::string JetMissingEtAttributeGroupNames[JetMissingEt::NjetMetAttr] = {GroupNames[Grp::QualityPhysics]};

#endif


