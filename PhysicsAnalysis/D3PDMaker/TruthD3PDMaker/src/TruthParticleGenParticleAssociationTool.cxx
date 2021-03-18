/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TruthD3PDMaker/TruthParticleGenParticleAssociationTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2010
 * @brief Associate from a TruthParticle to a corresponding GenParticle.
 */


#include "TruthParticleGenParticleAssociationTool.h"
#include "McParticleEvent/TruthParticle.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TruthParticleGenParticleAssociationTool::TruthParticleGenParticleAssociationTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
{
}


/**
 * @brief Return the target object.
 * @param p The source object for the association.
 *
 * Return the target of the association, or 0.
 */
#ifdef HEPMC3
const HepMC::GenParticle*
TruthParticleGenParticleAssociationTool::get (const TruthParticle& p)
{	  
return p.genParticle().get();
}
#else
const HepMC::GenParticle*
TruthParticleGenParticleAssociationTool::get (const TruthParticle& p)
{
  return p.genParticle();
}
#endif


} // namespace D3PD
