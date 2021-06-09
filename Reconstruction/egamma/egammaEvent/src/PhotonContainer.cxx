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
  DataVector<Analysis::Photon>(ownPolicy)
{}

// print objects found in container
void PhotonContainer::print()
{ 

  DataVector<Analysis::Photon>::const_iterator c_itr;

  std::cout << "Number of Photon objects in container " 
	    << this->size() << std::endl;

  for (c_itr=begin(); c_itr!=end(); ++c_itr) 
  {
    (*c_itr)->print();
  }
}
