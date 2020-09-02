// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TruthD3PDMaker/TruthParticleGenParticleAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2010
 * @brief Associate from a TruthParticle to the corresponding GenParticle.
 */


#ifndef TRUTHD3PDMAKER_TRUTHPARTICLEGENPARTICLEASSOCIATIONTOOL_H
#define TRUTHD3PDMAKER_TRUTHPARTICLEGENPARTICLEASSOCIATIONTOOL_H


#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "McParticleEvent/TruthParticle.h"
#include "AtlasHepMC/GenParticle.h"


namespace D3PD {


/**
 * @brief Associate from a TruthParticle to the corresponding GenParticle.
 */
class TruthParticleGenParticleAssociationTool
  : public SingleAssociationTool<TruthParticle, HepMC::GenParticle>
{
public:
  typedef SingleAssociationTool<TruthParticle, HepMC::GenParticle> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TruthParticleGenParticleAssociationTool (const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent);


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const HepMC::GenParticle* get (const TruthParticle& p);
};


} // namespace D3PD


#endif // not TRUTHD3PDMAKER_TRUTHPARTICLEGENPARTICLEASSOCIATIONTOOL_H
