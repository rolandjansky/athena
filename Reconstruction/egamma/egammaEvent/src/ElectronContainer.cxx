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
  DataVector<Analysis::Electron>(ownPolicy),
  m_ownPolicy(ownPolicy)
{}

// push_back with const_cast (temporary)
void ElectronContainer::push_back(const Analysis::Electron* El)
{

  if (m_ownPolicy==SG::OWN_ELEMENTS) {
    std::cout << "ERROR Should not push_back const cell in OWN egammaContainer." << std::endl;
  }

  // have to const_cast because egammaContainer is a DataVector<egamma> not const Electron
  Analysis::Electron* theNonConstEl = const_cast<Analysis::Electron*>(El);
  
  DataVector<Analysis::Electron>::push_back(theNonConstEl) ;
}

void ElectronContainer::push_back(Analysis::Electron* El)
{
  DataVector<Analysis::Electron>::push_back(El) ;
}


// print objects found in container
void ElectronContainer::print()
{ 

  DataVector<Analysis::Electron>::const_iterator c_itr;

  std::cout << "Number of Electron objects in container " 
	    << this->size() << std::endl;

  for (c_itr=begin(); c_itr!=end(); c_itr++) 
  {
    (*c_itr)->print();
  }
}
