/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// DetectorDescription class for LAR Crate
//
// Author: Franck Martin <Franck.Martin@cern.ch>
// November 19, 2004
//
//************************************************************

#include "LarCrate/LArCrateDescription.hh"
#include "TileG4DetDescr/DetectorDescriptionStore.h"

LArCrateDescription::LArCrateDescription(std::string n):  
  DetectorDescription(n),
  zLength(0),
  yLength(0),
  xLength(0)
{
}







