/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// DetectorDescription class for Phantom Barrel
//
// Author: Franck Martin <Franck.Martin@cern.ch>
// December 15th, 2003
//
//************************************************************

#include "PhantomCalorimeter/PhantomBarrelDescription.hh"
#include "TileG4DetDescr/DetectorDescriptionStore.h"

PhantomBarrelDescription::PhantomBarrelDescription(std::string n):  
  DetectorDescription(n),
  zLength(0),
  rMin(0),
  rMax(0)
{
}







