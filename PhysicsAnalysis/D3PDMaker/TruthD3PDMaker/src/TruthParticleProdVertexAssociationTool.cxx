/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TruthD3PDMaker/src/TruthParticleProdVertexAssociationTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2010
 * @brief Associate from a TruthParticle to its GenVertex production vertex.
 */


#include "TruthParticleProdVertexAssociationTool.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TruthParticleProdVertexAssociationTool::TruthParticleProdVertexAssociationTool
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
const xAOD::TruthVertex*
TruthParticleProdVertexAssociationTool::get (const xAOD::TruthParticle& p)
{
  return p.prodVtx();
}


} // namespace D3PD
