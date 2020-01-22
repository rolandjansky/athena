/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EnergyParticleOrderingTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "EnergyParticleOrderingTool.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"

/** Constructor **/
ISF::EnergyParticleOrderingTool::EnergyParticleOrderingTool(const std::string& t, const std::string& n, const IInterface* p) :
  base_class(t,n,p)
{
}

/** Destructor **/
ISF::EnergyParticleOrderingTool::~EnergyParticleOrderingTool()
{
}

/** Compute particle order */
ISF::ParticleOrder ISF::EnergyParticleOrderingTool::computeOrder(const ISFParticle &p) const
{
  // Multiply to make use of float info
  return (int)(p.ekin() * 100.0);
}
