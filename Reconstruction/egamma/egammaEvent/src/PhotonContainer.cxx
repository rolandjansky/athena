/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     PhotonContainer.cxx
PACKAGE:  offline/Reconstruction/egammaEvent

********************************************************************/ 

// INCLUDE HEADER FILES:

#include "egammaEvent/PhotonContainer.h"
#include "egammaEvent/Photon.h"
#include <iomanip>
#include <iostream>

//  END OF HEADER FILES INCLUDE

PhotonContainer::PhotonContainer(SG::OwnershipPolicy ownPolicy ) : 
  DataVector<Analysis::Photon>(ownPolicy),
  m_ownPolicy(ownPolicy)
{}

// push_back with const_cast (temporary)
void PhotonContainer::push_back(const Analysis::Photon* Ph)
{

  if (m_ownPolicy==SG::OWN_ELEMENTS) {
    std::cout << "ERROR Should not push_back const cell in OWN egammaContainer." << std::endl;
  }

  // have to const_cast because egammaContainer is a DataVector<egamma> not const Photon
  Analysis::Photon* theNonConstPh = const_cast<Analysis::Photon*>(Ph);
  
  DataVector<Analysis::Photon>::push_back(theNonConstPh) ;
}

void PhotonContainer::push_back(Analysis::Photon* Ph)
{
  DataVector<Analysis::Photon>::push_back(Ph) ;
}

// print objects found in container
void PhotonContainer::print()
{ 

  DataVector<Analysis::Photon>::const_iterator c_itr;

  std::cout << "Number of Photon objects in container " 
	    << this->size() << std::endl;

  for (c_itr=begin(); c_itr!=end(); c_itr++) 
  {
    (*c_itr)->print();
  }
}
