// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TruthD3PDMaker/src/TruthParticleChildAssociationTool.h
 * @author Ryan Reece  <ryan.reece@cern.ch>
 * @date Mar, 2010
 * @brief Associate children of TruthParticles.
 */


#ifndef TRUTHD3PDMAKER_TRUTHPARTICLECHILDASSOCIATIONTOOL_H
#define TRUTHD3PDMAKER_TRUTHPARTICLECHILDASSOCIATIONTOOL_H


#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "xAODTruth/TruthParticle.h"
#include <vector>


namespace D3PD {


/**
 * @brief Associate all particles within a DR cut.
 *
 * This is a multiple association tool.
 * Given an @c TruthParticle object return all of its children.
 *
 */
class TruthParticleChildAssociationTool
  : public MultiAssociationTool<xAOD::TruthParticle>
{
public:
  typedef MultiAssociationTool<xAOD::TruthParticle> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TruthParticleChildAssociationTool (const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize() override;


  /**
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  virtual StatusCode reset (const xAOD::TruthParticle& p) override;


  /**
   * @brief Return a pointer to the next element in the association.
   *
   * Return 0 when the association has been exhausted.
   */
  virtual const xAOD::TruthParticle* next() override;


private:
  /// Vector of xAOD pointers.
  std::vector<const xAOD::TruthParticle*> m_xaod_children;
  
  /// Current index.
  unsigned int m_i; 
};


} // namespace D3PD


#endif // not TRUTHD3PDMAKER_TRUTHPARTICLECHILDASSOCIATIONTOOL_H

