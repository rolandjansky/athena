/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// DetectorDescription class for Muon Wall
//
// Author: Franck Martin <Franck.Martin@cern.ch>
// January 12, 2004
//
//************************************************************

#include "MuonWall/MuonWallDescription.hh"
#include "TileG4DetDescr/DetectorDescriptionStore.h"

MuonWallDescription::MuonWallDescription(std::string n):  
  DetectorDescription(n),
  zLength(0),
  yLength(0),
  xLength(0)
{
}

