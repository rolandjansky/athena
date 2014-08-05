// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TruthD3PDMaker/src/TruthParticleProdVertexAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2010
 * @brief Associate from a TruthParticle to its vertex.
 */


#ifndef TRUTHD3PDMAKER_TRUTHPARTICLEPRODVERTEXASSOCIATIONTOOL_H
#define TRUTHD3PDMAKER_TRUTHPARTICLEPRODVERTEXASSOCIATIONTOOL_H


#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"


namespace D3PD {


/**
 * @brief Associate from a TruthParticle to its GenVertex production vertex.
 */
class TruthParticleProdVertexAssociationTool
  : public SingleAssociationTool<xAOD::TruthParticle, xAOD::TruthVertex>
{
public:
  typedef SingleAssociationTool<xAOD::TruthParticle, xAOD::TruthVertex> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TruthParticleProdVertexAssociationTool (const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent);


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const xAOD::TruthVertex* get (const xAOD::TruthParticle& p);
};


} // namespace D3PD



#endif // not TRUTHD3PDMAKER_TRUTHPARTICLEPRODVERTEXASSOCIATIONTOOL_H
