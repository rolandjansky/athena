/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// DetectorDescription class for PhantomBarrel
//
// Author: Franck Martin <Franck.Martin@cern.ch>
// December 15th, 2003
//
//************************************************************


#ifndef PhantomBarrelDescription_H 
#define PhantomBarrelDescription_H

#include "TileG4DetDescr/DetectorDescription.h"

class PhantomBarrelDescription: public FADS::DetectorDescription 
{
public:
  double zLength;       // Total length of the phantom barrel along Z and Y  axis
  double rMin;          // Minimal radius of the phantom barrel
  double rMax;          // Maximal radius of the phantom barrel

  PhantomBarrelDescription(std::string n);
};

#endif

