/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FadsElement_H
#define FadsElement_H

#include "G4Element.hh"
#include "FadsMaterial/MaterialManager.h"

namespace FADS {

class FadsElement: public G4Element {
public:
	FadsElement (std::string name,std::string symbol,double z,double a):
					G4Element(name,symbol,z,a)
	{
		MaterialManager* matManager=MaterialManager::GetMaterialManager();
		matManager->StoreElement(this);
	} 
};

}	// end namespace

#endif
