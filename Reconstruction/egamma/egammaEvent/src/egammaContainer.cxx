/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     egammaContainer.cxx
PACKAGE:  offline/Reconstruction/egammaEvent

AUTHORS:  H. Ma
CREATED:  Nov. 2000

PURPOSE:  This is the Container Class for egamma Objects.
Updated:  Moved to egammaEvent 
          May 5, 2003    (MW)
          make code compliant for gcc-3.2
********************************************************************/ 

// INCLUDE HEADER FILES:

#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/egamma.h"
#include <iomanip>
#include <iostream>

//  END OF HEADER FILES INCLUDE

egammaContainer::egammaContainer(SG::OwnershipPolicy ownPolicy ) : 
  DataVector<egamma>(ownPolicy)
{}

// print objects found in container
void egammaContainer::print()
{ 

  DataVector<egamma>::const_iterator c_itr;

  for (c_itr=begin(); c_itr!=end(); ++c_itr) 
  {
    (*c_itr)->print();
  }

}
