/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class PhantomBarrel. 
// Class implementing geometry of the phantom Calorimeter
//
// Author: Franck Martin<Franck.Martin@cern.ch>
// December 15th, 2003
//
//************************************************************

#ifndef PhantomBarrel_H
#define PhantomBarrel_H

#include "FadsGeometry/DetectorFacility.h"
using namespace FADS;
class PhantomBarrelDescription;
class PhantomBarrelSD;

class PhantomBarrel: public DetectorFacility {
private:
  PhantomBarrelDescription* phantombar;

public:
  PhantomBarrel(G4String s);
  ~PhantomBarrel();
	
  void UseDescription(std::string dname);
  void BuildGeometry();
};
#endif
