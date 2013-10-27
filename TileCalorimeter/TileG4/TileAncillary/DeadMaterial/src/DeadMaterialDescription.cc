/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// DetectorDescription class for Dead Material
//
// Author: Franck Martin <Franck.Martin@cern.ch>
// July 12, 2004
//
//************************************************************

#include "DeadMaterial/DeadMaterialDescription.hh"
#include "TileG4DetDescr/DetectorDescriptionStore.h"

DeadMaterialDescription::DeadMaterialDescription(std::string n):  
  DetectorDescription(n),
  zLength(0),
  yLength(0),
  xLength(0)
{
}







