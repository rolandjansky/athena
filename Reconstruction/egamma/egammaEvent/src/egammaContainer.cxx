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
  DataVector<egamma>(ownPolicy),
  m_ownPolicy(ownPolicy)
{}

// push_back with const_cast (temporary)
void egammaContainer::push_back(const egamma * eg)
{

  if (m_ownPolicy==SG::OWN_ELEMENTS){
    std::cout << "ERROR Should not push_back const cell in OWN egammaContainer." << std::endl;
  }

  // have to const_cast because egammaContainer is a DataVector<egamma> not const egamma
  egamma * theNonConsteg = const_cast<egamma*>(eg);
  
  DataVector<egamma>::push_back(theNonConsteg) ;
}

void egammaContainer::push_back(egamma * eg)
{
  DataVector<egamma>::push_back(eg) ;
}

// print objects found in container
void egammaContainer::print()
{ 

  DataVector<egamma>::const_iterator c_itr;

  //std::cout << "Number of egamma objects in container " << this->size() << std::endl;

  for (c_itr=begin(); c_itr!=end(); c_itr++) 
  {
    (*c_itr)->print();
  }

}
