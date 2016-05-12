/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DetectorElement_H
#define DetectorElement_H

#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "FadsMaterial/MaterialManager.h"

#include <iostream>

namespace FADS {

class DetectorElement {
public:
	G4String name;
	MaterialManager *matManager;
	
	DetectorElement(G4String n): name(n)
	{
//		cout<<"New detector element created "<<name<<endl;
		matManager=MaterialManager::GetMaterialManager();
	}
	virtual G4LogicalVolume* build()=0;
	virtual void print() {cout<<"generic detector element"<<endl;}
	virtual ~DetectorElement() {}

};

}	// end namespace

#endif
