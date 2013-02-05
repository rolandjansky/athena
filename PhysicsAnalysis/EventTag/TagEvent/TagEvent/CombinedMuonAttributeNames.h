/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COMBINEDMUONATTRIBUTENAMES_H
#define COMBINEDMUONATTRIBUTENAMES_H

/****************************************
 Author - Ketevi A. Assamagan
 Feb 2006
****************************************/

#include "GlobalGroupAndUnitNames.h"
#include <string>

/** list of ALL the Muon ID attributes */
namespace MuonID {
  static const int NcombMuonAttr=1;    // This number of combined muon TAG attributes
  static const int NumCombinedMuon=1;  // This the max number of combined muon words in the TAG per some attributes
}

/** This is the list of the Combined Muon TAG attributes */
static const std::string CombinedMuonAttributeNames[MuonID::NcombMuonAttr] = {"CombinedMuonWord"};


/** This is the list of the Combined Muon TAG attributes Unit names */
static const std::string CombinedMuonAttributeUnitNames[MuonID::NcombMuonAttr] = {UnitNames[Unt::Bitmask]};

/** This is the list of the Combined Muon TAG attributes Group names */
static const std::string CombinedMuonAttributeGroupNames[MuonID::NcombMuonAttr] = {GroupNames[Grp::QualityPhysics]};

#endif

