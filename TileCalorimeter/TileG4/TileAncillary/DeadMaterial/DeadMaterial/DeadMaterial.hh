/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class DeadMaterial
// Class implementing geometry of the DeadMaterial
//
// Author: Franck Martin<Franck.Martin@cern.ch>
// July 12, 2004
//
//************************************************************

#ifndef DeadMaterial_H
#define DeadMaterial_H

#include "FadsGeometry/DetectorFacility.h"
using namespace FADS;
class DeadMaterialDescription;
class IMessageSvc;

class DeadMaterial: public DetectorFacility {
private:
  DeadMaterialDescription* m_deadmaterial;
  IMessageSvc* m_msgSvc;

public:
  DeadMaterial(G4String s);
  ~DeadMaterial();
	
  void UseDescription(std::string dname);
  void BuildGeometry();
};
#endif
