/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// DetectorDescription class for Scintillator between LAr/Tile
//
// Author: Franck Martin <Franck.Martin@cern.ch>
// January 13, 2004
//
//************************************************************

#include "CombinedScintillator/CombinedScintillatorDescription.hh"
#include "TileG4DetDescr/DetectorDescriptionStore.h"

CombinedScintillatorDescription::CombinedScintillatorDescription(std::string n):  
  DetectorDescription(n),
  rMin(0),
  rMax(0),
  dzSci(0),
  phiPos(0),
  phiNeg(0)
{
}

