/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     ElectronContainer.cxx
PACKAGE:  offline/Reconstruction/egammaEvent

********************************************************************/ 

// INCLUDE HEADER FILES:

#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/Electron.h"
#include <iomanip>
#include <iostream>

//  END OF HEADER FILES INCLUDE

ElectronContainer::ElectronContainer(SG::OwnershipPolicy ownPolicy ) : 
  DataVector<Analysis::Electron>(ownPolicy)
{}

// print objects found in container
void ElectronContainer::print()
{ 

  DataVector<Analysis::Electron>::const_iterator c_itr;

  std::cout << "Number of Electron objects in container " 
	    << this->size() << std::endl;

  for (c_itr=begin(); c_itr!=end(); ++c_itr) 
  {
    (*c_itr)->print();
  }
}
