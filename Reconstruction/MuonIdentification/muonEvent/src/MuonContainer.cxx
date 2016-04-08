/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////////////
///
/// Name:     MuonContainer.h
/// Package : offline/Reconstruction/MuonIdentification/muonEvent
///
/// Authors:  K. A. Assamagan
/// Created:  December 2004
///
/// Purpose:  This is a data object, containing a collection of Combined Muon Objects
/////////////////////////////////////////////////////////////////////////////////////

#include "muonEvent/MuonContainer.h"
#include <iomanip>
#include <iostream>

namespace Analysis {

// print objects found in container
void MuonContainer::print()
{ 
  std::cout << "Number of Muon objects in container " 
	    << this->size() << std::endl;
}

}




