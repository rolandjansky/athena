/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class LArCrate
// Class implementing geometry of the LArCrate in Tile Gap
//
// Author: Franck Martin<Franck.Martin@cern.ch>
// November 18, 2004
//
//************************************************************

#ifndef LArCrate_H
#define LArCrate_H

#include "FadsGeometry/DetectorFacility.h"
using namespace FADS;
class LArCrateDescription;
class IMessageSvc;

class LArCrate: public DetectorFacility {
private:
  LArCrateDescription* m_larcrate;
  IMessageSvc* m_msgSvc;
public:
  LArCrate(G4String s);
  ~LArCrate();
	
  void UseDescription(std::string dname);
  void BuildGeometry();
};
#endif
