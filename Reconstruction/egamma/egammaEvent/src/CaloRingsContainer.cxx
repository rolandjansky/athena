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
  DataVector<CaloRings>(ownPolicy)
{}

