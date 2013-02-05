/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAATTRIBUTENAMES_H
#define EGAMMAATTRIBUTENAMES_H

/****************************************
 Author - Ketevi A. Assamagan
 Feb 2006
****************************************/

#include "GlobalGroupAndUnitNames.h"
#include <string>

/** list of ALL the Egamma ID attributes */
namespace Egamma { 
   static const int NegAttr = 1;      // This number of Egamma TAG attributes
   static const int NumEgamma = 1;    // This maximum number of Egamma words in the TAG per some attributes
}

/** This is the list of the Egamma TAG attributes */
static const std::string EgammaAttributeNames[Egamma::NegAttr] = {"EgammaWord"};


/** This is the list of the TAG attribute Unit names */
static const std::string EgammaAttributeUnitNames[Egamma::NegAttr] = {UnitNames[Unt::Bitmask]};


/** This is the list of the TAG attribute Group names */
static const std::string EgammaAttributeGroupNames[Egamma::NegAttr] = {GroupNames[Grp::QualityPhysics]};

#endif


