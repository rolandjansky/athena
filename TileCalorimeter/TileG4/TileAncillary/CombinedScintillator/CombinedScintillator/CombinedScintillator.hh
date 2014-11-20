/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class CombinedScintillator
// Class implementing geometry of the scintillator between Lar/Tile
//
// Author: Franck Martin<Franck.Martin@cern.ch>
// January 12, 2004
//
//************************************************************

#ifndef CombinedScintillator_H
#define CombinedScintillator_H

#include "FadsGeometry/DetectorFacility.h"
using namespace FADS;
class CombinedScintillatorDescription;
class CombinedScintillatorSD;

class CombinedScintillator: public DetectorFacility {
private:
  CombinedScintillatorDescription* tsci;

public:
  CombinedScintillator(G4String s);
  ~CombinedScintillator();
	
  void UseDescription(std::string dname);
  void BuildGeometry();
};
#endif
