/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EMShowerMinimalContainer.cxx
PACKAGE:  offline/Reconstruction/egammaEvent

AUTHORS:  M.P. Casado
CREATED:  Sep 2003

PURPOSE:  This is the Container Class for EMShowerMinimal Objects.

********************************************************************/ 

// INCLUDE HEADER FILES:

#include "egammaEvent/EMShowerMinimalContainer.h"
#include "egammaEvent/EMShowerMinimal.h"
#include <iomanip>
#include <iostream>

//  END OF HEADER FILES INCLUDE

// print objects found in container
void EMShowerMinimalContainer::print()
{ 

  DataVector<EMShowerMinimal>::const_iterator c_itr;

  std::cout << "Number of EMShowerMinimal objects in container " 
	    << this->size() << std::endl;

  for (c_itr=begin(); c_itr!=end(); c_itr++) 
  {
    (*c_itr)->print();
  }

}
