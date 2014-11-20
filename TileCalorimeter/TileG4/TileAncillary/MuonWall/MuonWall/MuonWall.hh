/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class MuonWall
// Class implementing geometry of the scintillator Muon Wall
//
// Author: Franck Martin<Franck.Martin@cern.ch>
// January 12, 2004
//
//************************************************************

#ifndef MuonWall_H
#define MuonWall_H

#include "FadsGeometry/DetectorFacility.h"
using namespace FADS;
class MuonWallDescription;
class MuonWallSD;

class MuonWall: public DetectorFacility {
private:
  MuonWallDescription* muonwall;

public:
  MuonWall(G4String s);
  ~MuonWall();
	
  void UseDescription(std::string dname);
  void BuildGeometry();
};
#endif
