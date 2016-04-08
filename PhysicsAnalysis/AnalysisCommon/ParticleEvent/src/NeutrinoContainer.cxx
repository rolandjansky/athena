/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////
///
/// NAME:     NeutrinoContainer.cxx
/// Package : offline/PhysicsAnalysis/AnalysisCommon/ParticleEvent
///
/// AUTHOR:  Ketevi A. Assamagan
/// CREATED:  August 2004
///
/// PURPOSE:  This is the Container Class for Neutrino Objects
//////////////////////////////////////////////////////////////////////

// INCLUDE HEADER FILES:

#include "ParticleEvent/NeutrinoContainer.h"
#include "ParticleEvent/Neutrino.h"
#include <iomanip>
#include <iostream>

//  END OF HEADER FILES INCLUDE

// print objects found in container
void NeutrinoContainer::print()
{ 

  std::cout << "Number of Neutrino objects in container " 
	    << this->size() << std::endl;


}
