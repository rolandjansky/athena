/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "KLFitterExtras/InterfaceBase.h" 
#include "KLFitter/Particles.h" 

// --------------------------------------------------------- 
KLFitter::InterfaceBase::InterfaceBase() :
  fParticles(0),
  fParticlesTruth(0),  
  fWeight(0)
{
}

// --------------------------------------------------------- 
KLFitter::InterfaceBase::~InterfaceBase()
{
  if (fParticles) 
    delete fParticles; 

  if (fParticlesTruth)
    delete fParticlesTruth; 
}

// --------------------------------------------------------- 

