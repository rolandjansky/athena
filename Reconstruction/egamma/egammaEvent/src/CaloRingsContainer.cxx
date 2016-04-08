/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// INCLUDE HEADER FILES:

#include "egammaEvent/CaloRingsContainer.h"
#include "egammaEvent/CaloRings.h"
#include <iomanip>
#include <iostream>

//  END OF HEADER FILES INCLUDE

CaloRingsContainer::CaloRingsContainer(SG::OwnershipPolicy ownPolicy ) : 
  DataVector<CaloRings>(ownPolicy),
  m_ownPolicy(ownPolicy)
{}

// push_back with const_cast (temporary)
void CaloRingsContainer::push_back(const CaloRings * eg)
{

  if (m_ownPolicy==SG::OWN_ELEMENTS){
    std::cout << "ERROR Should not push_back const cell in OWN egammaContainer." << std::endl;
  }

  // have to const_cast because egammaContainer is a DataVector<egamma> not const egamma
  CaloRings * theNonConsteg = const_cast<CaloRings*>(eg);
  
  DataVector<CaloRings>::push_back(theNonConsteg) ;
}

void CaloRingsContainer::push_back(CaloRings * eg)
{
  DataVector<CaloRings>::push_back(eg) ;
}

