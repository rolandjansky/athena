/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SpacePointContainer.cxx
//   Implementation file for class SpacePointContainer
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 14/10/2003 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#include "TrkSpacePoint/SpacePointContainer.h"

// Constructor with parameters:
SpacePointContainer::SpacePointContainer(unsigned int max) :
  IdentifiableContainer<SpacePointCollection>(max)
{
}

// Constructor with parameters:
SpacePointContainer::SpacePointContainer(SpacePointCache *cache) :
  IdentifiableContainer<SpacePointCollection>(cache)
{
}

// Destructor:
SpacePointContainer::~SpacePointContainer()
{

}


